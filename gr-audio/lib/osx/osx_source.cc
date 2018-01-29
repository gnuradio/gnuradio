/* -*- c++ -*- */
/*
 * Copyright 2006-2011,2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "audio_registry.h"
#include "osx_source.h"

#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <stdexcept>

namespace gr {
  namespace audio {

    source::sptr
    osx_source_fcn(int sampling_rate,
                   const std::string& device_name,
                   bool ok_to_block)
    {
      return source::sptr
        (new osx_source(sampling_rate, device_name, ok_to_block));
    }

    static std::string
    default_device_name()
    {
      return prefs::singleton()->get_string
        ("audio_osx", "default_input_device", "built-in");
    }

    osx_source::osx_source
    (int sample_rate,
     const std::string& device_name,
     bool ok_to_block)
      : sync_block("audio_osx_source",
		   io_signature::make(0, 0, 0),
		   io_signature::make(0, 0, 0)),
        d_device_sample_rate(0.0), d_output_sample_rate(0.0),
	d_input_buffer_size_frames(0), d_input_buffer_size_bytes(0),
	d_output_buffer_size_frames(0), d_output_buffer_size_bytes(0),
	d_device_buffer_size_frames(0), d_device_buffer_size_bytes(0),
	d_lead_size_frames(0), d_lead_size_bytes(0),
	d_trail_size_frames(0), d_trail_size_bytes(0),
	d_extra_buffer_size_frames(0), d_extra_buffer_size_bytes(0),
	d_queue_sample_count(0), d_buffer_sample_count(0),
	d_n_available_input_frames(0), d_n_actual_input_frames(0),
	d_n_user_channels(0), d_n_dev_channels(0),
	d_ok_to_block(ok_to_block), d_pass_through(false),
	d_waiting_for_data(false), d_do_reset(false),
	d_hardware_changed(false), d_using_default_device(false),
	d_desired_name(device_name.empty() ? default_device_name()
		       : device_name),
	d_input_ad_id(0), d_input_au(0), d_input_buffer(0),
	d_output_buffer(0), d_audio_converter(0)
    {
      // set the desired output sample rate

      if(sample_rate <= 0) {
	GR_LOG_ERROR(d_logger, boost::format
		     ("Invalid Sample Rate: %d")
		     % sample_rate);
        throw std::invalid_argument("audio_osx_source");
      }
      else {
        d_output_sample_rate = (Float64)sample_rate;
      }

      // set up for audio input using the stored desired parameters

      setup();
    }

    void osx_source::setup()
    {
      OSStatus err = noErr;

      // set the default input audio device id to "unknown"

      d_input_ad_id = kAudioDeviceUnknown;

      // try to find the input audio device, if specified

      std::vector < AudioDeviceID > all_ad_ids;
      std::vector < std::string > all_names;

      osx::find_audio_devices
	(d_desired_name, true,
	 &all_ad_ids, &all_names);

      // check number of device(s) returned

      if (d_desired_name.length() != 0) {
	if (all_ad_ids.size() == 1) {

	  // exactly 1 match was found; see if it was partial

	  if (all_names[0].compare(d_desired_name) != 0) {

	    // yes: log the full device name
	    GR_LOG_INFO(d_logger, boost::format
			("Using input audio device '%s'.")
			% all_names[0]);

	  }

	  // store info on this device

	  d_input_ad_id = all_ad_ids[0];
	  d_selected_name = all_names[0];

	} else {

	  // either 0 or more than 1 device was found; get all input
	  // device names, print those, and error out.

	  osx::find_audio_devices("", true, NULL, &all_names);

	  std::string err_str("\n\nA unique input audio device name "
			      "matching the string '");
	  err_str += d_desired_name;
	  err_str += "' was not found.\n\n";
	  err_str += "The current known input audio device name";
	  err_str += ((all_names.size() > 1) ? "s are" : " is");
	  err_str += ":\n";
	  for (UInt32 nn = 0; nn < all_names.size(); ++nn) {
	    err_str += "    " + all_names[nn] + "\n";
	  }
	  GR_LOG_ERROR(d_logger, boost::format(err_str));
	  throw std::runtime_error("audio_osx_source::setup");

	}
      }

      // if no input audio device id was found or no specific device
      // name was provided, use the default input audio device id as
      // set in System Preferences.

      if (d_input_ad_id == kAudioDeviceUnknown) {

        UInt32 prop_size = (UInt32)sizeof(AudioDeviceID);
        AudioObjectPropertyAddress ao_address = {
	  kAudioHardwarePropertyDefaultInputDevice,
	  kAudioObjectPropertyScopeGlobal,
	  kAudioObjectPropertyElementMaster
        };

        err = AudioObjectGetPropertyData
	  (kAudioObjectSystemObject, &ao_address,
	   0, NULL, &prop_size, &d_input_ad_id);
	check_error_and_throw
	  (err, "Getting the default input audio device ID failed",
	   "audio_osx_source::setup");

	{
	  // retrieve the device name; max name length is 64 characters.

	  UInt32 prop_size = 65;
	  char c_name_buf[prop_size];
	  memset((void*)c_name_buf, 0, (size_t)prop_size);
	  --prop_size;

	  AudioObjectPropertyAddress ao_address = {
	    kAudioDevicePropertyDeviceName,
	    kAudioDevicePropertyScopeInput, 0
	  };

	  if ((err = AudioObjectGetPropertyData
	       (d_input_ad_id, &ao_address, 0, NULL,
		&prop_size, (void*)c_name_buf)) != noErr) {

	    check_error(err, "Unable to retrieve input audio device name");

	  } else {

	    GR_LOG_INFO(d_logger, boost::format
			 ("\n\nUsing input audio device '%s'.\n  ... "
			  "which is the current default input audio"
			  " device.\n  Changing the default input"
			  " audio device in the System Preferences"
			  " will \n  result in changing it here, too "
			  "(with an internal reconfiguration).\n") %
			std::string(c_name_buf));

	  }

	  d_selected_name = c_name_buf;

	}

	d_using_default_device = true;

      }

      // retrieve the total number of channels for the selected input
      // audio device

      osx::get_num_channels_for_audio_device_id
	(d_input_ad_id, &d_n_dev_channels, NULL);

      // set the block output signature, if not already set
      // (d_n_user_channels is set in check_topology, which is called
      // before the flow-graph is running)

      if (d_n_user_channels == 0) {
	set_output_signature(io_signature::make
			     (1, d_n_dev_channels, sizeof(float)));
      }

      // set the interim buffer size; to work with the GR scheduler,
      // must be at least 16kB.  Pick 50 kB since that's plenty yet
      // not very much.

      d_buffer_sample_count = (d_output_sample_rate < 50000.0 ?
			       50000 : (UInt32)d_output_sample_rate);

#if _OSX_AU_DEBUG_
      std::cerr << "source(): max # samples = "
		<< d_buffer_sample_count << std::endl;
#endif

      // create the default AudioUnit for input

      // Open the default input unit

#ifndef GR_USE_OLD_AUDIO_UNIT
      AudioComponentDescription desc;
#else
      ComponentDescription desc;
#endif

      desc.componentType = kAudioUnitType_Output;
      desc.componentSubType = kAudioUnitSubType_HALOutput;
      desc.componentManufacturer = kAudioUnitManufacturer_Apple;
      desc.componentFlags = 0;
      desc.componentFlagsMask = 0;

#ifndef GR_USE_OLD_AUDIO_UNIT

      AudioComponent comp = AudioComponentFindNext(NULL, &desc);
      if(!comp) {
        GR_LOG_FATAL(d_logger, boost::format
		     ("AudioComponentFindNext Failed"));
        throw std::runtime_error("audio_osx_source::setup");
      }
      err = AudioComponentInstanceNew(comp, &d_input_au);
      check_error_and_throw(err, "AudioComponentInstanceNew Failed",
			    "audio_osx_source::setup");

#else

      Component comp = FindNextComponent(NULL, &desc);
      if(!comp) {
        GR_LOG_FATAL(d_logger, boost::format
		     ("FindNextComponent Failed"));
        throw std::runtime_error("audio_osx_source::setup");
      }
      err = OpenAComponent(comp, &d_input_au);
      check_error_and_throw(err, "OpenAComponent Failed",
                         "audio_osx_source::setup");

#endif

      // must enable the AUHAL for input and disable output
      // before setting the AUHAL's current device

      // Enable input on the AUHAL

      UInt32 enable_io = 1;
      err = AudioUnitSetProperty
	(d_input_au,
	 kAudioOutputUnitProperty_EnableIO,
	 kAudioUnitScope_Input, 1, // input element
	 &enable_io, sizeof(enable_io));
      check_error_and_throw
	(err, "AudioUnitSetProperty Input Enable",
	 "audio_osx_source::setup");

      // Disable output on the AUHAL

      enable_io = 0;
      err = AudioUnitSetProperty
	(d_input_au,
	 kAudioOutputUnitProperty_EnableIO,
	 kAudioUnitScope_Output, 0, // output element
	 &enable_io, sizeof(enable_io));
      check_error_and_throw
	(err, "AudioUnitSetProperty Output Disable",
	 "audio_osx_source::setup");

      // set the selected device ID as the current input device

      err = AudioUnitSetProperty
	(d_input_au, kAudioOutputUnitProperty_CurrentDevice,
	 kAudioUnitScope_Global, 0,
	 &d_input_ad_id, sizeof(d_input_ad_id));
      check_error_and_throw
	(err, "Setting selected input device as current failed",
	 "audio_osx_source::setup");

      // Set up a callback function to retrieve input from the Audio Device

      AURenderCallbackStruct au_callback = {
        reinterpret_cast<AURenderCallback>
	  (&osx_source::au_input_callback),
	reinterpret_cast<void*>(this)
      };
      UInt32 prop_size = (UInt32)sizeof(au_callback);

      err = AudioUnitSetProperty
	(d_input_au,
	 kAudioOutputUnitProperty_SetInputCallback,
	 kAudioUnitScope_Global, 0,
	 &au_callback, prop_size);
      check_error_and_throw
	(err, "Set Input Callback",
	 "audio_osx_source::setup");

      // Get the Stream Format (device side; cannot generally be changed)

      prop_size = (UInt32)sizeof(d_asbd_device);
      memset((void*)(&d_asbd_device), 0, (size_t)prop_size);
      err = AudioUnitGetProperty
	(d_input_au,
	 kAudioUnitProperty_StreamFormat,
	 kAudioUnitScope_Input, 1,
	 &d_asbd_device, &prop_size);
      check_error_and_throw
	(err, "Get Device Input Stream Format (before) failed",
	 "audio_osx_source::setup");

#if _OSX_AU_DEBUG_
      std::cerr << std::endl << "---- Device Stream Format (before) ----"
		<< std::endl << d_asbd_device << std::endl << std::endl;
#endif

      // try to set the device (input) side of the audio device to the
      // sample rate of this source.  This will likely fail, and
      // that's OK; just ignore the error since we can accomplish
      // audio input in other ways.

      prop_size = (UInt32)sizeof(d_asbd_device);
      d_asbd_device.mSampleRate = d_output_sample_rate;
      err = AudioUnitSetProperty
	(d_input_au,
	 kAudioUnitProperty_StreamFormat,
	 kAudioUnitScope_Input, 1,
	 &d_asbd_device, prop_size);
#if _OSX_AU_DEBUG_
      check_error
	(err, "Set Device Input Stream Format failed (expected)");
#endif

      memset((void*)(&d_asbd_device), 0, (size_t)prop_size);
      err = AudioUnitGetProperty
	(d_input_au,
	 kAudioUnitProperty_StreamFormat,
	 kAudioUnitScope_Input, 1,
	 &d_asbd_device, &prop_size);
      check_error_and_throw
	(err, "Get Device Input Stream Format (after) failed",
	 "audio_osx_source::setup");

#if _OSX_AU_DEBUG_
      std::cerr << std::endl << "---- Device Stream Format (after) ----"
		<< std::endl << d_asbd_device << std::endl << std::endl;
#endif

      d_device_sample_rate = d_asbd_device.mSampleRate;

      // Get the Stream Format (client side; might be changeable)

      prop_size = (UInt32)sizeof(d_asbd_client);
      memset((void*)(&d_asbd_client), 0, (size_t)prop_size);
      err = AudioUnitGetProperty
	(d_input_au,
	 kAudioUnitProperty_StreamFormat,
	 kAudioUnitScope_Output, 1,
	 &d_asbd_client, &prop_size);
      check_error_and_throw
	(err, "Get Device Output Stream Format (before) failed",
	 "audio_osx_source::setup");

#if _OSX_AU_DEBUG_
      std::cerr << "---- Client Stream Format (Before) ----"
		<< std::endl << d_asbd_client << std::endl << std::endl;
#endif

      // Set the format of all the AUs to the
      // input/output devices channel count

      d_asbd_client.mFormatID = kAudioFormatLinearPCM;
      d_asbd_client.mFormatFlags = (kAudioFormatFlagIsFloat |
                                  kAudioFormatFlagIsPacked |
                                  kAudioFormatFlagIsNonInterleaved);
      if((d_asbd_client.mFormatID == kAudioFormatLinearPCM) &&
         (d_n_dev_channels == 1)) {
        d_asbd_client.mFormatFlags &= ~kLinearPCMFormatFlagIsNonInterleaved;
      }
      d_asbd_client.mBytesPerFrame = (UInt32)sizeof(float);
      d_asbd_client.mFramesPerPacket = 1;
      d_asbd_client.mBitsPerChannel = d_asbd_client.mBytesPerFrame * 8;
      d_asbd_client.mChannelsPerFrame = d_n_dev_channels;
      d_asbd_client.mBytesPerPacket = d_asbd_client.mBytesPerFrame;

      // according to Apple docs [see, e.g., Apple Technical Note
      // TN2091 "Device input using the HAL Output Audio Unit"], the
      // device input and output sample rate must be the same; do
      // sample rate conversion elsewhere.

      d_asbd_client.mSampleRate = d_asbd_device.mSampleRate;
      err = AudioUnitSetProperty
	(d_input_au,
	 kAudioUnitProperty_StreamFormat,
	 kAudioUnitScope_Output, 1,
	 &d_asbd_client, prop_size);
      check_error_and_throw
	(err, "Set Device Output Stream Format failed",
	 "audio_osx_source::setup");

      // Get the Stream Format (client side), again

      prop_size = (UInt32)sizeof(d_asbd_client);
      memset((void*)(&d_asbd_client), 0, (size_t)prop_size);
      err = AudioUnitGetProperty
	(d_input_au,
	 kAudioUnitProperty_StreamFormat,
	 kAudioUnitScope_Output, 1,
	 &d_asbd_client, &prop_size);
      check_error_and_throw
	(err, "Get Device Output Stream Format (after) failed",
	 "audio_osx_source::setup");

#if _OSX_AU_DEBUG_
      std::cerr << "---- Client Stream Format (After) ----"
		<< std::endl << d_asbd_client << std::endl << std::endl;
#endif

      d_pass_through = (d_asbd_client.mSampleRate == d_output_sample_rate);

      if (d_pass_through) {

        // no need to do conversion if d_asbd_client matches user wants
        d_lead_size_frames = d_trail_size_frames = 0L;

      }
      else {

	// create an ASBD for the user's wants

	memset((void*)(&d_asbd_user), 0, sizeof(d_asbd_user));
	d_asbd_user.mSampleRate = d_output_sample_rate;
	d_asbd_user.mFormatID = kAudioFormatLinearPCM;
	d_asbd_user.mFormatFlags = (kLinearPCMFormatFlagIsFloat |
				    GR_PCM_ENDIANNESS |
				    kLinearPCMFormatFlagIsPacked |
				    kAudioFormatFlagIsNonInterleaved);
	d_asbd_user.mBytesPerPacket = (UInt32)sizeof(float);
	d_asbd_user.mFramesPerPacket = 1;
	d_asbd_user.mBytesPerFrame = d_asbd_user.mBytesPerPacket;
	d_asbd_user.mChannelsPerFrame = d_n_dev_channels;
	d_asbd_user.mBitsPerChannel = d_asbd_user.mBytesPerPacket * 8;

        // Create the audio converter

        err = AudioConverterNew(&d_asbd_client,
				&d_asbd_user,
				&d_audio_converter);
        check_error_and_throw
	  (err, "AudioConverterNew failed",
	   "audio_osx_source::setup");

        // Set the audio converter sample rate quality to "max" ...
        // requires more samples, but should sound nicer

        UInt32 ac_quality = kAudioConverterQuality_Max;
        prop_size = (UInt32)sizeof(ac_quality);
        err = AudioConverterSetProperty
	  (d_audio_converter,
	   kAudioConverterSampleRateConverterQuality,
	   prop_size, &ac_quality);
        check_error_and_throw
	  (err, "Set Sample Rate Converter Quality failed",
	   "audio_osx_source::setup");

        // set the audio converter's prime method to "pre",
        // which uses both leading and trailing frames
        // from the "current input".  All of this is handled
        // internally by the AudioConverter; we just supply
        // the frames for conversion.

        UInt32 ac_prime_method = kConverterPrimeMethod_Pre;
        prop_size = (UInt32)sizeof(ac_prime_method);
        err = AudioConverterSetProperty
	  (d_audio_converter,
	   kAudioConverterPrimeMethod,
	   prop_size, &ac_prime_method);
        check_error_and_throw
	  (err, "Set Prime Method failed",
	   "audio_osx_source::setup");

        // Get the size of the priming I/O buffer space to allow for
        // pre-allocated buffers

        AudioConverterPrimeInfo ac_prime_info = {0, 0};
        prop_size = (UInt32)sizeof(ac_prime_info);
        err = AudioConverterGetProperty
	  (d_audio_converter,
	   kAudioConverterPrimeInfo,
	   &prop_size, &ac_prime_info);
        check_error_and_throw
	  (err, "Get Prime Info failed",
	   "audio_osx_source::setup");

	d_lead_size_frames = ac_prime_info.leadingFrames;
	d_trail_size_frames = ac_prime_info.trailingFrames;
      }

      d_lead_size_bytes = d_lead_size_frames * sizeof(float);
      d_trail_size_bytes = d_trail_size_frames * sizeof(float);

      prop_size = (UInt32)sizeof(d_device_buffer_size_frames);
      err = AudioUnitGetProperty
	(d_input_au,
	 kAudioDevicePropertyBufferFrameSize,
	 kAudioUnitScope_Global, 0,
	 &d_device_buffer_size_frames, &prop_size);
      check_error_and_throw
	(err, "Get Buffer Frame Size failed",
	 "audio_osx_source::setup");

      d_device_buffer_size_bytes = (d_device_buffer_size_frames *
				    sizeof(float));
      d_input_buffer_size_bytes = (d_device_buffer_size_bytes +
				   d_lead_size_bytes);
      d_input_buffer_size_frames = (d_device_buffer_size_frames +
				    d_lead_size_frames);

      // outBufSizeBytes = floor (inBufSizeBytes * rate_out / rate_in)
      // since this is rarely exact, we need another buffer to hold
      // "extra" samples not processed at any given sampling period
      // this buffer must be at least 4 floats in size, but generally
      // follows the rule that
      // extraBufSize =  ceil (rate_in / rate_out)*sizeof(float)

      d_extra_buffer_size_frames =
	((UInt32)ceil(d_device_sample_rate /
		      d_output_sample_rate) *
	 sizeof(float));
      if(d_extra_buffer_size_frames < 4)
        d_extra_buffer_size_frames = 4;
      d_extra_buffer_size_bytes =
	d_extra_buffer_size_frames * sizeof(float);

      d_output_buffer_size_frames =
	(UInt32)ceil(((Float64)d_input_buffer_size_frames) *
		     d_output_sample_rate / d_device_sample_rate);
      d_output_buffer_size_bytes =
	d_output_buffer_size_frames * sizeof(float);
      d_input_buffer_size_frames += d_extra_buffer_size_frames;

      // pre-alloc all CoreAudio buffers

      alloc_audio_buffer_list
	(&d_input_buffer, d_n_dev_channels,
	 d_input_buffer_size_bytes);
      if(!d_pass_through) {
        alloc_audio_buffer_list
	  (&d_output_buffer, d_n_dev_channels,
	   d_output_buffer_size_bytes);
      }
      else {
        d_output_buffer = d_input_buffer;
      }

      // allocate the output circular buffer(s), one per device
      // channel (the user may select fewer channels; those buffers
      // just won't get used).

      d_buffers.resize(d_n_dev_channels);
      for(UInt32 nn = 0; nn < d_n_dev_channels; ++nn) {
	d_buffers[nn] = new circular_buffer<float>
	  (d_buffer_sample_count, false, false);
      }

      // clear the RunLoop (whatever that is); needed, for some
      // reason, before a listener will work.

      {
	CFRunLoopRef the_run_loop = NULL;
	AudioObjectPropertyAddress property = {
	  kAudioHardwarePropertyRunLoop,
	  kAudioObjectPropertyScopeGlobal,
	  kAudioObjectPropertyElementMaster
	};
	prop_size = (UInt32)sizeof(the_run_loop);
	err = AudioObjectSetPropertyData
	  (kAudioObjectSystemObject, &property, 0, NULL,
	   prop_size, &the_run_loop);
	  check_error(err, "Clearing RunLoop failed; "
		      "Audio Input Device Listener might not work.");
      }

      // set up listeners

#ifndef GR_USE_OLD_AUDIO_UNIT

      // 10.4 and newer

      {

	// set up a listener if hardware changes (at all)

	AudioObjectPropertyAddress property = {
	  kAudioHardwarePropertyDevices,
	  kAudioObjectPropertyScopeGlobal,
	  kAudioObjectPropertyElementMaster
	};

	err = AudioObjectAddPropertyListener
	  (kAudioObjectSystemObject, &property,
	   reinterpret_cast<AudioObjectPropertyListenerProc>
	     (&osx_source::hardware_listener),
	   reinterpret_cast<void*>(this));
	check_error(err, "Adding Audio Hardware Listener failed");
      }

      if (d_using_default_device) {

	// set up a listener if default hardware input device changes

	AudioObjectPropertyAddress property = {
	  kAudioHardwarePropertyDefaultInputDevice,
	  kAudioObjectPropertyScopeGlobal,
	  kAudioObjectPropertyElementMaster
	};

	err = AudioObjectAddPropertyListener
	  (kAudioObjectSystemObject, &property,
	   reinterpret_cast<AudioObjectPropertyListenerProc>
	     (&osx_source::default_listener),
	   reinterpret_cast<void*>(this));
	check_error(err, "Adding Default Input Audio Listener failed");

      }

#else

      // 10.5 and older

      err = AudioHardwareAddPropertyListener
	(kAudioHardwarePropertyDevices,
	 reinterpret_cast<AudioHardwarePropertyListenerProc>
	   (&osx_source::hardware_listener),
	 reinterpret_cast<void*>(this));
      check_error(err, "Adding Audio Hardware Listener failed");

      if (d_using_default_device) {

	err = AudioHardwareAddPropertyListener
	  (kAudioHardwarePropertyDefaultInputDevice,
	   reinterpret_cast<AudioHardwarePropertyListenerProc>
	     (&osx_source::default_listener),
	   reinterpret_cast<void*>(this));
	check_error(err, "Adding Default Input Audio Listener failed");

      }

#endif

      // initialize the AU for input, so that it is ready to be used

      err = AudioUnitInitialize(d_input_au);
      check_error_and_throw
	(err, "AudioUnitInitialize",
	 "audio_osx_source::check_channels");

#if _OSX_AU_DEBUG_
      std::cerr << std::endl << "audio_osx_source Parameters:"
		<< std::endl << "  Device Sample Rate is "
		<< d_device_sample_rate << std::endl
		<< "  Client Sample Rate is "
		<< (d_pass_through ? d_output_sample_rate :
		    d_device_sample_rate) << std::endl
		<< "  User Sample Rate is "
		<< d_output_sample_rate << std::endl
		<< "  Do Passthrough is "
		<< (d_pass_through ? "true" : "false") << std::endl
		<< "  Max Sample Count is "
		<< d_buffer_sample_count << std::endl
		<< "  # Device Channels is "
		<< d_n_dev_channels << std::endl
		<< "  Device Buffer Size in Frames = "
		<< d_device_buffer_size_frames << std::endl
		<< "  Lead Size in Frames = "
		<< d_lead_size_frames << std::endl
		<< "  Trail Size in Frames = "
		<< d_trail_size_frames << std::endl
		<< "  Input Buffer Size in Frames = "
		<< d_input_buffer_size_frames << std::endl
		<< "  Output Buffer Size in Frames = "
		<< d_output_buffer_size_frames << std::endl
		<< std::endl;
#endif
    }

    void
    osx_source::alloc_audio_buffer_list
    (AudioBufferList** t_abl,
     UInt32 n_channels,
     UInt32 input_buffer_size_bytes)
    {
      free_audio_buffer_list(t_abl);
      UInt32 prop_size = (offsetof(AudioBufferList, mBuffers[0]) +
			  (sizeof(AudioBuffer) * n_channels));
      *t_abl = (AudioBufferList*)calloc(1, prop_size);
      (*t_abl)->mNumberBuffers = n_channels;

      int counter = n_channels;

#if _OSX_AU_DEBUG_
      std::cerr << "alloc_audio_buffer_list: (#chan, #bytes) == ("
		<< n_channels << ", " << input_buffer_size_bytes
		<< ")" << std::endl;
#endif

      while(--counter >= 0) {
	AudioBuffer* t_ab = &((*t_abl)->mBuffers[counter]);
	t_ab->mNumberChannels = 1;
	t_ab->mDataByteSize = input_buffer_size_bytes;
	t_ab->mData = calloc(1, input_buffer_size_bytes);
      }
    }

    void
    osx_source::free_audio_buffer_list(AudioBufferList** t_abl)
    {
      // free pre-allocated audio buffer, if it exists
      if(*t_abl) {
        int counter = (*t_abl)->mNumberBuffers;
        while(--counter >= 0) {
	  AudioBuffer* t_ab = &((*t_abl)->mBuffers[counter]);
          free(t_ab->mData);
	  t_ab->mData = 0;
	}
        free(*t_abl);
        (*t_abl) = 0;
      }
    }

    bool
    osx_source::is_running()
    {
      UInt32 au_running = 0;
      if (d_input_au) {

	UInt32 prop_size = (UInt32)sizeof(au_running);
	OSStatus err = AudioUnitGetProperty
	  (d_input_au,
	   kAudioOutputUnitProperty_IsRunning,
	   kAudioUnitScope_Global, 0,
	   &au_running, &prop_size);
	check_error_and_throw
	  (err, "AudioUnitGetProperty IsRunning",
	   "audio_osx_source::is_running");
      }

      return(au_running != 0);
    }

    bool
    osx_source::start()
    {
#if _OSX_AU_DEBUG_
      std::cerr << ((void*)(pthread_self()))
		<< " : audio_osx_source::start: Starting." << std::endl;
#endif

      if((!is_running ()) && d_input_au) {

#if _OSX_AU_DEBUG_
	std::cerr << ((void*)(pthread_self()))
		  << " : audio_osx_source::start: Starting Audio Unit."
		  << std::endl;
#endif

	// reset buffers before starting

	for(UInt32 nn = 0; nn < d_buffers.size(); ++nn) {
	  d_buffers[nn]->reset();
	}

	// start the audio unit

        OSStatus err = AudioOutputUnitStart(d_input_au);
        check_error_and_throw(err, "AudioOutputUnitStart",
                           "audio_osx_source::start");

	// clear reset (will sometimes be necessary, and it has to
	// happen after AudioOutputUnitStart)

	d_do_reset = false;

      }

#if _OSX_AU_DEBUG_
      std::cerr << ((void*)(pthread_self()))
		<< " : audio_osx_source::start: Returning." << std::endl;
#endif

      return (true);
    }

    bool
    osx_source::stop()
    {
#if _OSX_AU_DEBUG_
      std::cerr << ((void*)(pthread_self()))
		<< " : audio_osx_source::stop: Starting." << std::endl;
#endif
      if(is_running ()) {

#if _OSX_AU_DEBUG_
	std::cerr << ((void*)(pthread_self()))
		  << " : audio_osx_source::stop: stopping audio unit."
		  << std::endl;
#endif

	// stop the audio unit

        OSStatus err = AudioOutputUnitStop(d_input_au);
        check_error_and_throw(err, "AudioOutputUnitStart",
                           "audio_osx_source::stop");

	// abort all buffers

        for(UInt32 nn = 0; nn < d_n_user_channels; ++nn) {
          d_buffers[nn]->abort ();
        }
      }

#if _OSX_AU_DEBUG_
      std::cerr << ((void*)(pthread_self()))
		<< " : audio_osx_source::stop: Returning." << std::endl;
#endif

      return (true);
    }

    void
    osx_source::teardown()
    {
#if _OSX_AU_DEBUG_
      std::cerr << ((void*)(pthread_self()))
		<< " : audio_osx_source::teardown: Starting." << std::endl;
#endif

      OSStatus err = noErr;

      // stop the AudioUnit

      stop();

      // remove the listeners

#ifndef GR_USE_OLD_AUDIO_UNIT

      // 10.4 and newer
      {
	AudioObjectPropertyAddress property = {
	  kAudioHardwarePropertyDevices,
	  kAudioObjectPropertyScopeGlobal,
	  kAudioObjectPropertyElementMaster
	};

	err = AudioObjectRemovePropertyListener
	  (kAudioObjectSystemObject, &property,
	   reinterpret_cast<AudioObjectPropertyListenerProc>
	     (&osx_source::hardware_listener),
	   reinterpret_cast<void*>(this));
#if _OSX_AU_DEBUG_
	check_error(err, "teardown: AudioObjectRemovePropertyListener "
		    "hardware failed");
#endif

      }

      if (d_using_default_device) {

	AudioObjectPropertyAddress property = {
	  kAudioHardwarePropertyDefaultInputDevice,
	  kAudioObjectPropertyScopeGlobal,
	  kAudioObjectPropertyElementMaster
	};

	err = AudioObjectRemovePropertyListener
	  (kAudioObjectSystemObject, &property,
	   reinterpret_cast<AudioObjectPropertyListenerProc>
	     (&osx_source::default_listener),
	   reinterpret_cast<void*>(this));
#if _OSX_AU_DEBUG_
	check_error(err, "AudioObjectRemovePropertyListener default");
#endif

	d_using_default_device = false;

      }
#else

      // 10.5 and older

      err = AudioHardwareRemovePropertyListener
	(kAudioHardwarePropertyDevices,
	 reinterpret_cast<AudioHardwarePropertyListenerProc>
	   (&osx_source::hardware_listener));
#if _OSX_AU_DEBUG_
      check_error(err, "AudioObjectRemovePropertyListener hardware");
#endif

      if (d_using_default_device) {
	err = AudioHardwareRemovePropertyListener
	  (kAudioHardwarePropertyDefaultInputDevice,
	   reinterpret_cast<AudioHardwarePropertyListenerProc>
	     (&osx_source::default_listener));
#if _OSX_AU_DEBUG_
	check_error(err, "AudioObjectRemovePropertyListener default");
#endif
	d_using_default_device = false;

      }

#endif // GR_USE_OLD_AUDIO_UNIT

      // free pre-allocated audio buffers
      free_audio_buffer_list(&d_input_buffer);

      if(!d_pass_through) {
        err = AudioConverterDispose(d_audio_converter);
#if _OSX_AU_DEBUG_
        check_error(err, "~audio_osx_source: AudioConverterDispose");
#endif
        free_audio_buffer_list(&d_output_buffer);
      }

      // remove the audio unit
      err = AudioUnitUninitialize(d_input_au);
#if _OSX_AU_DEBUG_
      check_error(err, "~audio_osx_source: AudioUnitUninitialize");
#endif

#ifndef GR_USE_OLD_AUDIO_UNIT
      err = AudioComponentInstanceDispose(d_input_au);
#if _OSX_AU_DEBUG_
      check_error(err, "~audio_osx_source: AudioComponentInstanceDispose");
#endif
#else
      err = CloseComponent(d_input_au);
#if _OSX_AU_DEBUG_
      check_error(err, "~audio_osx_source: CloseComponent");
#endif
#endif

      // empty and delete the queues

      for(UInt32 nn = 0; nn < d_buffers.size(); ++nn) {
        delete d_buffers[nn];
        d_buffers[nn] = 0;
      }
      d_buffers.resize(0);

      // clear important variables; not # user channels

      d_queue_sample_count = 0;
      d_device_sample_rate = 0;
      d_n_dev_channels = 0;
      d_input_ad_id = 0;
      d_input_au = 0;
      d_input_buffer = d_output_buffer = 0;
      d_audio_converter = 0;
      d_using_default_device = false;

#if _OSX_AU_DEBUG_
      std::cerr << ((void*)(pthread_self()))
		<< " : audio_osx_source::teardown: Returning." << std::endl;
#endif
    }

    bool
    osx_source::check_topology(int ninputs, int noutputs)
    {
      // check # inputs to make sure it's valid
      if(ninputs != 0) {

        GR_LOG_FATAL(d_logger, boost::format
		     ("check_topology(): number of input "
		      "streams provided (%d) should be 0.")
		     % ninputs);
        throw std::runtime_error
	  ("audio_osx_source::check_topology");

      }

      // check # outputs to make sure it's valid
      if((noutputs < 1) | (noutputs > (int) d_n_dev_channels)) {

        GR_LOG_FATAL(d_logger, boost::format
		     ("check_topology(): number of output "
		      "streams provided (%d) should be in [1,%d] "
		      "for the selected input audio device.")
		     % noutputs % d_n_dev_channels);
        throw std::runtime_error
	  ("audio_osx_source::check_topology");
      }

      // save the actual number of output (user) channels

      d_n_user_channels = noutputs;

#if _OSX_AU_DEBUG_
      std::cerr << "chk_topo: Actual # user output channels = "
                << noutputs << std::endl;
#endif

      return(true);
    }

    int
    osx_source::work(int noutput_items,
                     gr_vector_const_void_star &input_items,
                     gr_vector_void_star &output_items)
    {
#if _OSX_AU_DEBUG_RENDER_
      std::cerr << ((void*)(pthread_self()))
		<< " : audio_osx_source::work: Starting." << std::endl;
#endif
      if (d_do_reset) {
	if (d_hardware_changed) {

	  // see if the current AudioDeviceID is still available

	  std::vector < AudioDeviceID > all_ad_ids;
	  osx::find_audio_devices
	    (d_desired_name, true,
	     &all_ad_ids, NULL);
	  bool found = false;
	  for (UInt32 nn = 0; (nn < all_ad_ids.size()) && (!found);
	       ++nn) {
	    found = (all_ad_ids[nn] == d_input_ad_id);
	  }
	  if (!found) {

	    GR_LOG_FATAL(d_logger, boost::format
			 ("The selected input audio device ('%s') "
			  "is no longer available.\n")
			 % d_selected_name);
	    return(gr::block::WORK_DONE);

	  }

	  d_do_reset = d_hardware_changed = false;

	} else {

#if _OSX_AU_DEBUG_RENDER_
	  std::cerr << "audio_osx_source::work: doing reset."
		    << std::endl;
#endif

	  GR_LOG_WARN(d_logger, boost::format
		      ("\n\nThe default input audio device has "
		       "changed; resetting audio.\nThere may "
		       "be a sound glitch while resetting.\n"));

	  // for any changes, just tear down the current
	  // configuration, then set it up again using the user's
	  // parameters to try to make selections.

	  teardown();

	  gr::thread::scoped_lock l(d_internal);

#if _OSX_AU_DEBUG_RENDER_
	  std::cerr << "audio_osx_source::work: mutex locked."
		    << std::endl;
#endif

	  setup();
	  start();

#if _OSX_AU_DEBUG_RENDER_
	  std::cerr << "audio_osx_source::work: returning after reset."
		    << std::endl;
#endif
	  return(0);
	}
      }

      gr::thread::scoped_lock l(d_internal);

#if _OSX_AU_DEBUG_RENDER_
      std::cerr << "audio_osx_source::work: mutex locked." << std::endl;
#endif

#if _OSX_AU_DEBUG_RENDER_
      std::cerr << "work1: SC = " << d_queue_sample_count
                << ", #OI = " << noutput_items
                << ", #Chan = " << output_items.size() << std::endl;
#endif

      // set the actual # of output items to the 'desired' amount then
      // verify that data is available; if not enough data is
      // available, either wait until it is (is "ok_to_block" is
      // true), return (0) is no data is available and "ok_to_block"
      // is false, or process the actual amount of available data.

      UInt32 actual_noutput_items = noutput_items;

      if(d_queue_sample_count < actual_noutput_items) {
        if(d_queue_sample_count == 0) {
          // no data; ok_to_block decides what to do
          if(d_ok_to_block == true) {
	    // block until there is data to return, or on reset
            while(d_queue_sample_count == 0) {
              // release control so-as to allow data to be retrieved;
              // block until there is data to return
#if _OSX_AU_DEBUG_RENDER_
	      std::cerr << "audio_osx_source::work: waiting."
			<< std::endl;
#endif
	      d_waiting_for_data = true;
              d_cond_data.wait(l);
	      d_waiting_for_data = false;
#if _OSX_AU_DEBUG_RENDER_
	      std::cerr << "audio_osx_source::work: done waiting."
			<< std::endl;
#endif
              // the condition's 'notify' was called; acquire control to
              // keep thread safe

	      // if doing a reset, just return here; reset will pick
	      // up the next time this method is called.
	      if (d_do_reset) {
#if _OSX_AU_DEBUG_RENDER_
		std::cerr << "audio_osx_source::work: "
		  "returning for reset." << std::endl;
#endif
		return(0);
	      }
            }
          }
          else {
            // no data & not blocking; return nothing
#if _OSX_AU_DEBUG_RENDER_
	    std::cerr << "audio_osx_source::work: no data "
	      "& not blocking; returning 0." << std::endl;
#endif
            return (0);
          }
        }

        // use the actual amount of available data
        actual_noutput_items = d_queue_sample_count;
      }

#if _OSX_AU_DEBUG_RENDER_
      std::cerr << "audio_osx_source::work: copying "
		<< actual_noutput_items << " items per channel"
		<< std::endl;
#endif

      // number of channels
      int l_counter = (int)output_items.size();

      // copy the items from the circular buffer(s) to 'work's output
      // buffers; verify that the number copied out is as expected.

      while(--l_counter >= 0) {

        size_t t_n_output_items = actual_noutput_items;
        d_buffers[l_counter]->dequeue
	  ((float*)output_items[l_counter],
	   &t_n_output_items);

        if(t_n_output_items != actual_noutput_items) {

          GR_LOG_FATAL(d_logger, boost::format
		       ("work(): ERROR: number of available "
			"items changing unexpectedly; expecting %d"
			", got %d.")
		       % actual_noutput_items % t_n_output_items);
          throw std::runtime_error("audio_osx_source::work()");

        }
      }

      // subtract the actual number of items removed from the buffer(s)
      // from the local accounting of the number of available samples

      d_queue_sample_count -= actual_noutput_items;

#if _OSX_AU_DEBUG_RENDER_
      std::cerr << "work2: SC = " << d_queue_sample_count
                << ", act#OI = " << actual_noutput_items << std::endl
                << "Returning." << std::endl;
#endif
#if _OSX_AU_DEBUG_RENDER_
	std::cerr << "audio_osx_source::work: returning." << std::endl;
#endif

      return (actual_noutput_items);
    }

    OSStatus
    osx_source::converter_callback
    (AudioConverterRef in_audio_converter,
     UInt32* io_number_data_packets,
     AudioBufferList* io_data,
     AudioStreamPacketDescription** out_aspd,
     void* in_user_data)
    {
      // This callback is for us to provide the buffers to CoreAudio
      // for conversion.  We need to set the buffers in the provided
      // buffer list (io_data) to the buffers we know about and use to
      // do data input (d_input_buffers).

      osx_source* This = static_cast<osx_source*>(in_user_data);
      AudioBufferList* l_input_abl = This->d_input_buffer;
      UInt32 total_input_buffer_size_bytes =
	((*io_number_data_packets) * sizeof(float));
      int counter = This->d_n_dev_channels;
      io_data->mNumberBuffers = This->d_n_dev_channels;
      This->d_n_actual_input_frames = (*io_number_data_packets);

#if _OSX_AU_DEBUG_RENDER_
      std::cerr << "cc1: io#DP = " << (*io_number_data_packets)
                << ", TIBSB = " << total_input_buffer_size_bytes
                << ", #C = " << counter << std::endl;
#endif

      while(--counter >= 0) {
        AudioBuffer* t_ab = &(io_data->mBuffers[counter]);
        t_ab->mNumberChannels = 1;
        t_ab->mData = l_input_abl->mBuffers[counter].mData;
        t_ab->mDataByteSize = total_input_buffer_size_bytes;
      }

#if _OSX_AU_DEBUG_RENDER_
      std::cerr << "cc2: Returning." << std::endl;
#endif

      return (noErr);
    }

    OSStatus
    osx_source::au_input_callback
    (void *in_ref_con,
     AudioUnitRenderActionFlags *io_action_flags,
     const AudioTimeStamp *in_time_stamp,
     UInt32 in_bus_number,
     UInt32 in_number_frames,
     AudioBufferList *io_data)
    {
#if _OSX_AU_DEBUG_RENDER_
      std::cerr << ((void*)(pthread_self()))
		<< " : audio_osx_source::au_input_callback: Starting."
		<< std::endl;
#endif

      osx_source* This = reinterpret_cast
	<osx_source*>(in_ref_con);
      gr::thread::scoped_lock l(This->d_internal);
      gr::logger_ptr d_logger = This->d_logger;

#if _OSX_AU_DEBUG_RENDER_
      std::cerr << "audio_osx_source::au_input_callback: mutex locked."
		<< std::endl;
#endif

      OSStatus err = noErr;

#if _OSX_AU_DEBUG_RENDER_
      std::cerr << "cb0: in#Fr = " << in_number_frames
                << ", inBus# = " << in_bus_number
		<< ", idD = " << ((void*)io_data)
		<< ", d_ib = " << ((void*)(This->d_input_buffer))
		<< ", d_ib#c = " << This->d_input_buffer->mNumberBuffers
                << ", SC = " << This->d_queue_sample_count << std::endl;
#endif

      if (This->d_do_reset) {

	// clear audio data; do not render since it will generate an error

	AudioBufferList* t_abl = This->d_input_buffer;
	for (UInt32 nn = 0; nn < t_abl->mNumberBuffers; ++nn) {
	  AudioBuffer* t_ab = &(t_abl->mBuffers[nn]);
	  memset(t_ab->mData, 0, (size_t)((t_ab->mDataByteSize) *
					  (t_ab->mNumberChannels)));
	}
      } else {

	// Get the new audio data from the input device

	err = AudioUnitRender
	  (This->d_input_au, io_action_flags,
	   in_time_stamp, 1, //inBusNumber,
	   in_number_frames, This->d_input_buffer);
	check_error_and_throw
	  (err, "AudioUnitRender",
	   "audio_osx_source::au_input_callback");

      }

      UInt32 available_input_frames =
	This->d_n_available_input_frames = in_number_frames;

      // get the number of actual output frames,
      // either via converting the buffer or not

      UInt32 actual_output_frames = available_input_frames;

      if(!This->d_pass_through) {
        UInt32 available_input_bytes =
	  available_input_frames * sizeof(float);
        UInt32 available_output_bytes = available_input_bytes;
        UInt32 prop_size = sizeof(available_output_bytes);
        err = AudioConverterGetProperty
	  (This->d_audio_converter,
	   kAudioConverterPropertyCalculateOutputBufferSize,
	   &prop_size,
	   &available_output_bytes);
        check_error_and_throw
	  (err, "Get Output Buffer Size failed",
	   "audio_osx_source::au_input_callback");

        UInt32 available_output_frames =
	  available_output_bytes / sizeof(float);

#if 0
        // when decimating too much, the output sounds warbly due to
        // fluctuating # of output frames
        // This should not be a surprise, but there's probably some
        // clever programming that could lessed the effect ...
        // like finding the "ideal" # of output frames, and keeping
        // that number constant no matter the # of input frames

        UInt32 l_input_bytes = available_output_bytes;
        prop_size = sizeof(available_output_bytes);
        err = AudioConverterGetProperty
	  (This->d_audio_converter,
	   kAudioConverterPropertyCalculateInputBufferSize,
	   &prop_size, &l_input_bytes);
        check_error_and_throw
	  (err, "Get Input Buffer Size failed",
	   "audio_osx_source::au_input_callback");

        if(l_input_bytes < available_input_bytes) {
          // OK to zero pad the input a little
          ++available_output_frames;
          available_output_bytes = available_output_frames * sizeof(float);
        }

#endif

#if _OSX_AU_DEBUG_RENDER_
        std::cerr << "cb1:  avail: #IF = " << available_input_frames
                  << ", #OF = " << available_output_frames << std::endl;
#endif
        actual_output_frames = available_output_frames;

        // convert the data to the correct rate; on input,
        // actual_output_frames is the number of available output frames

        err = AudioConverterFillComplexBuffer
	  (This->d_audio_converter,
	   reinterpret_cast<AudioConverterComplexInputDataProc>
	     (&(This->converter_callback)),
	   in_ref_con,
	   &actual_output_frames,
	   This->d_output_buffer,
	   NULL);
        check_error_and_throw
	  (err, "AudioConverterFillComplexBuffer failed",
	   "audio_osx_source::au_input_callback");

        // on output, actual_output_frames is the actual number of
        // output frames

#if _OSX_AU_DEBUG_RENDER_
        std::cerr << "cb2: actual: #IF = " << This->d_n_actual_input_frames
                  << ", #OF = " << actual_output_frames << std::endl;
        if(This->d_n_actual_input_frames != available_input_frames)
          std::cerr << "cb2.1: avail#IF = " << available_input_frames
                    << ", actual#IF = " << This->d_n_actual_input_frames << std::endl;
#endif
      }

      // add the output frames to the buffers' queue, checking for overflow

      int counter = This->d_n_user_channels;
      int res = 0;

      while(--counter >= 0) {
        float* in_buffer = (float*)
	  This->d_output_buffer->mBuffers[counter].mData;

#if _OSX_AU_DEBUG_RENDER_
        std::cerr << "cb3: enqueuing audio data." << std::endl;
#endif

        int l_res = This->d_buffers[counter]->enqueue
	  (in_buffer, actual_output_frames);
        if(l_res == -1)
          res = -1;
      }

      if(res == -1) {
        // data coming in too fast
        // drop oldest buffer
        fputs("aO", stderr);
        fflush(stderr);
        // set the local number of samples available to the max
        This->d_queue_sample_count =
	  This->d_buffers[0]->buffer_length_items();
      }
      else {
        // keep up the local sample count
        This->d_queue_sample_count += actual_output_frames;
      }

#if _OSX_AU_DEBUG_RENDER_
      std::cerr << "cb4: #OI = " << actual_output_frames
                << ", #Cnt = " << This->d_queue_sample_count
                << ", mSC = " << This->d_buffer_sample_count << std::endl;
#endif

      // signal that data is available, if appropriate

      if (This->d_waiting_for_data) {
	This->d_cond_data.notify_one();
      }

#if _OSX_AU_DEBUG_RENDER_
      std::cerr << "cb5: returning." << std::endl;
#endif

      return(err);
    }

#ifndef GR_USE_OLD_AUDIO_UNIT

    OSStatus
    osx_source::hardware_listener
    (AudioObjectID in_object_id,
     UInt32 in_num_addresses,
     const AudioObjectPropertyAddress in_addresses[],
     void* in_client_data)

#else

    OSStatus
    osx_source::hardware_listener
    (AudioHardwarePropertyID in_property_id,
     void* in_client_data)

#endif
    {
      osx_source* This = reinterpret_cast
	<osx_source*>(in_client_data);
      This->reset(true);
      return(noErr);
    }

#ifndef GR_USE_OLD_AUDIO_UNIT

    OSStatus
    osx_source::default_listener
    (AudioObjectID in_object_id,
     UInt32 in_num_addresses,
     const AudioObjectPropertyAddress in_addresses[],
     void* in_client_data)

#else

    OSStatus
    osx_source::default_listener
    (AudioHardwarePropertyID in_property_id,
     void* in_client_data)

#endif
    {
      osx_source* This = reinterpret_cast
	<osx_source*>(in_client_data);
      This->reset(false);
      return(noErr);
    }

  } /* namespace audio */
} /* namespace gr */

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
#include "osx_sink.h"

#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <stdexcept>

namespace gr {
  namespace audio {

    sink::sptr
    osx_sink_fcn(int sampling_rate,
                 const std::string& device_name,
                 bool ok_to_block)
    {
      return sink::sptr
        (new osx_sink(sampling_rate, device_name, ok_to_block));
    }

    static std::string
    default_device_name()
    {
      return prefs::singleton()->get_string
        ("audio_osx", "default_output_device", "built-in");
    }

    osx_sink::osx_sink(int sample_rate,
                       const std::string& device_name,
                       bool ok_to_block)
      : sync_block("audio_osx_sink",
                      io_signature::make(0, 0, 0),
                      io_signature::make(0, 0, 0)),
        d_input_sample_rate(0.0), d_n_user_channels(0),
        d_n_dev_channels(0), d_queue_sample_count(0),
        d_buffer_size_samples(0), d_ok_to_block(ok_to_block),
        d_do_reset(false), d_hardware_changed(false),
        d_using_default_device(false), d_waiting_for_data(false),
        d_desired_name(device_name.empty() ? default_device_name()
                       : device_name),
        d_output_au(0), d_output_ad_id(0)
    {
      if(sample_rate <= 0) {
        GR_LOG_ERROR(d_logger, boost::format
                     ("Invalid Sample Rate: %d")
                     % sample_rate);
        throw std::invalid_argument("audio_osx_sink");
      }
      else {
        d_input_sample_rate = (Float64)sample_rate;
      }

      // set up for audio output using the stored desired parameters

      setup();
    }

    void osx_sink::setup()
    {
      OSStatus err = noErr;

      // set the default output audio device id to "unknown"

      d_output_ad_id = kAudioDeviceUnknown;

      // try to find the output audio device, if specified

      std::vector < AudioDeviceID > all_ad_ids;
      std::vector < std::string > all_names;

      osx::find_audio_devices
        (d_desired_name, false,
         &all_ad_ids, &all_names);

      // check number of device(s) returned

      if (d_desired_name.length() != 0) {
        if (all_ad_ids.size() == 1) {

          // exactly 1 match was found; see if it was partial

          if (all_names[0].compare(d_desired_name) != 0) {

            // yes: log the full device name
            GR_LOG_INFO(d_logger, boost::format
                        ("Using output audio device '%s'.")
                        % all_names[0]);

          }

          // store info on this device

          d_output_ad_id = all_ad_ids[0];
          d_selected_name = all_names[0];

        } else {

          // either 0 or more than 1 device was found; get all output
          // device names, print those, and error out.

          osx::find_audio_devices("", false, NULL, &all_names);

          std::string err_str("\n\nA unique output audio device name "
                              "matching the string '");
          err_str += d_desired_name;
          err_str += "' was not found.\n\n";
          err_str += "The current known output audio device name";
          err_str += ((all_names.size() > 1) ? "s are" : " is");
          err_str += ":\n";
          for (UInt32 nn = 0; nn < all_names.size(); ++nn) {
            err_str += "    " + all_names[nn] + "\n";
          }
          GR_LOG_ERROR(d_logger, boost::format(err_str));
          throw std::runtime_error("audio_osx_sink::setup");

        }
      }

      // if no output audio device id was found, use the default
      // output audio device as set in System Preferences.

      if (d_output_ad_id == kAudioDeviceUnknown) {

        UInt32 size = sizeof(AudioDeviceID);
        AudioObjectPropertyAddress ao_address = {
          kAudioHardwarePropertyDefaultOutputDevice,
          kAudioObjectPropertyScopeGlobal,
          kAudioObjectPropertyElementMaster
        };

        err = AudioObjectGetPropertyData
          (kAudioObjectSystemObject, &ao_address,
           0, NULL, &size, &d_output_ad_id);
        check_error_and_throw
          (err, "Getting the default output audio device ID failed",
           "audio_osx_sink::setup");

        {
          // retrieve the device name; max name length is 64 characters.

          UInt32 prop_size = 65;
          char c_name_buf[prop_size];
          bzero((void*)c_name_buf, prop_size);
          --prop_size;

          AudioObjectPropertyAddress ao_address = {
            kAudioDevicePropertyDeviceName,
            kAudioDevicePropertyScopeOutput, 0
          };

          if ((err = AudioObjectGetPropertyData
               (d_output_ad_id, &ao_address, 0, NULL,
                &prop_size, (void*)c_name_buf)) != noErr) {

            check_error(err, "Unable to retrieve output audio device name");

          } else {

            GR_LOG_INFO(d_logger, boost::format
                         ("\n\nUsing output audio device '%s'.\n  ... "
                          "which is the current default output audio"
                          " device.\n  Changing the default output"
                          " audio device in the System Preferences"
                          " will \n  result in changing it here, too "
                          "(with an internal reconfiguration).\n") %
                        std::string(c_name_buf));

          }

          d_selected_name = c_name_buf;

        }

        d_using_default_device = true;

      }

      // retrieve the total number of channels for the selected audio
      // output device

      osx::get_num_channels_for_audio_device_id
        (d_output_ad_id, NULL, &d_n_dev_channels);

      // set the block input signature, if not already set
      // (d_n_user_channels is set in check_topology, which is called
      // before the flow-graph is running)

      if (d_n_user_channels == 0) {
        set_input_signature(io_signature::make
                             (1, d_n_dev_channels, sizeof(float)));
      }

      // set the interim buffer size; to work with the GR scheduler,
      // must be at least 16kB.  Pick 50 kS since that's plenty yet
      // not very much.

      d_buffer_size_samples = (d_input_sample_rate < 50000.0 ?
                               50000 : (UInt32)d_input_sample_rate);

#if _OSX_AU_DEBUG_
      std::cerr << ((void*)(pthread_self()))
                << " : audio_osx_sink: max # samples = "
                << d_buffer_size_samples << std::endl;
#endif

      // create the default AudioUnit for output:

      // Open the default output unit

#ifndef GR_USE_OLD_AUDIO_UNIT
      AudioComponentDescription desc;
#else
      ComponentDescription desc;
#endif

      desc.componentType = kAudioUnitType_Output;
      desc.componentSubType = kAudioUnitSubType_DefaultOutput;
      desc.componentManufacturer = kAudioUnitManufacturer_Apple;
      desc.componentFlags = 0;
      desc.componentFlagsMask = 0;

#ifndef GR_USE_OLD_AUDIO_UNIT

      AudioComponent comp = AudioComponentFindNext(NULL, &desc);
      if(!comp) {
        GR_LOG_FATAL(d_logger, boost::format
                     ("AudioComponentFindNext Failed"));
        throw std::runtime_error("audio_osx_sink::setup");
      }
      err = AudioComponentInstanceNew(comp, &d_output_au);
      check_error_and_throw(err, "AudioComponentInstanceNew Failed",
                         "audio_osx_sink::setup");

#else

      Component comp = FindNextComponent(NULL, &desc);
      if(comp == NULL) {
        GR_LOG_FATAL(d_logger, boost::format
                     ("FindNextComponent Failed"));
        throw std::runtime_error("audio_osx_sink::setup");
      }
      err = OpenAComponent(comp, &d_output_au);
      check_error_and_throw(err, "OpenAComponent Failed",
                            "audio_osx_sink::setup");

#endif

      // set the selected device ID as the current output device

      err = AudioUnitSetProperty
        (d_output_au, kAudioOutputUnitProperty_CurrentDevice,
         kAudioUnitScope_Global, 0,
         &d_output_ad_id, sizeof(d_output_ad_id));
      check_error_and_throw
        (err, "Setting selected output device as current failed",
         "audio_osx_sink::setup");

      // Set up a callback function to generate output to the output unit

      AURenderCallbackStruct au_callback = {
        reinterpret_cast<AURenderCallback>
          (&osx_sink::au_output_callback),
        reinterpret_cast<void*>(this)
      };
      UInt32 prop_size = (UInt32)sizeof(au_callback);

      err = AudioUnitSetProperty
        (d_output_au,
         kAudioUnitProperty_SetRenderCallback,
         kAudioUnitScope_Input, 0,
         &au_callback, prop_size);
      check_error_and_throw
        (err, "Set Render Callback",
         "audio_osx_sink::setup");

      // create the stream format for the output unit, so that it
      // handles any format conversions.  Set number of channels in
      // ::start, once the actual number of channels is known.

      memset((void*)(&d_stream_format), 0, sizeof(d_stream_format));
      d_stream_format.mSampleRate = d_input_sample_rate;
      d_stream_format.mFormatID = kAudioFormatLinearPCM;
      d_stream_format.mFormatFlags = (kLinearPCMFormatFlagIsFloat |
                                   GR_PCM_ENDIANNESS |
                                   kLinearPCMFormatFlagIsPacked |
                                   kAudioFormatFlagIsNonInterleaved);
      d_stream_format.mBytesPerPacket = sizeof(float);
      d_stream_format.mFramesPerPacket = 1;
      d_stream_format.mBytesPerFrame = sizeof(float);
      d_stream_format.mBitsPerChannel = 8*sizeof(float);

      // set the render quality to maximum

      UInt32 render_quality = kRenderQuality_Max;
      prop_size = (UInt32)sizeof(render_quality);
      err = AudioUnitSetProperty
        (d_output_au,
         kAudioUnitProperty_RenderQuality,
         kAudioUnitScope_Global, 0,
         &render_quality, prop_size);
      check_error(err, "Setting render quality failed");

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
                    "Audio Output Device Listener might not work.");
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
             (&osx_sink::hardware_listener),
           reinterpret_cast<void*>(this));
        check_error(err, "Adding Audio Hardware Listener failed");
      }

      if (d_using_default_device) {

        // set up a listener for the default output device so that if
        // the device changes, this routine will be called and we can
        // internally handle this change (if/as necessary)

        {
          AudioObjectPropertyAddress property = {
            kAudioHardwarePropertyDefaultOutputDevice,
            kAudioObjectPropertyScopeGlobal,
            kAudioObjectPropertyElementMaster
          };
          err = AudioObjectAddPropertyListener
            (kAudioObjectSystemObject, &property,
             reinterpret_cast<AudioObjectPropertyListenerProc>
               (&osx_sink::hardware_listener),
             reinterpret_cast<void*>(this));
          check_error(err, "Adding Default Output Audio Listener failed");
        }
      }

#else

      // 10.5 and older

      err = AudioHardwareAddPropertyListener
        (kAudioHardwarePropertyDevices,
         reinterpret_cast<AudioHardwarePropertyListenerProc>
           (&osx_sink::hardware_listener),
         reinterpret_cast<void*>(this));
      check_error(err, "Adding Audio Hardware Listener failed");

      if (d_using_default_device) {

        err = AudioHardwareAddPropertyListener
          (kAudioHardwarePropertyDefaultOutputDevice,
           reinterpret_cast<AudioHardwarePropertyListenerProc>
             (&osx_sink::default_listener),
           reinterpret_cast<void*>(this));
        check_error(err, "Adding Default Output Audio Listener failed");

      }

#endif

      // initialize the AU for output, so that it is ready to be used

      err = AudioUnitInitialize(d_output_au);
      check_error_and_throw
        (err, "AudioUnit Initialize Failed",
         "audio_osx_sink::setup");


#if _OSX_AU_DEBUG_
      std::cerr << ((void*)(pthread_self()))
                << " : audio_osx_sink Parameters:" << std::endl
                << "  Sample Rate is " << d_input_sample_rate << std::endl
                << "  Max # samples to store per channel is "
                << d_buffer_size_samples << std::endl;
#endif
    }

    void osx_sink::teardown()
    {
#if _OSX_AU_DEBUG_
      std::cerr << ((void*)(pthread_self()))
                << " : audio_osx_sink::teardown: starting"
                << std::endl;
#endif

      OSStatus err = noErr;

      // stop the AudioUnit

      stop();

      if (d_using_default_device) {
        // remove the listener

        OSStatus err = noErr;
        AudioObjectPropertyAddress property = {
          kAudioHardwarePropertyDefaultOutputDevice,
          kAudioObjectPropertyScopeGlobal,
          kAudioObjectPropertyElementMaster
        };
        err = AudioObjectRemovePropertyListener
          (kAudioObjectSystemObject, &property,
           reinterpret_cast<AudioObjectPropertyListenerProc>
             (&osx_sink::hardware_listener),
           reinterpret_cast<void*>(this));
#if _OSX_AU_DEBUG_
        check_error(err, "teardown: AudioObjectRemovePropertyListener "
                    "hardware failed");
#endif
      }

      // uninitialize the AudioUnit

      err = AudioUnitUninitialize(d_output_au);
#if _OSX_AU_DEBUG_
      check_error(err, "teardown: AudioUnitUninitialize failed");
#endif

      // dispose / close the AudioUnit

#ifndef GR_USE_OLD_AUDIO_UNIT
      err = AudioComponentInstanceDispose(d_output_au);
#if _OSX_AU_DEBUG_
      check_error(err, "teardown: AudioComponentInstanceDispose failed");
#endif
#else
      CloseComponent(d_output_au);
#if _OSX_AU_DEBUG_
      check_error(err, "teardown: CloseComponent failed");
#endif
#endif

      // delete buffers

      for(UInt32 nn = 0; nn < d_buffers.size(); ++nn) {
        delete d_buffers[nn];
        d_buffers[nn] = 0;
      }
      d_buffers.resize(0);

      // clear important variables; not # user channels

      d_n_dev_channels = d_n_buffer_channels =
        d_queue_sample_count = d_buffer_size_samples = 0;
      d_using_default_device = false;
      d_output_au = 0;
      d_output_ad_id = 0;

#if _OSX_AU_DEBUG_
      std::cerr << ((void*)(pthread_self()))
                << " : audio_osx_sink::teardown: finished"
                << std::endl;
#endif
    }

    bool
    osx_sink::is_running()
    {
      UInt32 au_running = 0;

      if (d_output_au) {

        UInt32 prop_size = (UInt32)sizeof(UInt32);
        OSStatus err = AudioUnitGetProperty
          (d_output_au,
           kAudioOutputUnitProperty_IsRunning,
           kAudioUnitScope_Global, 0,
           &au_running, &prop_size);
        check_error_and_throw
          (err, "AudioUnitGetProperty IsRunning",
           "audio_osx_sink::is_running");

      }

      return(au_running != 0);
    }

    bool
    osx_sink::check_topology(int ninputs, int noutputs)
    {
      // check # output to make sure it's valid
      if(noutputs != 0) {

        GR_LOG_FATAL(d_logger, boost::format
                     ("check_topology(): number of output "
                      "streams provided (%d) should be 0.")
                     % noutputs);
        throw std::runtime_error
          ("audio_osx_sink::check_topology");

      }

      // check # outputs to make sure it's valid
      if((ninputs < 1) | (ninputs > (int) d_n_dev_channels)) {

        GR_LOG_FATAL(d_logger, boost::format
                     ("check_topology(): number of input "
                      "streams provided (%d) should be in [1,%d] "
                      "for the selected output audio device.")
                     % ninputs % d_n_dev_channels);
        throw std::runtime_error("audio_osx_sink::check_topology");

      }

      // save the actual number of input (user) channels

      d_n_user_channels = ninputs;

#if _OSX_AU_DEBUG_
      std::cerr << ((void*)(pthread_self()))
                << " : audio_osx_sink::check_topology: "
                << "Actual # user input channels = "
                << d_n_user_channels << std::endl;
#endif

      return (true);
    }

    void
    osx_sink::check_channels(bool force_reset)
    {
      if (d_buffers.size() == 0) {

        // allocate the output circular buffer(s), one per user channel

        d_buffers.resize(d_n_user_channels);
        for(UInt32 nn = 0; nn < d_n_user_channels; ++nn) {
          d_buffers[nn] = new circular_buffer<float>
            (d_buffer_size_samples, false, false);
        }
      }
      else {
        if(d_buffers.size() == d_n_user_channels) {
          if (force_reset) {
            for(UInt32 nn = 0; nn < d_buffers.size(); ++nn) {
              d_buffers[nn]->reset();
            }
          }
          return;
        }

        // reallocate the output circular buffer(s)

        if (d_n_user_channels < d_buffers.size()) {

          // too many buffers; delete some

          for (UInt32 nn = d_n_user_channels; nn < d_buffers.size(); ++nn) {
            delete d_buffers[nn];
            d_buffers[nn] = 0;
          }
          d_buffers.resize(d_n_user_channels);

          // reset remaining buffers

          for(UInt32 nn = 0; nn < d_buffers.size(); ++nn) {
            d_buffers[nn]->reset();
          }
        }
        else {

          // too few buffers; create some more

          // reset old buffers first

          for(UInt32 nn = 0; nn < d_buffers.size(); ++nn) {
            d_buffers[nn]->reset();
          }

          d_buffers.resize(d_n_user_channels);
          for (UInt32 nn = d_buffers.size(); nn < d_n_user_channels; ++nn) {
            d_buffers[nn] = new circular_buffer<float>
              (d_buffer_size_samples, false, false);
          }
        }
      }

      // reset the output audio unit for the correct number of channels
      // have to uninitialize, set, initialize.

      OSStatus err = AudioUnitUninitialize(d_output_au);
      check_error(err, "AudioUnitUninitialize");

      d_stream_format.mChannelsPerFrame = d_n_user_channels;

      err = AudioUnitSetProperty
        (d_output_au,
         kAudioUnitProperty_StreamFormat,
         kAudioUnitScope_Input, 0,
         &d_stream_format, sizeof(d_stream_format));
      check_error_and_throw
        (err, "AudioUnitSetProperty StreamFormat",
         "audio_osx_sink::check_channels");

      // initialize the AU for output, so that it is ready to be used

      err = AudioUnitInitialize(d_output_au);
      check_error_and_throw
        (err, "AudioUnitInitialize",
         "audio_osx_sink::check_channels");
    }

    bool
    osx_sink::start()
    {
      if(!is_running() && d_output_au) {

#if _OSX_AU_DEBUG_
        std::cerr << ((void*)(pthread_self()))
                << " : audio_osx_sink::start: starting Output AudioUnit."
                  << std::endl;
#endif

        // check channels, (re)allocate and reset buffers if/as necessary

        check_channels(true);

        // start the audio unit (should never fail)

        OSStatus err = AudioOutputUnitStart(d_output_au);
        check_error_and_throw
          (err, "AudioOutputUnitStart",
           "audio_osx_sink::start");
      }

#if _OSX_AU_DEBUG_
      else {
        std::cerr << ((void*)(pthread_self()))
                  << " : audio_osx_sink::start: "
                  << "already running." << std::endl;
      }
#endif

      return (true);
    }

    bool
    osx_sink::stop()
    {
      if(is_running()) {

#if _OSX_AU_DEBUG_
        std::cerr << ((void*)(pthread_self()))
                  << " : audio_osx_sink::stop: "
                  << "stopping Output AudioUnit."
                  << std::endl;
#endif

        // if waiting in ::work, signal to wake up
        if (d_waiting_for_data) {
#if _OSX_AU_DEBUG_
          std::cerr << ((void*)(pthread_self()))
                    << " : audio_osx_sink::stop: "
                    << "signaling waiting condition" << std::endl;
#endif
          d_cond_data.notify_one();
        }

        // stop the audio unit (should never fail)

        OSStatus err = AudioOutputUnitStop(d_output_au);
        check_error_and_throw
          (err, "AudioOutputUnitStop",
           "audio_osx_sink::stop");

        // abort and reset all buffers

        for(UInt32 nn = 0; nn < d_n_user_channels; ++nn) {
          d_buffers[nn]->abort();
          d_buffers[nn]->reset();
        }

        // reset local knowledge of amount of data in queues

        d_queue_sample_count = 0;

      }
#if _OSX_AU_DEBUG_
      else {
        std::cerr << ((void*)(pthread_self()))
                  << " : audio_osx_sink::stop: "
                  << "already stopped." << std::endl;
      }
#endif

      return(true);
    }

    int
    osx_sink::work(int noutput_items,
                   gr_vector_const_void_star &input_items,
                   gr_vector_void_star &output_items)
    {
#if _OSX_AU_DEBUG_RENDER_
      {
        gr::thread::scoped_lock l(d_internal);
        std::cerr << ((void*)(pthread_self()))
                  << " : audio_osx_sink::work: "
                  << "Starting: #OI = "
                  << noutput_items << ", reset = "
                  << (d_do_reset ? "true" : "false") << std::endl;
      }
#endif
      if (d_do_reset) {
        if (d_hardware_changed) {

          // see if the current AudioDeviceID is still available

          std::vector < AudioDeviceID > all_ad_ids;
          osx::find_audio_devices
            (d_desired_name, false,
             &all_ad_ids, NULL);
          bool found = false;
          for (UInt32 nn = 0; (nn < all_ad_ids.size()) && (!found);
               ++nn) {
            found = (all_ad_ids[nn] == d_output_ad_id);
          }
          if (!found) {

            GR_LOG_FATAL(d_logger, boost::format
                         ("The selected output audio device ('%s') "
                          "is no longer available.\n")
                         % d_selected_name);
            return(gr::block::WORK_DONE);

          }

          d_do_reset = d_hardware_changed = false;

        }
        else {

#if _OSX_AU_DEBUG_RENDER_
          {
            gr::thread::scoped_lock l(d_internal);
            std::cerr << ((void*)(pthread_self()))
                      << " : audio_osx_sink::work: "
                      << "doing reset." << std::endl;
          }
#endif

          GR_LOG_WARN(d_logger, boost::format
                      ("\n\nThe default output audio device has "
                       "changed; resetting audio.\nThere may "
                       "be a sound glitch while resetting.\n"));

          // for any changes, just tear down the current
          // configuration, then set it up again using the user's
          // parameters to try to make selections.

          teardown();

          gr::thread::scoped_lock l(d_internal);

          setup();
          start();

#if _OSX_AU_DEBUG_RENDER_
          std::cerr << ((void*)(pthread_self()))
                    << " : audio_osx_sink: "
                    << "returning 0 after reset."
                    << std::endl;
#endif
          return(0);
        }
      }

      gr::thread::scoped_lock l(d_internal);

      // take the input data, copy it, and push it to the bottom of
      // the queue.  mono input is pushed onto queue[0]; stereo input
      // is pushed onto queue[1].  If the number of user/graph
      // channels is less than the number of device channels, copy the
      // data from the last / highest number channel to remaining
      // device channels.

      // find the maximum amount of buffer space available right now

      UInt32 l_max_count;
      int diff_count = ((int)d_buffer_size_samples) - noutput_items;
      if(diff_count < 0) {
        l_max_count = 0;
      }
      else {
        l_max_count = (UInt32)diff_count;
      }

#if _OSX_AU_DEBUG_RENDER_
      std::cerr << ((void*)(pthread_self()))
                << " : audio_osx_sink::work: "
                << "qSC = " << d_queue_sample_count
                << ", lMC = "<< l_max_count
                << ", dBSC = " << d_buffer_size_samples
                << ", #OI = " << noutput_items << std::endl;
#endif

      if(d_queue_sample_count > l_max_count) {

        // data coming in too fast; ok_to_block decides what to do: if
        // ok to block, then wait until the render callback makes
        // enough space.  If not blocking, detect overflow via writing
        // data to the circular buffer.

        if(d_ok_to_block == true) {
          // block until there is data to return, or on reset
          while(d_queue_sample_count > l_max_count) {
            // release control so-as to allow data to be retrieved;
            // block until there is data to return
#if _OSX_AU_DEBUG_RENDER_
            std::cerr << ((void*)(pthread_self()))
                      << " : audio_osx_sink::work: "
                      << "waiting." << std::endl;
#endif
            d_waiting_for_data = true;
            d_cond_data.wait(l);
            d_waiting_for_data = false;
#if _OSX_AU_DEBUG_RENDER_
            std::cerr << ((void*)(pthread_self()))
                      << " : audio_osx_sink::work: "
                      << "done waiting." << std::endl;
#endif
            // the condition's 'notify' was called; acquire control to
            // keep thread safe

            // if doing a reset, just return here; reset will pick
            // up the next time this method is called.
            if (d_do_reset) {
#if _OSX_AU_DEBUG_RENDER_
              std::cerr << ((void*)(pthread_self()))
                        << " : audio_osx_sink::work: "
                        << "returning 0 for reset." << std::endl;
#endif
              return(0);
            }
          }
        }
      }

      // not blocking and overflow is handled by the circular buffer,
      // or enough data space is available

      // add the input frames to the buffers' queue, checking for overflow

      UInt32 nn;
      int res = 0;
      float* inBuffer = (float*)input_items[0];
      const UInt32 l_size = input_items.size();
      for(nn = 0; nn < l_size; ++nn) {
        inBuffer = (float*)input_items[nn];
        int l_res = d_buffers[nn]->enqueue(inBuffer, noutput_items);
        if(l_res == -1) {
          res = -1;
        }
      }
      while(nn < d_n_user_channels) {
        // for extra channels, copy the last input's data
        int l_res = d_buffers[nn++]->enqueue(inBuffer, noutput_items);
        if(l_res == -1) {
          res = -1;
        }
      }

      // did overflow occur?

      if(res == -1) {
        // yes: data coming in too fast; drop oldest data.
        fputs("aO", stderr);
        fflush(stderr);
        // set the local number of samples available to the max
        d_queue_sample_count = d_buffers[0]->buffer_length_items();
      }
      else {
        // no: keep up the local sample count
        d_queue_sample_count += noutput_items;
      }

#if _OSX_AU_DEBUG_RENDER_
      std::cerr << ((void*)(pthread_self()))
                << " : audio_osx_sink::work: "
                << "returning: #OI = "
                << noutput_items << ", qSC = "
                << d_queue_sample_count << ", bSS = "
                << d_buffer_size_samples << std::endl;
#endif

      return (noutput_items);
    }

    OSStatus
    osx_sink::au_output_callback
    (void* in_ref_con,
     AudioUnitRenderActionFlags* io_action_flags,
     const AudioTimeStamp* in_time_stamp,
     UInt32 in_bus_number,
     UInt32 in_number_frames,
     AudioBufferList* io_data)
    {
      // NOTE: This is a callback from the OS, so throwing here does
      // not work; return an error instead when something does not go
      // as planned.

      osx_sink* This = reinterpret_cast<osx_sink*>(in_ref_con);
      OSStatus err = noErr;

      gr::thread::scoped_lock l(This->d_internal);

#if _OSX_AU_DEBUG_RENDER_
      std::cerr << ((void*)(pthread_self()))
                << " : audio_osx_sink::au_output_callback: "
                << "starting: qSC = "
                << This->d_queue_sample_count
                << ", in#F = " << in_number_frames
                << ", in#C = " << This->d_n_user_channels << std::endl;
#endif

      if(This->d_queue_sample_count < in_number_frames) {

        // not enough data to fill request; probably happened on
        // start-up, where this callback was called before ::work was.

        fputs("aU", stderr);
        fflush(stderr);
        err = kAudioUnitErr_Initialized;

      }
      else {
        // enough data; remove data from our buffers into the AU's buffers
        int nn = This->d_n_user_channels;
        while(--nn >= 0) {

          size_t t_n_output_items = in_number_frames;
          float* out_buffer = (float*)(io_data->mBuffers[nn].mData);
          int rv = This->d_buffers[nn]->dequeue
            (out_buffer, &t_n_output_items);

          if((rv != 1) || (t_n_output_items != in_number_frames)) {

            std::cerr << "audio_osx_sink::au_output_callback: "
                      << "number of available items changing "
                      << "unexpectedly (should never happen): was "
                      << in_number_frames << " now "
                      << t_n_output_items<< std::endl;
            err = kAudioUnitErr_TooManyFramesToProcess;

          }
        }

        This->d_queue_sample_count -= in_number_frames;
      }

      // signal that data is available, if appropriate

      if (This->d_waiting_for_data) {
#if _OSX_AU_DEBUG_RENDER_
        std::cerr << ((void*)(pthread_self()))
                  << " : audio_osx_sink::au_output_callback: "
                  << "signaling waiting condition" << std::endl;
#endif
        This->d_cond_data.notify_one();
      }

#if _OSX_AU_DEBUG_RENDER_
      std::cerr << ((void*)(pthread_self()))
                << " : audio_osx_sink::au_output_callback: "
                << "returning: qSC = "
                << This->d_queue_sample_count
                << ", err = " << err << std::endl;
#endif

      return (err);
    }

#ifndef GR_USE_OLD_AUDIO_UNIT

    OSStatus
    osx_sink::hardware_listener
    (AudioObjectID in_object_id,
     UInt32 in_num_addresses,
     const AudioObjectPropertyAddress in_addresses[],
     void* in_client_data)

#else

    OSStatus
    osx_sink::hardware_listener
    (AudioHardwarePropertyID in_property_id,
     void* in_client_data)

#endif
    {
      osx_sink* This = static_cast
        <osx_sink*>(in_client_data);
      This->reset(true);
      return(noErr);
    }

#ifndef GR_USE_OLD_AUDIO_UNIT

    OSStatus
    osx_sink::default_listener
    (AudioObjectID in_object_id,
     UInt32 in_num_addresses,
     const AudioObjectPropertyAddress in_addresses[],
     void* in_client_data)

#else

    OSStatus
    osx_sink::default_listener
    (AudioHardwarePropertyID in_property_id,
     void* in_client_data)

#endif
    {
      osx_sink* This = reinterpret_cast
        <osx_sink*>(in_client_data);
      This->reset(false);
      return(noErr);
    }

  } /* namespace audio */
} /* namespace gr */

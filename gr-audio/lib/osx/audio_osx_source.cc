/* -*- c++ -*- */
/*
 * Copyright 2006-2011 Free Software Foundation, Inc.
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

#include "gr_audio_registry.h"
#include <audio_osx_source.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <audio_osx.h>

#define _OSX_AU_DEBUG_ 0
#define _OSX_DO_LISTENERS_ 0

AUDIO_REGISTER_SOURCE(REG_PRIO_HIGH, osx)(
    int sampling_rate, const std::string &device_name, bool ok_to_block
){
    return audio_source::sptr(new audio_osx_source(sampling_rate, device_name, ok_to_block));
}

void PrintStreamDesc (AudioStreamBasicDescription *inDesc)
{
  if (inDesc == NULL) {
    std::cerr << "PrintStreamDesc: Can't print a NULL desc!" << std::endl;
    return;
  }

  std::cerr << "  Sample Rate        : " << inDesc->mSampleRate << std::endl;
  char format_id[4];
  strncpy (format_id, (char*)(&inDesc->mFormatID), 4);
  std::cerr << "  Format ID          : " << format_id << std::endl;
  std::cerr << "  Format Flags       : " << inDesc->mFormatFlags << std::endl;
  std::cerr << "  Bytes per Packet   : " << inDesc->mBytesPerPacket << std::endl;
  std::cerr << "  Frames per Packet  : " << inDesc->mFramesPerPacket << std::endl;
  std::cerr << "  Bytes per Frame    : " << inDesc->mBytesPerFrame << std::endl;
  std::cerr << "  Channels per Frame : " << inDesc->mChannelsPerFrame << std::endl;
  std::cerr << "  Bits per Channel   : " << inDesc->mBitsPerChannel << std::endl;
}

// FIXME these should query some kind of user preference

audio_osx_source::audio_osx_source (int sample_rate,
				    const std::string device_name,
				    bool do_block,
				    int channel_config,
				    int max_sample_count)
  : gr_sync_block ("audio_osx_source",
		   gr_make_io_signature (0, 0, 0),
		   gr_make_io_signature (0, 0, 0)),
    d_deviceSampleRate (0.0), d_outputSampleRate (0.0),
    d_channel_config (0),
    d_inputBufferSizeFrames (0), d_inputBufferSizeBytes (0),
    d_outputBufferSizeFrames (0), d_outputBufferSizeBytes (0),
    d_deviceBufferSizeFrames (0), d_deviceBufferSizeBytes (0),
    d_leadSizeFrames (0), d_leadSizeBytes (0),
    d_trailSizeFrames (0), d_trailSizeBytes (0),
    d_extraBufferSizeFrames (0), d_extraBufferSizeBytes (0),
    d_queueSampleCount (0), d_max_sample_count (0),
    d_n_AvailableInputFrames (0), d_n_ActualInputFrames (0),
    d_n_user_channels (0), d_n_max_channels (0), d_n_deviceChannels (0),
    d_do_block (do_block), d_passThrough (false),
    d_internal (0), d_cond_data (0),
    d_buffers (0),
    d_InputAU (0), d_InputBuffer (0), d_OutputBuffer (0),
    d_AudioConverter (0)
{
  if (sample_rate <= 0) {
    std::cerr << "Invalid Sample Rate: " << sample_rate << std::endl;
    throw std::invalid_argument ("audio_osx_source::audio_osx_source");
  } else
    d_outputSampleRate = (Float64) sample_rate;

  if (channel_config <= 0 & channel_config != -1) {
    std::cerr << "Invalid Channel Config: " << channel_config << std::endl;
    throw std::invalid_argument ("audio_osx_source::audio_osx_source");
  } else if (channel_config == -1) {
// no user input; try "device name" instead
    int l_n_channels = (int) strtol (device_name.data(), (char **)NULL, 10);
    if (l_n_channels == 0 & errno) {
      std::cerr << "Error Converting Device Name: " << errno << std::endl;
      throw std::invalid_argument ("audio_osx_source::audio_osx_source");
    }
    if (l_n_channels <= 0)
      channel_config = 2;
    else
      channel_config = l_n_channels;
  }

  d_channel_config = channel_config;

// check that the max # of samples to store is valid

  if (max_sample_count == -1)
    max_sample_count = sample_rate;
  else if (max_sample_count <= 0) {
    std::cerr << "Invalid Max Sample Count: " << max_sample_count << std::endl;
    throw std::invalid_argument ("audio_osx_source::audio_osx_source");
  }

  d_max_sample_count = max_sample_count;

#if _OSX_AU_DEBUG_
  std::cerr << "source(): max # samples = " << d_max_sample_count << std::endl;
#endif

  OSStatus err = noErr;

// create the default AudioUnit for input

// Open the default input unit
#ifndef GR_USE_OLD_AUDIO_UNIT
  AudioComponentDescription InputDesc;
#else
  ComponentDescription InputDesc;
#endif
  

  InputDesc.componentType = kAudioUnitType_Output;
  InputDesc.componentSubType = kAudioUnitSubType_HALOutput;
  InputDesc.componentManufacturer = kAudioUnitManufacturer_Apple;
  InputDesc.componentFlags = 0;
  InputDesc.componentFlagsMask = 0;

#ifndef GR_USE_OLD_AUDIO_UNIT
  AudioComponent comp = AudioComponentFindNext (NULL, &InputDesc);
#else
  Component comp = FindNextComponent (NULL, &InputDesc);
#endif
  
  if (comp == NULL) {
#ifndef GR_USE_OLD_AUDIO_UNIT
    std::cerr << "AudioComponentFindNext Error" << std::endl;
#else
    std::cerr << "FindNextComponent Error" << std::endl;
#endif
    throw std::runtime_error ("audio_osx_source::audio_osx_source");
  }

#ifndef GR_USE_OLD_AUDIO_UNIT
  err = AudioComponentInstanceNew (comp, &d_InputAU);
  CheckErrorAndThrow (err, "AudioComponentInstanceNew",
		      "audio_osx_source::audio_osx_source");
#else
  err = OpenAComponent (comp, &d_InputAU);
  CheckErrorAndThrow (err, "OpenAComponent",
		      "audio_osx_source::audio_osx_source");
#endif
  

  UInt32 enableIO;

// must enable the AUHAL for input and disable output 
// before setting the AUHAL's current device

// Enable input on the AUHAL
  enableIO = 1;
  err = AudioUnitSetProperty (d_InputAU,
			      kAudioOutputUnitProperty_EnableIO,
			      kAudioUnitScope_Input,
			      1, // input element
			      &enableIO,
			      sizeof (UInt32));
  CheckErrorAndThrow (err, "AudioUnitSetProperty Input Enable",
		      "audio_osx_source::audio_osx_source");

// Disable output on the AUHAL
  enableIO = 0;
  err = AudioUnitSetProperty (d_InputAU,
			      kAudioOutputUnitProperty_EnableIO,
			      kAudioUnitScope_Output,
			      0, // output element
			      &enableIO,
			      sizeof (UInt32));
  CheckErrorAndThrow (err, "AudioUnitSetProperty Output Disable",
		      "audio_osx_source::audio_osx_source");

// set the default input device for our input AU

  SetDefaultInputDeviceAsCurrent ();

#if _OSX_DO_LISTENERS_
// set up a listener if default hardware input device changes

  err = AudioHardwareAddPropertyListener
    (kAudioHardwarePropertyDefaultInputDevice,
     (AudioHardwarePropertyListenerProc) HardwareListener,
     this);

  CheckErrorAndThrow (err, "AudioHardwareAddPropertyListener",
		      "audio_osx_source::audio_osx_source");

// Add a listener for any changes in the input AU's output stream
// the function "UnitListener" will be called if the stream format
// changes for whatever reason

  err = AudioUnitAddPropertyListener
    (d_InputAU,
     kAudioUnitProperty_StreamFormat,
     (AudioUnitPropertyListenerProc) UnitListener,
     this);
  CheckErrorAndThrow (err, "Adding Unit Property Listener",
		      "audio_osx_source::audio_osx_source");
#endif

// Now find out if it actually can do input.

  UInt32 hasInput = 0;
  UInt32 dataSize = sizeof (hasInput);
  err = AudioUnitGetProperty (d_InputAU,
			      kAudioOutputUnitProperty_HasIO,
			      kAudioUnitScope_Input,
			      1,
			      &hasInput,
			      &dataSize);
  CheckErrorAndThrow (err, "AudioUnitGetProperty HasIO",
		      "audio_osx_source::audio_osx_source");
  if (hasInput == 0) {
    std::cerr << "Selected Audio Device does not support Input." << std::endl;
    throw std::runtime_error ("audio_osx_source::audio_osx_source");
  }

// Set up a callback function to retrieve input from the Audio Device

  AURenderCallbackStruct AUCallBack;

  AUCallBack.inputProc = (AURenderCallback)(audio_osx_source::AUInputCallback);
  AUCallBack.inputProcRefCon = this;

  err = AudioUnitSetProperty (d_InputAU,
			      kAudioOutputUnitProperty_SetInputCallback,
			      kAudioUnitScope_Global,
			      0,
			      &AUCallBack,
			      sizeof (AURenderCallbackStruct));
  CheckErrorAndThrow (err, "AudioUnitSetProperty Input Callback",
		      "audio_osx_source::audio_osx_source");

  UInt32 propertySize;
  AudioStreamBasicDescription asbd_device, asbd_client, asbd_user;

// asbd_device: ASBD of the device that is creating the input data stream
// asbd_client: ASBD of the client size (output) of the hardware device
// asbd_user:   ASBD of the user's arguments

// Get the Stream Format (device side)

  propertySize = sizeof (asbd_device);
  err = AudioUnitGetProperty (d_InputAU,
			      kAudioUnitProperty_StreamFormat,
			      kAudioUnitScope_Input,
			      1,
			      &asbd_device,
			      &propertySize);
  CheckErrorAndThrow (err, "AudioUnitGetProperty Device Input Stream Format",
		      "audio_osx_source::audio_osx_source");

#if _OSX_AU_DEBUG_
  std::cerr << std::endl << "---- Device Stream Format ----" << std::endl;
  PrintStreamDesc (&asbd_device);
#endif

// Get the Stream Format (client side)
  propertySize = sizeof (asbd_client);
  err = AudioUnitGetProperty (d_InputAU,
			      kAudioUnitProperty_StreamFormat,
			      kAudioUnitScope_Output,
			      1,
			      &asbd_client,
			      &propertySize);
  CheckErrorAndThrow (err, "AudioUnitGetProperty Device Ouput Stream Format",
		      "audio_osx_source::audio_osx_source");

#if _OSX_AU_DEBUG_
  std::cerr << std::endl << "---- Client Stream Format ----" << std::endl;
  PrintStreamDesc (&asbd_client);
#endif

// Set the format of all the AUs to the input/output devices channel count

// get the max number of input (& thus output) channels supported by
// this device
  d_n_max_channels = asbd_device.mChannelsPerFrame;

// create the output io signature;
// no input siganture to set (source is hardware)
  set_output_signature (gr_make_io_signature (1,
					      d_n_max_channels,
					      sizeof (float)));

// allocate the output circular buffer(s), one per channel
  d_buffers = (circular_buffer<float>**) new
    circular_buffer<float>* [d_n_max_channels];
  UInt32 n_alloc = (UInt32) ceil ((double) d_max_sample_count);
  for (UInt32 n = 0; n < d_n_max_channels; n++) {
    d_buffers[n] = new circular_buffer<float> (n_alloc, false, false);
  }

  d_deviceSampleRate = asbd_device.mSampleRate;
  d_n_deviceChannels = asbd_device.mChannelsPerFrame;

  asbd_client.mSampleRate = asbd_device.mSampleRate;
  asbd_client.mFormatID = kAudioFormatLinearPCM;
  asbd_client.mFormatFlags = (kAudioFormatFlagIsFloat |
			      kAudioFormatFlagIsPacked |
			      kAudioFormatFlagIsNonInterleaved);
  if ((asbd_client.mFormatID == kAudioFormatLinearPCM) &&
      (d_n_deviceChannels == 1)) {
      asbd_client.mFormatFlags &= ~kLinearPCMFormatFlagIsNonInterleaved;
  }
  asbd_client.mBytesPerFrame = sizeof (float);
  asbd_client.mFramesPerPacket = 1;
  asbd_client.mBitsPerChannel = asbd_client.mBytesPerFrame * 8;
  asbd_client.mChannelsPerFrame = d_n_deviceChannels;
  asbd_client.mBytesPerPacket = asbd_client.mBytesPerFrame;

  propertySize = sizeof(AudioStreamBasicDescription);
  err = AudioUnitSetProperty (d_InputAU,
			      kAudioUnitProperty_StreamFormat,
			      kAudioUnitScope_Output,
			      1,
			      &asbd_client,
			      propertySize);
  CheckErrorAndThrow (err, "AudioUnitSetProperty Device Ouput Stream Format",
		      "audio_osx_source::audio_osx_source");

// create an ASBD for the user's wants

  asbd_user.mSampleRate = d_outputSampleRate;
  asbd_user.mFormatID = kAudioFormatLinearPCM;
  asbd_user.mFormatFlags = (kLinearPCMFormatFlagIsFloat |
			    GR_PCM_ENDIANNESS |
			    kLinearPCMFormatFlagIsPacked |
			    kAudioFormatFlagIsNonInterleaved);
  asbd_user.mBytesPerPacket = sizeof (float);
  asbd_user.mFramesPerPacket = 1;
  asbd_user.mBytesPerFrame = asbd_user.mBytesPerPacket;
  asbd_user.mChannelsPerFrame = d_n_deviceChannels;
  asbd_user.mBitsPerChannel = asbd_user.mBytesPerPacket * 8;

  if (d_deviceSampleRate == d_outputSampleRate) {
// no need to do conversion if asbd_client matches user wants
    d_passThrough = true;
    d_leadSizeFrames = d_trailSizeFrames = 0L;
  } else {
    d_passThrough = false;
// Create the audio converter

    err = AudioConverterNew (&asbd_client, &asbd_user, &d_AudioConverter);
    CheckErrorAndThrow (err, "AudioConverterNew",
			"audio_osx_source::audio_osx_source");

// Set the audio converter sample rate quality to "max" ...
// requires more samples, but should sound nicer

    UInt32 ACQuality = kAudioConverterQuality_Max;
    propertySize = sizeof (ACQuality);
    err = AudioConverterSetProperty (d_AudioConverter,
				     kAudioConverterSampleRateConverterQuality,
				     propertySize,
				     &ACQuality);
    CheckErrorAndThrow (err, "AudioConverterSetProperty "
			"SampleRateConverterQuality",
			"audio_osx_source::audio_osx_source");

// set the audio converter's prime method to "pre",
// which uses both leading and trailing frames
// from the "current input".  All of this is handled
// internally by the AudioConverter; we just supply
// the frames for conversion.

//   UInt32 ACPrimeMethod = kConverterPrimeMethod_None;
    UInt32 ACPrimeMethod = kConverterPrimeMethod_Pre;
    propertySize = sizeof (ACPrimeMethod);
    err = AudioConverterSetProperty (d_AudioConverter, 
				     kAudioConverterPrimeMethod,
				     propertySize,
				     &ACPrimeMethod);
    CheckErrorAndThrow (err, "AudioConverterSetProperty PrimeMethod",
			"audio_osx_source::audio_osx_source");

// Get the size of the I/O buffer(s) to allow for pre-allocated buffers
      
// lead frame info (trail frame info is ignored)

    AudioConverterPrimeInfo ACPrimeInfo = {0, 0};
    propertySize = sizeof (ACPrimeInfo);
    err = AudioConverterGetProperty (d_AudioConverter, 
				     kAudioConverterPrimeInfo,
				     &propertySize,
				     &ACPrimeInfo);
    CheckErrorAndThrow (err, "AudioConverterGetProperty PrimeInfo",
			"audio_osx_source::audio_osx_source");

    switch (ACPrimeMethod) {
    case (kConverterPrimeMethod_None):
      d_leadSizeFrames =
	d_trailSizeFrames = 0L;
      break;
    case (kConverterPrimeMethod_Normal):
      d_leadSizeFrames = 0L;
      d_trailSizeFrames = ACPrimeInfo.trailingFrames;
      break;
    default:
      d_leadSizeFrames = ACPrimeInfo.leadingFrames;
      d_trailSizeFrames = ACPrimeInfo.trailingFrames;
    }
  }
  d_leadSizeBytes = d_leadSizeFrames * sizeof (Float32);
  d_trailSizeBytes = d_trailSizeFrames * sizeof (Float32);

  propertySize = sizeof (d_deviceBufferSizeFrames);
  err = AudioUnitGetProperty (d_InputAU,
			      kAudioDevicePropertyBufferFrameSize,
			      kAudioUnitScope_Global,
			      0,
			      &d_deviceBufferSizeFrames,
			      &propertySize);
  CheckErrorAndThrow (err, "AudioUnitGetProperty Buffer Frame Size",
		      "audio_osx_source::audio_osx_source");

  d_deviceBufferSizeBytes = d_deviceBufferSizeFrames * sizeof (Float32);
  d_inputBufferSizeBytes = d_deviceBufferSizeBytes + d_leadSizeBytes;
  d_inputBufferSizeFrames = d_deviceBufferSizeFrames + d_leadSizeFrames;

// outBufSizeBytes = floor (inBufSizeBytes * rate_out / rate_in)
// since this is rarely exact, we need another buffer to hold
// "extra" samples not processed at any given sampling period
// this buffer must be at least 4 floats in size, but generally
// follows the rule that
// extraBufSize =  ceil (rate_in / rate_out)*sizeof(float)

  d_extraBufferSizeFrames = ((UInt32) ceil (d_deviceSampleRate
					    / d_outputSampleRate)
			     * sizeof (float));
  if (d_extraBufferSizeFrames < 4)
    d_extraBufferSizeFrames = 4;
  d_extraBufferSizeBytes = d_extraBufferSizeFrames * sizeof (float);

  d_outputBufferSizeFrames = (UInt32) ceil (((Float64) d_inputBufferSizeFrames)
					    * d_outputSampleRate
					    / d_deviceSampleRate);
  d_outputBufferSizeBytes = d_outputBufferSizeFrames * sizeof (float);
  d_inputBufferSizeFrames += d_extraBufferSizeFrames;

// pre-alloc all buffers

  AllocAudioBufferList (&d_InputBuffer, d_n_deviceChannels,
			d_inputBufferSizeBytes);
  if (d_passThrough == false) {
    AllocAudioBufferList (&d_OutputBuffer, d_n_max_channels,
			  d_outputBufferSizeBytes);
  } else {
    d_OutputBuffer = d_InputBuffer;
  }

// create the stuff to regulate I/O

  d_cond_data = new gruel::condition_variable ();
  if (d_cond_data == NULL)
    CheckErrorAndThrow (errno, "new condition (data)",
			"audio_osx_source::audio_osx_source");

  d_internal = new gruel::mutex ();
  if (d_internal == NULL)
    CheckErrorAndThrow (errno, "new mutex (internal)",
			"audio_osx_source::audio_osx_source");

// initialize the AU for input

  err = AudioUnitInitialize (d_InputAU);
  CheckErrorAndThrow (err, "AudioUnitInitialize",
		      "audio_osx_source::audio_osx_source");

#if _OSX_AU_DEBUG_
  std::cerr << "audio_osx_source Parameters:" << std::endl;
  std::cerr << "  Device Sample Rate is " << d_deviceSampleRate << std::endl;
  std::cerr << "  User Sample Rate is " << d_outputSampleRate << std::endl;
  std::cerr << "  Max Sample Count is " << d_max_sample_count << std::endl;
  std::cerr << "  # Device Channels is " << d_n_deviceChannels << std::endl;
  std::cerr << "  # Max Channels is " << d_n_max_channels << std::endl;
  std::cerr << "  Device Buffer Size is Frames = " << d_deviceBufferSizeFrames << std::endl;
  std::cerr << "  Lead Size is Frames = " << d_leadSizeFrames << std::endl;
  std::cerr << "  Trail Size is Frames = " << d_trailSizeFrames << std::endl;
  std::cerr << "  Input Buffer Size is Frames = " << d_inputBufferSizeFrames << std::endl;
  std::cerr << "  Output Buffer Size is Frames = " << d_outputBufferSizeFrames << std::endl;
#endif
}

void
audio_osx_source::AllocAudioBufferList (AudioBufferList** t_ABL,
					UInt32 n_channels,
					UInt32 bufferSizeBytes)
{
  FreeAudioBufferList (t_ABL);
  UInt32 propertySize = (offsetof (AudioBufferList, mBuffers[0]) +
			 (sizeof (AudioBuffer) * n_channels));
  *t_ABL = (AudioBufferList*) calloc (1, propertySize);
  (*t_ABL)->mNumberBuffers = n_channels;

  int counter = n_channels;

  while (--counter >= 0) {
    (*t_ABL)->mBuffers[counter].mNumberChannels = 1;
    (*t_ABL)->mBuffers[counter].mDataByteSize = bufferSizeBytes;
    (*t_ABL)->mBuffers[counter].mData = calloc (1, bufferSizeBytes);
  }
}

void
audio_osx_source::FreeAudioBufferList (AudioBufferList** t_ABL)
{
// free pre-allocated audio buffer, if it exists
  if (*t_ABL != NULL) {
    int counter = (*t_ABL)->mNumberBuffers;
    while (--counter >= 0)
      free ((*t_ABL)->mBuffers[counter].mData);
    free (*t_ABL);
    (*t_ABL) = 0;
  }
}

bool audio_osx_source::IsRunning ()
{
  UInt32 AURunning = 0, AUSize = sizeof (UInt32);

  OSStatus err = AudioUnitGetProperty (d_InputAU,
				       kAudioOutputUnitProperty_IsRunning,
				       kAudioUnitScope_Global,
				       0,
				       &AURunning,
				       &AUSize);
  CheckErrorAndThrow (err, "AudioUnitGetProperty IsRunning",
		      "audio_osx_source::IsRunning");

  return (AURunning);
}

bool audio_osx_source::start ()
{
  if (! IsRunning ()) {
    OSStatus err = AudioOutputUnitStart (d_InputAU);
    CheckErrorAndThrow (err, "AudioOutputUnitStart",
			"audio_osx_source::start");
  }

  return (true);
}

bool audio_osx_source::stop ()
{
  if (IsRunning ()) {
    OSStatus err = AudioOutputUnitStop (d_InputAU);
    CheckErrorAndThrow (err, "AudioOutputUnitStart",
			"audio_osx_source::stop");
    for (UInt32 n = 0; n < d_n_user_channels; n++) {
      d_buffers[n]->abort ();
    }
  }

  return (true);
}

audio_osx_source::~audio_osx_source ()
{
  OSStatus err = noErr;

// stop the AudioUnit
  stop();

#if _OSX_DO_LISTENERS_
// remove the listeners

  err = AudioUnitRemovePropertyListener
    (d_InputAU,
     kAudioUnitProperty_StreamFormat,
     (AudioUnitPropertyListenerProc) UnitListener);
  CheckError (err, "~audio_osx_source: AudioUnitRemovePropertyListener");

  err = AudioHardwareRemovePropertyListener
    (kAudioHardwarePropertyDefaultInputDevice,
     (AudioHardwarePropertyListenerProc) HardwareListener);
  CheckError (err, "~audio_osx_source: AudioHardwareRemovePropertyListener");
#endif

// free pre-allocated audio buffers
  FreeAudioBufferList (&d_InputBuffer);

  if (d_passThrough == false) {
    err = AudioConverterDispose (d_AudioConverter);
    CheckError (err, "~audio_osx_source: AudioConverterDispose");
    FreeAudioBufferList (&d_OutputBuffer);
  }

// remove the audio unit
  err = AudioUnitUninitialize (d_InputAU);
  CheckError (err, "~audio_osx_source: AudioUnitUninitialize");

#ifndef GR_USE_OLD_AUDIO_UNIT
  err = AudioComponentInstanceDispose (d_InputAU);
  CheckError (err, "~audio_osx_source: AudioComponentInstanceDispose");
#else
  err = CloseComponent (d_InputAU);
  CheckError (err, "~audio_osx_source: CloseComponent");
#endif

// empty and delete the queues
  for (UInt32 n = 0; n < d_n_max_channels; n++) {
    delete d_buffers[n];
    d_buffers[n] = 0;
  }
  delete [] d_buffers;
  d_buffers = 0;

// close and delete the control stuff
  delete d_cond_data;
  d_cond_data = 0;
  delete d_internal;
  d_internal = 0;
}

bool
audio_osx_source::check_topology (int ninputs, int noutputs)
{
// check # inputs to make sure it's valid
  if (ninputs != 0) {
    std::cerr << "audio_osx_source::check_topology(): number of input "
	      << "streams provided (" << ninputs
	      << ") should be 0." << std::endl;
    throw std::runtime_error ("audio_osx_source::check_topology()");
  }

// check # outputs to make sure it's valid
  if ((noutputs < 1) | (noutputs > (int) d_n_max_channels)) {
    std::cerr << "audio_osx_source::check_topology(): number of output "
	      << "streams provided (" << noutputs << ") should be in [1,"
	      << d_n_max_channels << "] for the selected audio device."
	      << std::endl;
    throw std::runtime_error ("audio_osx_source::check_topology()");
  }

// save the actual number of output (user) channels
  d_n_user_channels = noutputs;

#if _OSX_AU_DEBUG_
  std::cerr << "chk_topo: Actual # user output channels = "
	    << noutputs << std::endl;
#endif

  return (true);
}

int
audio_osx_source::work
(int noutput_items,
 gr_vector_const_void_star &input_items,
 gr_vector_void_star &output_items)
{
  // acquire control to do processing here only
  gruel::scoped_lock l (*d_internal);

#if _OSX_AU_DEBUG_
  std::cerr << "work1: SC = " << d_queueSampleCount
	    << ", #OI = " << noutput_items
	    << ", #Chan = " << output_items.size() << std::endl;
#endif

  // set the actual # of output items to the 'desired' amount then
  // verify that data is available; if not enough data is available,
  // either wait until it is (is "do_block" is true), return (0) is no
  // data is available and "do_block" is false, or process the actual
  // amount of available data.

  UInt32 actual_noutput_items = noutput_items;

  if (d_queueSampleCount < actual_noutput_items) {
    if (d_queueSampleCount == 0) {
      // no data; do_block decides what to do
      if (d_do_block == true) {
	while (d_queueSampleCount == 0) {
	  // release control so-as to allow data to be retrieved;
	  // block until there is data to return
	  d_cond_data->wait (l);
	  // the condition's 'notify' was called; acquire control to
	  // keep thread safe
	}
      } else {
	// no data & not blocking; return nothing
	return (0);
      }
    }
    // use the actual amount of available data
    actual_noutput_items = d_queueSampleCount;
  }

  // number of channels
  int l_counter = (int) output_items.size();

  // copy the items from the circular buffer(s) to 'work's output buffers
  // verify that the number copied out is as expected.

  while (--l_counter >= 0) {
    size_t t_n_output_items = actual_noutput_items;
    d_buffers[l_counter]->dequeue ((float*) output_items[l_counter],
				   &t_n_output_items);
    if (t_n_output_items != actual_noutput_items) {
      std::cerr << "audio_osx_source::work(): ERROR: number of "
		<< "available items changing unexpectedly; expecting "
		<< actual_noutput_items << ", got "
		<< t_n_output_items << "." << std::endl;
      throw std::runtime_error ("audio_osx_source::work()");
    }
  }

  // subtract the actual number of items removed from the buffer(s)
  // from the local accounting of the number of available samples

  d_queueSampleCount -= actual_noutput_items;

#if _OSX_AU_DEBUG_
  std::cerr << "work2: SC = " << d_queueSampleCount
	    << ", act#OI = " << actual_noutput_items << std::endl
	    << "Returning." << std::endl;
#endif

  return (actual_noutput_items);
}

OSStatus
audio_osx_source::ConverterCallback
(AudioConverterRef inAudioConverter,
 UInt32* ioNumberDataPackets,
 AudioBufferList* ioData,
 AudioStreamPacketDescription** ioASPD,
 void* inUserData)
{
  // take current device buffers and copy them to the tail of the
  // input buffers the lead buffer is already there in the first
  // d_leadSizeFrames slots

  audio_osx_source* This = static_cast<audio_osx_source*>(inUserData);
  AudioBufferList* l_inputABL = This->d_InputBuffer;
  UInt32 totalInputBufferSizeBytes = ((*ioNumberDataPackets) * sizeof (float));
  int counter = This->d_n_deviceChannels;
  ioData->mNumberBuffers = This->d_n_deviceChannels;
  This->d_n_ActualInputFrames = (*ioNumberDataPackets);

#if _OSX_AU_DEBUG_
  std::cerr << "cc1: io#DP = " << (*ioNumberDataPackets)
	    << ", TIBSB = " << totalInputBufferSizeBytes
	    << ", #C = " << counter << std::endl;
#endif

  while (--counter >= 0)  {
    AudioBuffer* l_ioD_AB = &(ioData->mBuffers[counter]);
    l_ioD_AB->mNumberChannels = 1;
    l_ioD_AB->mData = (float*)(l_inputABL->mBuffers[counter].mData);
    l_ioD_AB->mDataByteSize = totalInputBufferSizeBytes;
  }

#if _OSX_AU_DEBUG_
  std::cerr << "cc2: Returning." << std::endl;
#endif

  return (noErr);
}

OSStatus
audio_osx_source::AUInputCallback (void* inRefCon,
				   AudioUnitRenderActionFlags* ioActionFlags,
				   const AudioTimeStamp* inTimeStamp,
				   UInt32 inBusNumber,
				   UInt32 inNumberFrames,
				   AudioBufferList* ioData)
{
  OSStatus err = noErr;
  audio_osx_source* This = static_cast<audio_osx_source*>(inRefCon);

  gruel::scoped_lock l (*This->d_internal);

#if _OSX_AU_DEBUG_
  std::cerr << "cb0: in#F = " << inNumberFrames
	    << ", inBN = " << inBusNumber
	    << ", SC = " << This->d_queueSampleCount << std::endl;
#endif

// Get the new audio data from the input device

  err = AudioUnitRender (This->d_InputAU,
			 ioActionFlags,
			 inTimeStamp,
			 1, //inBusNumber,
			 inNumberFrames,
			 This->d_InputBuffer);
  CheckErrorAndThrow (err, "AudioUnitRender",
		      "audio_osx_source::AUInputCallback");

  UInt32 AvailableInputFrames = inNumberFrames;
  This->d_n_AvailableInputFrames = inNumberFrames;

// get the number of actual output frames,
// either via converting the buffer or not

  UInt32 ActualOutputFrames;

  if (This->d_passThrough == true) {
    ActualOutputFrames = AvailableInputFrames;
  } else {
    UInt32 AvailableInputBytes = AvailableInputFrames * sizeof (float);
    UInt32 AvailableOutputBytes = AvailableInputBytes;
    UInt32 AvailableOutputFrames = AvailableOutputBytes / sizeof (float);
    UInt32 propertySize = sizeof (AvailableOutputBytes);
    err = AudioConverterGetProperty (This->d_AudioConverter,
		   kAudioConverterPropertyCalculateOutputBufferSize,
				     &propertySize,
				     &AvailableOutputBytes);
    CheckErrorAndThrow (err, "AudioConverterGetProperty CalculateOutputBufferSize", "audio_osx_source::audio_osx_source");

    AvailableOutputFrames = AvailableOutputBytes / sizeof (float);

#if 0
// when decimating too much, the output sounds warbly due to
// fluctuating # of output frames
// This should not be a surprise, but there's probably some
// clever programming that could lessed the effect ...
// like finding the "ideal" # of output frames, and keeping
// that number constant no matter the # of input frames
    UInt32 l_InputBytes = AvailableOutputBytes;
    propertySize = sizeof (AvailableOutputBytes);
    err = AudioConverterGetProperty (This->d_AudioConverter,
		     kAudioConverterPropertyCalculateInputBufferSize,
				     &propertySize,
				     &l_InputBytes);
    CheckErrorAndThrow (err, "AudioConverterGetProperty CalculateInputBufferSize", "audio_osx_source::audio_osx_source");

    if (l_InputBytes < AvailableInputBytes) {
// OK to zero pad the input a little
      AvailableOutputFrames += 1;
      AvailableOutputBytes = AvailableOutputFrames * sizeof (float);
    }
#endif

#if _OSX_AU_DEBUG_
    std::cerr << "cb1:  avail: #IF = " << AvailableInputFrames
	      << ", #OF = " << AvailableOutputFrames << std::endl;
#endif
    ActualOutputFrames = AvailableOutputFrames;

// convert the data to the correct rate
// on input, ActualOutputFrames is the number of available output frames

    err = AudioConverterFillComplexBuffer (This->d_AudioConverter,
	   (AudioConverterComplexInputDataProc)(This->ConverterCallback),
					   inRefCon,
					   &ActualOutputFrames,
					   This->d_OutputBuffer,
					   NULL);
    CheckErrorAndThrow (err, "AudioConverterFillComplexBuffer",
			"audio_osx_source::AUInputCallback");

// on output, ActualOutputFrames is the actual number of output frames

#if _OSX_AU_DEBUG_
    std::cerr << "cb2: actual: #IF = " << This->d_n_ActualInputFrames
	      << ", #OF = " << AvailableOutputFrames << std::endl;
    if (This->d_n_ActualInputFrames != AvailableInputFrames)
      std::cerr << "cb2.1: avail#IF = " << AvailableInputFrames
		<< ", actual#IF = " << This->d_n_ActualInputFrames << std::endl;
#endif
  }

// add the output frames to the buffers' queue, checking for overflow

  int l_counter = This->d_n_user_channels;
  int res = 0;

  while (--l_counter >= 0) {
    float* inBuffer = (float*) This->d_OutputBuffer->mBuffers[l_counter].mData;

#if _OSX_AU_DEBUG_
    std::cerr << "cb3: enqueuing audio data." << std::endl;
#endif

    int l_res = This->d_buffers[l_counter]->enqueue (inBuffer, ActualOutputFrames);
    if (l_res == -1)
      res = -1;
  }

  if (res == -1) {
// data coming in too fast
// drop oldest buffer
    fputs ("aO", stderr);
    fflush (stderr);
// set the local number of samples available to the max
    This->d_queueSampleCount = This->d_buffers[0]->buffer_length_items ();
  } else {
// keep up the local sample count
    This->d_queueSampleCount += ActualOutputFrames;
  }

#if _OSX_AU_DEBUG_
  std::cerr << "cb4: #OI = " << ActualOutputFrames
	    << ", #Cnt = " << This->d_queueSampleCount
	    << ", mSC = " << This->d_max_sample_count << std::endl;
#endif

// signal that data is available, if appropraite
  This->d_cond_data->notify_one ();

#if _OSX_AU_DEBUG_
  std::cerr << "cb5: returning." << std::endl;
#endif

  return (err);
}

void
audio_osx_source::SetDefaultInputDeviceAsCurrent
()
{
// set the default input device
  AudioDeviceID deviceID = 0;
  UInt32 dataSize = sizeof (AudioDeviceID);
  OSStatus err = noErr;

#ifndef GR_USE_OLD_AUDIO_UNIT
  AudioObjectPropertyAddress theAddress =
    { kAudioHardwarePropertyDefaultInputDevice,
      kAudioObjectPropertyScopeGlobal,
      kAudioObjectPropertyElementMaster };

  err = AudioObjectGetPropertyData
    (kAudioObjectSystemObject,
     &theAddress,
     0,
     NULL,
     &dataSize,
     &deviceID);
#else
  err = AudioHardwareGetProperty
    (kAudioHardwarePropertyDefaultInputDevice,
     &dataSize,
     &deviceID);
#endif

  CheckErrorAndThrow (err, "Get Audio Unit Property for Current Device",
		      "audio_osx_source::SetDefaultInputDeviceAsCurrent");

  err = AudioUnitSetProperty
    (d_InputAU,
     kAudioOutputUnitProperty_CurrentDevice,
     kAudioUnitScope_Global,
     0,
     &deviceID,
     sizeof (AudioDeviceID));

  CheckErrorAndThrow (err, "AudioUnitSetProperty Current Device",
		      "audio_osx_source::SetDefaultInputDeviceAsCurrent");
}

#if _OSX_DO_LISTENERS_
OSStatus
audio_osx_source::HardwareListener
(AudioHardwarePropertyID inPropertyID, 
 void *inClientData)
{
  OSStatus err = noErr;
  audio_osx_source* This = static_cast<audio_osx_source*>(inClientData);

  std::cerr << "a_o_s::HardwareListener" << std::endl;

// set the new default hardware input device for use by our AU

  This->SetDefaultInputDeviceAsCurrent ();

// reset the converter to tell it that the stream has changed

  err = AudioConverterReset (This->d_AudioConverter);
  CheckErrorAndThrow (err, "AudioConverterReset",
		      "audio_osx_source::UnitListener");

  return (err);
}

OSStatus
audio_osx_source::UnitListener
(void *inRefCon,
 AudioUnit ci,
 AudioUnitPropertyID inID,
 AudioUnitScope inScope,
 AudioUnitElement inElement)
{
  OSStatus err = noErr;
  audio_osx_source* This = static_cast<audio_osx_source*>(inRefCon);
  AudioStreamBasicDescription asbd;			

  std::cerr << "a_o_s::UnitListener" << std::endl;

// get the converter's input ASBD (for printing)

  UInt32 propertySize = sizeof (asbd);
  err = AudioConverterGetProperty (This->d_AudioConverter,
				   kAudioConverterCurrentInputStreamDescription,
				   &propertySize,
				   &asbd);
  CheckErrorAndThrow (err, "AudioConverterGetProperty "
		      "CurrentInputStreamDescription",
		      "audio_osx_source::UnitListener");

  std::cerr << "UnitListener: Input Source changed." << std::endl
	    << "Old Source Output Info:" << std::endl;
  PrintStreamDesc (&asbd);

// get the new input unit's output ASBD

  propertySize = sizeof (asbd);
  err = AudioUnitGetProperty (This->d_InputAU,
			      kAudioUnitProperty_StreamFormat,
			      kAudioUnitScope_Output, 1,
			      &asbd, &propertySize);
  CheckErrorAndThrow (err, "AudioUnitGetProperty StreamFormat",
		      "audio_osx_source::UnitListener");

  std::cerr << "New Source Output Info:" << std::endl;
  PrintStreamDesc (&asbd);

// set the converter's input ASBD to this

  err = AudioConverterSetProperty (This->d_AudioConverter,
				   kAudioConverterCurrentInputStreamDescription,
				   propertySize,
				   &asbd);
  CheckErrorAndThrow (err, "AudioConverterSetProperty "
		      "CurrentInputStreamDescription",
		      "audio_osx_source::UnitListener");

// reset the converter to tell it that the stream has changed

  err = AudioConverterReset (This->d_AudioConverter);
  CheckErrorAndThrow (err, "AudioConverterReset",
		      "audio_osx_source::UnitListener");

  return (err);
}
#endif

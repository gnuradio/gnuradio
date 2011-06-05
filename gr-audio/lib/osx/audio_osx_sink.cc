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
#include <audio_osx_sink.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <audio_osx.h>

#define _OSX_AU_DEBUG_ 0

AUDIO_REGISTER_SINK(REG_PRIO_HIGH, osx)(
    int sampling_rate, const std::string &device_name, bool ok_to_block
){
    return audio_sink::sptr(new audio_osx_sink(sampling_rate, device_name, ok_to_block));
}

audio_osx_sink::audio_osx_sink (int sample_rate,
				const std::string device_name,
				bool do_block,
				int channel_config,
				int max_sample_count)
  : gr_sync_block ("audio_osx_sink",
		   gr_make_io_signature (0, 0, 0),
		   gr_make_io_signature (0, 0, 0)),
    d_sample_rate (0.0), d_channel_config (0), d_n_channels (0),
    d_queueSampleCount (0), d_max_sample_count (0),
    d_do_block (do_block), d_internal (0), d_cond_data (0),
    d_OutputAU (0)
{
  if (sample_rate <= 0) {
    std::cerr << "Invalid Sample Rate: " << sample_rate << std::endl;
    throw std::invalid_argument ("audio_osx_sink::audio_osx_sink");
  } else
    d_sample_rate = (Float64) sample_rate;

  if (channel_config <= 0 & channel_config != -1) {
    std::cerr << "Invalid Channel Config: " << channel_config << std::endl;
    throw std::invalid_argument ("audio_osx_sink::audio_osx_sink");
  } else if (channel_config == -1) {
// no user input; try "device name" instead
    int l_n_channels = (int) strtol (device_name.data(), (char **)NULL, 10);
    if (l_n_channels == 0 & errno) {
      std::cerr << "Error Converting Device Name: " << errno << std::endl;
      throw std::invalid_argument ("audio_osx_sink::audio_osx_sink");
    }
    if (l_n_channels <= 0)
      channel_config = 2;
    else
      channel_config = l_n_channels;
  }

  d_n_channels = d_channel_config = channel_config;

// set the input signature

  set_input_signature (gr_make_io_signature (1, d_n_channels, sizeof (float)));

// check that the max # of samples to store is valid

  if (max_sample_count == -1)
    max_sample_count = sample_rate;
  else if (max_sample_count <= 0) {
    std::cerr << "Invalid Max Sample Count: " << max_sample_count << std::endl;
    throw std::invalid_argument ("audio_osx_sink::audio_osx_sink");
  }

  d_max_sample_count = max_sample_count;

// allocate the output circular buffer(s), one per channel

  d_buffers = (circular_buffer<float>**) new
    circular_buffer<float>* [d_n_channels];
  UInt32 n_alloc = (UInt32) ceil ((double) d_max_sample_count);
  for (UInt32 n = 0; n < d_n_channels; n++) {
    d_buffers[n] = new circular_buffer<float> (n_alloc, false, false);
  }

// create the default AudioUnit for output
  OSStatus err = noErr;

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
  if (comp == NULL) {
    std::cerr << "AudioComponentFindNext Error" << std::endl;
    throw std::runtime_error ("audio_osx_sink::audio_osx_sink");
  }
#else
  Component comp = FindNextComponent (NULL, &desc);
  if (comp == NULL) {
    std::cerr << "FindNextComponent Error" << std::endl;
    throw std::runtime_error ("audio_osx_sink::audio_osx_sink");
  }
#endif

#ifndef GR_USE_OLD_AUDIO_UNIT
  err = AudioComponentInstanceNew (comp, &d_OutputAU);
  CheckErrorAndThrow (err, "AudioComponentInstanceNew", "audio_osx_sink::audio_osx_sink");
#else
  err = OpenAComponent (comp, &d_OutputAU);
  CheckErrorAndThrow (err, "OpenAComponent", "audio_osx_sink::audio_osx_sink");
#endif

// Set up a callback function to generate output to the output unit

  AURenderCallbackStruct input;
  input.inputProc = (AURenderCallback)(audio_osx_sink::AUOutputCallback);
  input.inputProcRefCon = this;

  err = AudioUnitSetProperty (d_OutputAU,
			      kAudioUnitProperty_SetRenderCallback, 
			      kAudioUnitScope_Input,
			      0, 
			      &input, 
			      sizeof (input));
  CheckErrorAndThrow (err, "AudioUnitSetProperty Render Callback", "audio_osx_sink::audio_osx_sink");

// tell the Output Unit what format data will be supplied to it
// so that it handles any format conversions

  AudioStreamBasicDescription streamFormat;
  streamFormat.mSampleRate = (Float64)(sample_rate);
  streamFormat.mFormatID = kAudioFormatLinearPCM;
  streamFormat.mFormatFlags = (kLinearPCMFormatFlagIsFloat |
			       GR_PCM_ENDIANNESS |
			       kLinearPCMFormatFlagIsPacked |
			       kAudioFormatFlagIsNonInterleaved);
  streamFormat.mBytesPerPacket = 4;
  streamFormat.mFramesPerPacket = 1;
  streamFormat.mBytesPerFrame = 4;
  streamFormat.mChannelsPerFrame = d_n_channels;
  streamFormat.mBitsPerChannel = 32;

  err = AudioUnitSetProperty (d_OutputAU,
			      kAudioUnitProperty_StreamFormat,
			      kAudioUnitScope_Input,
			      0,
			      &streamFormat,
			      sizeof (AudioStreamBasicDescription));
  CheckErrorAndThrow (err, "AudioUnitSetProperty StreamFormat", "audio_osx_sink::audio_osx_sink");

// create the stuff to regulate I/O

  d_cond_data = new gruel::condition_variable ();
  if (d_cond_data == NULL)
    CheckErrorAndThrow (errno, "new condition (data)",
			"audio_osx_sink::audio_osx_sink");

  d_internal = new gruel::mutex ();
  if (d_internal == NULL)
    CheckErrorAndThrow (errno, "new mutex (internal)",
			"audio_osx_sink::audio_osx_sink");

// initialize the AU for output

  err = AudioUnitInitialize (d_OutputAU);
  CheckErrorAndThrow (err, "AudioUnitInitialize",
		      "audio_osx_sink::audio_osx_sink");

#if _OSX_AU_DEBUG_
  std::cerr << "audio_osx_sink Parameters:" << std::endl;
  std::cerr << "  Sample Rate is " << d_sample_rate << std::endl;
  std::cerr << "  Number of Channels is " << d_n_channels << std::endl;
  std::cerr << "  Max # samples to store per channel is " << d_max_sample_count << std::endl;
#endif
}

bool audio_osx_sink::IsRunning ()
{
  UInt32 AURunning = 0, AUSize = sizeof (UInt32);

  OSStatus err = AudioUnitGetProperty (d_OutputAU,
				       kAudioOutputUnitProperty_IsRunning,
				       kAudioUnitScope_Global,
				       0,
				       &AURunning,
				       &AUSize);
  CheckErrorAndThrow (err, "AudioUnitGetProperty IsRunning",
		      "audio_osx_sink::IsRunning");

  return (AURunning);
}

bool audio_osx_sink::start ()
{
  if (! IsRunning ()) {
    OSStatus err = AudioOutputUnitStart (d_OutputAU);
    CheckErrorAndThrow (err, "AudioOutputUnitStart", "audio_osx_sink::start");
  }

  return (true);
}

bool audio_osx_sink::stop ()
{
  if (IsRunning ()) {
    OSStatus err = AudioOutputUnitStop (d_OutputAU);
    CheckErrorAndThrow (err, "AudioOutputUnitStop", "audio_osx_sink::stop");

    for (UInt32 n = 0; n < d_n_channels; n++) {
      d_buffers[n]->abort ();
    }
  }

  return (true);
}

audio_osx_sink::~audio_osx_sink ()
{
// stop and close the AudioUnit
  stop ();
  AudioUnitUninitialize (d_OutputAU);
#ifndef GR_USE_OLD_AUDIO_UNIT
  AudioComponentInstanceDispose (d_OutputAU);
#else
  CloseComponent (d_OutputAU);
#endif

// empty and delete the queues
  for (UInt32 n = 0; n < d_n_channels; n++) {
    delete d_buffers[n];
    d_buffers[n] = 0;
  }
  delete [] d_buffers;
  d_buffers = 0;

// close and delete control stuff
  delete d_cond_data;
  d_cond_data = 0;
  delete d_internal;
  d_internal = 0;
}

int
audio_osx_sink::work (int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
{
  gruel::scoped_lock l (*d_internal);

  /* take the input data, copy it, and push it to the bottom of the queue
     mono input are pushed onto queue[0];
     stereo input are pushed onto queue[1].
     Start the AudioUnit if necessary. */

  UInt32 l_max_count;
  int diff_count = d_max_sample_count - noutput_items;
  if (diff_count < 0)
    l_max_count = 0;
  else
    l_max_count = (UInt32) diff_count;

#if 0
  if (l_max_count < d_queueItemLength->back()) {
//  allow 2 buffers at a time, regardless of length
    l_max_count = d_queueItemLength->back();
  }
#endif

#if _OSX_AU_DEBUG_
  std::cerr << "work1: qSC = " << d_queueSampleCount << ", lMC = "<< l_max_count
	    << ", dmSC = " << d_max_sample_count << ", nOI = " << noutput_items << std::endl;
#endif

  if (d_queueSampleCount > l_max_count) {
// data coming in too fast; do_block decides what to do
    if (d_do_block == true) {
// block until there is data to return
      while (d_queueSampleCount > l_max_count) {
// release control so-as to allow data to be retrieved;
// block until there is data to return
	d_cond_data->wait (l);
// the condition's 'notify' was called; acquire control
// to keep thread safe
      }
    }
  }
// not blocking case and overflow is handled by the circular buffer

// add the input frames to the buffers' queue, checking for overflow

  UInt32 l_counter;
  int res = 0;
  float* inBuffer = (float*) input_items[0];
  const UInt32 l_size = input_items.size();
  for (l_counter = 0; l_counter < l_size; l_counter++) {
    inBuffer = (float*) input_items[l_counter];
    int l_res = d_buffers[l_counter]->enqueue (inBuffer,
					       noutput_items);
    if (l_res == -1)
      res = -1;
  }
  while (l_counter < d_n_channels) {
// for extra channels, copy the last input's data
    int l_res = d_buffers[l_counter++]->enqueue (inBuffer,
						 noutput_items);
    if (l_res == -1)
      res = -1;
  }

  if (res == -1) {
// data coming in too fast
// drop oldest buffer
    fputs ("aO", stderr);
    fflush (stderr);
// set the local number of samples available to the max
    d_queueSampleCount = d_buffers[0]->buffer_length_items ();
  } else {
// keep up the local sample count
    d_queueSampleCount += noutput_items;
  }

#if _OSX_AU_DEBUG_
  std::cerr << "work2: #OI = " << noutput_items << ", #Cnt = "
	    << d_queueSampleCount << ", mSC = " << d_max_sample_count << std::endl;
#endif

  return (noutput_items);
}

OSStatus audio_osx_sink::AUOutputCallback
(void *inRefCon, 
 AudioUnitRenderActionFlags *ioActionFlags, 
 const AudioTimeStamp *inTimeStamp, 
 UInt32 inBusNumber, 
 UInt32 inNumberFrames, 
 AudioBufferList *ioData)
{
  audio_osx_sink* This = (audio_osx_sink*) inRefCon;
  OSStatus err = noErr;

  gruel::scoped_lock l (*This->d_internal);

#if _OSX_AU_DEBUG_
  std::cerr << "cb_in: SC = " << This->d_queueSampleCount
	    << ", in#F = " << inNumberFrames << std::endl;
#endif

  if (This->d_queueSampleCount < inNumberFrames) {
// not enough data to fill request
    err = -1;
  } else {
// enough data; remove data from our buffers into the AU's buffers
    int l_counter = This->d_n_channels;

    while (--l_counter >= 0) {
      size_t t_n_output_items = inNumberFrames;
      float* outBuffer = (float*) ioData->mBuffers[l_counter].mData;
      This->d_buffers[l_counter]->dequeue (outBuffer, &t_n_output_items);
      if (t_n_output_items != inNumberFrames) {
	throw std::runtime_error ("audio_osx_sink::AUOutputCallback(): "
				  "number of available items changing "
				  "unexpectedly.\n");
      }
    }

    This->d_queueSampleCount -= inNumberFrames;
  }

#if _OSX_AU_DEBUG_
  std::cerr << "cb_out: SC = " << This->d_queueSampleCount << std::endl;
#endif

// signal that data is available
  This->d_cond_data->notify_one ();

  return (err);
}

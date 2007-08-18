/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#define _USE_OMNI_THREADS_

#include <audio_osx_sink.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <audio_osx.h>

#define _OSX_AU_DEBUG_ 0

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
    fprintf (stderr, "Invalid Sample Rate: %d\n", sample_rate);
    throw std::invalid_argument ("audio_osx_sink::audio_osx_sink");
  } else
    d_sample_rate = (Float64) sample_rate;

  if (channel_config <= 0 & channel_config != -1) {
    fprintf (stderr, "Invalid Channel Config: %d\n", channel_config);
    throw std::invalid_argument ("audio_osx_sink::audio_osx_sink");
  } else if (channel_config == -1) {
// no user input; try "device name" instead
    int l_n_channels = (int) strtol (device_name.data(), (char **)NULL, 10);
    if (l_n_channels == 0 & errno) {
      fprintf (stderr, "Error Converting Device Name: %d\n", errno);
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
    fprintf (stderr, "Invalid Max Sample Count: %d\n", max_sample_count);
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
  ComponentDescription desc;
  desc.componentType = kAudioUnitType_Output;
  desc.componentSubType = kAudioUnitSubType_DefaultOutput;
  desc.componentManufacturer = kAudioUnitManufacturer_Apple;
  desc.componentFlags = 0;
  desc.componentFlagsMask = 0;

  Component comp = FindNextComponent (NULL, &desc);
  if (comp == NULL) {
    fprintf (stderr, "FindNextComponent Error\n");
    throw std::runtime_error ("audio_osx_sink::audio_osx_sink");
  }

  err = OpenAComponent (comp, &d_OutputAU);
  CheckErrorAndThrow (err, "OpenAComponent", "audio_osx_sink::audio_osx_sink");

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

  d_cond_data = new mld_condition ();
  if (d_cond_data == NULL)
    CheckErrorAndThrow (errno, "new mld_condition (data)",
			"audio_osx_source::audio_osx_source");
  d_internal = d_cond_data->mutex ();

// initialize the AU for output

  err = AudioUnitInitialize (d_OutputAU);
  CheckErrorAndThrow (err, "AudioUnitInitialize",
		      "audio_osx_sink::audio_osx_sink");

#if _OSX_AU_DEBUG_
  fprintf (stderr, "audio_osx_sink Parameters:\n");
  fprintf (stderr, "  Sample Rate is %g\n", d_sample_rate);
  fprintf (stderr, "  Number of Channels is %ld\n", d_n_channels);
  fprintf (stderr, "  Max # samples to store per channel is %ld",
	   d_max_sample_count);
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
  CloseComponent (d_OutputAU);

// empty and delete the queues
  for (UInt32 n = 0; n < d_n_channels; n++) {
    delete d_buffers[n];
    d_buffers[n] = 0;
  }
  delete [] d_buffers;
  d_buffers = 0;

// close and delete control stuff
  delete d_cond_data;
}

audio_osx_sink_sptr
audio_osx_make_sink (int sampling_freq,
		     const std::string dev,
		     bool do_block,
		     int channel_config,
		     int max_sample_count)
{
  return audio_osx_sink_sptr (new audio_osx_sink (sampling_freq,
						  dev,
						  do_block,
						  channel_config,
						  max_sample_count));
}

int
audio_osx_sink::work (int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
{
  d_internal->lock ();

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
  fprintf (stderr, "work1: qSC = %ld, lMC = %ld, dmSC = %ld, nOI = %d\n",
	   d_queueSampleCount, l_max_count, d_max_sample_count, noutput_items);
#endif

  if (d_queueSampleCount > l_max_count) {
// data coming in too fast; do_block decides what to do
    if (d_do_block == true) {
// block until there is data to return
      while (d_queueSampleCount > l_max_count) {
// release control so-as to allow data to be retrieved;
// block until there is data to return
	d_cond_data->wait ();
// the condition's signal() was called; acquire control
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
    fputs ("oX", stderr);
    fflush (stderr);
// set the local number of samples available to the max
    d_queueSampleCount = d_buffers[0]->buffer_length_items ();
  } else {
// keep up the local sample count
    d_queueSampleCount += noutput_items;
  }

#if _OSX_AU_DEBUG_
  fprintf (stderr, "work2: #OI = %4d, #Cnt = %4ld, mSC = %ld\n",
	   noutput_items, d_queueSampleCount, d_max_sample_count);
#endif

// release control to allow for other processing parts to run
  d_internal->unlock ();

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

  This->d_internal->lock ();

#if _OSX_AU_DEBUG_
  fprintf (stderr, "cb_in: SC = %4ld, in#F = %4ld\n",
	   This->d_queueSampleCount, inNumberFrames);
#endif

  if (This->d_queueSampleCount < inNumberFrames) {
// not enough data to fill request
    err = -1;
  } else {
// enough data; remove data from our buffers into the AU's buffers
    int l_counter = This->d_n_channels;

    while (--l_counter >= 0) {
      UInt32 t_n_output_items = inNumberFrames;
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
  fprintf (stderr, "cb_out: SC = %4ld\n", This->d_queueSampleCount);
#endif

// signal that data is available
  This->d_cond_data->signal ();

// release control to allow for other processing parts to run
  This->d_internal->unlock ();

  return (err);
}

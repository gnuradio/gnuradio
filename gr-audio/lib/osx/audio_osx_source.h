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

#ifndef INCLUDED_AUDIO_OSX_SOURCE_H
#define INCLUDED_AUDIO_OSX_SOURCE_H

#include <gr_audio_source.h>
#include <string>
#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>
#include <circular_buffer.h>

/*!
 * \brief audio source using OSX
 * \ingroup audio_blk
 *
 * Input signature is one or two streams of floats.
 * Samples must be in the range [-1,1].
 */

class audio_osx_source : public audio_source {

  Float64             d_deviceSampleRate, d_outputSampleRate;
  int                 d_channel_config;
  UInt32              d_inputBufferSizeFrames, d_inputBufferSizeBytes;
  UInt32              d_outputBufferSizeFrames, d_outputBufferSizeBytes;
  UInt32              d_deviceBufferSizeFrames, d_deviceBufferSizeBytes;
  UInt32              d_leadSizeFrames, d_leadSizeBytes;
  UInt32              d_trailSizeFrames, d_trailSizeBytes;
  UInt32              d_extraBufferSizeFrames, d_extraBufferSizeBytes;
  UInt32              d_queueSampleCount, d_max_sample_count;
  UInt32              d_n_AvailableInputFrames, d_n_ActualInputFrames;
  UInt32              d_n_user_channels, d_n_max_channels, d_n_deviceChannels;
  bool                d_do_block, d_passThrough, d_waiting_for_data;
  gruel::mutex*       d_internal;
  gruel::condition_variable* d_cond_data;
  circular_buffer<float>** d_buffers;

// AudioUnits and Such
  AudioUnit           d_InputAU;
  AudioBufferList*    d_InputBuffer;
  AudioBufferList*    d_OutputBuffer;
  AudioConverterRef   d_AudioConverter;

public:
  audio_osx_source (int sample_rate = 44100,
		    const std::string device_name = "",
		    bool do_block = true,
		    int channel_config = -1,
		    int max_sample_count = -1);

  ~audio_osx_source ();

  bool start ();
  bool stop ();
  bool IsRunning ();

  bool check_topology (int ninputs, int noutputs);

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

private:
  void SetDefaultInputDeviceAsCurrent ();

  void AllocAudioBufferList (AudioBufferList** t_ABL,
			     UInt32 n_channels,
			     UInt32 inputBufferSizeBytes);

  void FreeAudioBufferList (AudioBufferList** t_ABL);

  static OSStatus ConverterCallback (AudioConverterRef inAudioConverter,
				     UInt32* ioNumberDataPackets,
				     AudioBufferList* ioData,
				     AudioStreamPacketDescription** outASPD,
				     void* inUserData);

  static OSStatus AUInputCallback (void *inRefCon,
				   AudioUnitRenderActionFlags *ioActionFlags,
				   const AudioTimeStamp *inTimeStamp,
				   UInt32 inBusNumber,
				   UInt32 inNumberFrames,
				   AudioBufferList *ioData);
#if _OSX_DO_LISTENERS_
  static OSStatus UnitListener (void *inRefCon,
				AudioUnit ci,
				AudioUnitPropertyID inID,
				AudioUnitScope inScope,
				AudioUnitElement inElement);

  static OSStatus HardwareListener (AudioHardwarePropertyID inPropertyID, 
				    void *inClientData);
#endif
};

#endif /* INCLUDED_AUDIO_OSX_SOURCE_H */

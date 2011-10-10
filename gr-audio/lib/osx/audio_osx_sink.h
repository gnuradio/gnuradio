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

#ifndef INCLUDED_AUDIO_OSX_SINK_H
#define INCLUDED_AUDIO_OSX_SINK_H

#include <gr_audio_sink.h>
#include <string>
#include <list>
#include <AudioUnit/AudioUnit.h>
#include <circular_buffer.h>

/*!
 * \brief audio sink using OSX
 * \ingroup audio_blk
 *
 * input signature is one or two streams of floats.
 * Input samples must be in the range [-1,1].
 */

class audio_osx_sink : public audio_sink {

  Float64             d_sample_rate;
  int                 d_channel_config;
  UInt32              d_n_channels;
  UInt32              d_queueSampleCount, d_max_sample_count;
  bool                d_do_block;
  gruel::mutex*       d_internal;
  gruel::condition_variable* d_cond_data;
  circular_buffer<float>** d_buffers;

// AudioUnits and Such
  AudioUnit           d_OutputAU;

public:
  audio_osx_sink (int sample_rate = 44100,
		  const std::string device_name = "2",
		  bool do_block = true,
		  int channel_config = -1,
		  int max_sample_count = -1);

  ~audio_osx_sink ();

  bool IsRunning ();
  bool start ();
  bool stop ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

private:
  static OSStatus AUOutputCallback (void *inRefCon, 
				    AudioUnitRenderActionFlags *ioActionFlags, 
				    const AudioTimeStamp *inTimeStamp, 
				    UInt32 inBusNumber, 
				    UInt32 inNumberFrames, 
				    AudioBufferList *ioData);
};

#endif /* INCLUDED_AUDIO_OSX_SINK_H */

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

%feature("autodoc","1");

%include "exception.i"
%import "gnuradio.i"				// the common stuff

%{
#include "gnuradio_swig_bug_workaround.h"	// mandatory bug fix
#include "audio_osx_sink.h"
#include "audio_osx_source.h"
#include <stdexcept>
%}

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(audio_osx,sink)

audio_osx_sink_sptr
audio_osx_make_sink (int sample_rate = 44100,
		     const std::string device_name = "2",
		     bool do_block = TRUE,
		     int channel_config = -1,
		     int max_sample_count = -1
		     ) throw (std::runtime_error);

class audio_osx_sink : public gr_sync_block {
 protected:
  audio_osx_sink (int sample_rate = 44100,
		  const std::string device_name = "2",
		  bool do_block = TRUE,
		  int channel_config = -1,
		  int max_sample_count = -1);

 public:
  ~audio_osx_sink ();

  bool start ();
  bool stop ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(audio_osx,source)

audio_osx_source_sptr
audio_osx_make_source (int sample_rate = 44100,
		     const std::string device_name = "2",
		     bool do_block = TRUE,
		     int channel_config = -1,
		     int max_sample_count = -1
		     ) throw (std::runtime_error);

class audio_osx_source : public gr_sync_block {
 protected:
  audio_osx_source (int sample_rate = 44100,
		  const std::string device_name = "2",
		  bool do_block = TRUE,
		  int channel_config = -1,
		  int max_sample_count = -1);

 public:
  ~audio_osx_source ();
  
  bool start ();
  bool stop ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

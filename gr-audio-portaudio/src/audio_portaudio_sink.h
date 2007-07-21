/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
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
#ifndef INCLUDED_AUDIO_PORTAUDIO_SINK_H
#define INCLUDED_AUDIO_PORTAUDIO_SINK_H

#include <gr_sync_block.h>
#include <gr_buffer.h>
#include <omnithread.h>
#include <string>
#include <portaudio.h>
#include <stdexcept>
#include <gri_logger.h>

class audio_portaudio_sink;
typedef boost::shared_ptr<audio_portaudio_sink> audio_portaudio_sink_sptr;

/*!
 * \PORTAUDIO audio sink.
 * \param sampling_rate	sampling rate in Hz
 * \param dev PORTAUDIO device name, e.g., "pa:"
 * \param ok_to_block	true if it's ok for us to block
 */
audio_portaudio_sink_sptr
audio_portaudio_make_sink (int sampling_rate,
			   const std::string dev = "",
			   bool ok_to_block = true);

PaStreamCallback portaudio_sink_callback;


/*!
 * \ Audio sink using PORTAUDIO
 *
 * Input samples must be in the range [-1,1].
 */
class audio_portaudio_sink : public gr_sync_block {
  friend audio_portaudio_sink_sptr
  audio_portaudio_make_sink (int sampling_rate,
			     const std::string device_name,
			     bool ok_to_block);

  friend PaStreamCallback portaudio_sink_callback;


  unsigned int		d_sampling_rate;
  std::string		d_device_name;
  bool			d_ok_to_block;
  bool			d_verbose;

  unsigned int		d_portaudio_buffer_size_frames;	// number of frames in a portaudio buffer

  PaStream		*d_stream;
  PaStreamParameters	d_output_parameters;

  gr_buffer_sptr	d_writer;		// buffer used between work and callback
  gr_buffer_reader_sptr	d_reader;
  omni_semaphore	d_ringbuffer_ready;	// binary semaphore


  // random stats
  int			d_nunderuns;		// count of underruns
  gri_logger_sptr	d_log;			// handle to non-blocking logging instance

  void output_error_msg (const char *msg, int err);
  void bail (const char *msg, int err) throw (std::runtime_error);
  void create_ringbuffer();


 protected:
  audio_portaudio_sink (int sampling_rate, const std::string device_name,
			bool ok_to_block);

 public:
  ~audio_portaudio_sink ();
  
  bool check_topology (int ninputs, int noutputs);

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_AUDIO_PORTAUDIO_SINK_H */

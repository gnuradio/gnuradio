/* -*- c++ -*- */
/*
 * Copyright 2005,2006 Free Software Foundation, Inc.
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
#ifndef INCLUDED_AUDIO_JACK_SINK_H
#define INCLUDED_AUDIO_JACK_SINK_H

#include <gr_sync_block.h>
#include <string>
#include <jack/jack.h>
#include <jack/ringbuffer.h>
#include <stdexcept>

class audio_jack_sink;
typedef boost::shared_ptr<audio_jack_sink> audio_jack_sink_sptr;

/*!
 * \brief make an JACK audio sink.
 *
 * \param sampling_rate	sampling rate in Hz
 * \param dev JACK device name, e.g., "gr_sink"
 * \param ok_to_block
 */
audio_jack_sink_sptr
audio_jack_make_sink (int sampling_rate,
		      const std::string dev = "",
		      bool ok_to_block = true);

int jack_sink_process (jack_nframes_t nframes, void *arg);

/*!
 * \brief audio sink using JACK
 *
 * The sink has one input stream of floats.
 *
 * Input samples must be in the range [-1,1].
 */
class audio_jack_sink : public gr_sync_block {
  friend audio_jack_sink_sptr
  audio_jack_make_sink (int sampling_rate, const std::string device_name, bool ok_to_block);

  friend int jack_sink_process (jack_nframes_t nframes, void *arg);

  // typedef for pointer to class work method
  typedef int (audio_jack_sink::*work_t)(int noutput_items,
					 gr_vector_const_void_star &input_items,
					 gr_vector_void_star &output_items);

  unsigned int		d_sampling_rate;
  std::string		d_device_name;
  bool			d_ok_to_block;

  jack_client_t		*d_jack_client;
  jack_port_t		*d_jack_output_port;
  jack_ringbuffer_t	*d_ringbuffer;
  jack_nframes_t	d_jack_buffer_size;
  pthread_cond_t	d_ringbuffer_ready;
  pthread_mutex_t	d_jack_process_lock;

  // random stats
  int			d_nunderuns;		// count of underruns

  void output_error_msg (const char *msg, int err);
  void bail (const char *msg, int err) throw (std::runtime_error);


 protected:
  audio_jack_sink (int sampling_rate, const std::string device_name, bool ok_to_block);

 public:
  ~audio_jack_sink ();
  
  bool check_topology (int ninputs, int noutputs);

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_AUDIO_JACK_SINK_H */

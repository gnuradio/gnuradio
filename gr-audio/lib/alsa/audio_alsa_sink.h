/* -*- c++ -*- */
/*
 * Copyright 2004-2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_AUDIO_ALSA_SINK_H
#define INCLUDED_AUDIO_ALSA_SINK_H

// use new ALSA API
#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API

#include <gr_audio_sink.h>
#include <string>
#include <alsa/asoundlib.h>
#include <stdexcept>

/*!
 * \brief audio sink using ALSA
 * \ingroup audio_blk
 *
 * The sink has N input streams of floats, where N depends
 * on the hardware characteristics of the selected device.
 *
 * Input samples must be in the range [-1,1].
 */
class audio_alsa_sink : public audio_sink {
  // typedef for pointer to class work method
  typedef int (audio_alsa_sink::*work_t)(int noutput_items,
					 gr_vector_const_void_star &input_items,
					 gr_vector_void_star &output_items);

  unsigned int		d_sampling_rate;
  std::string		d_device_name;
  snd_pcm_t            *d_pcm_handle;
  snd_pcm_hw_params_t  *d_hw_params;
  snd_pcm_sw_params_t  *d_sw_params;
  snd_pcm_format_t	d_format;
  unsigned int		d_nperiods;
  unsigned int		d_period_time_us;	// microseconds
  snd_pcm_uframes_t	d_period_size;		// in frames
  unsigned int		d_buffer_size_bytes;	// sizeof of d_buffer
  char		       *d_buffer;
  work_t	        d_worker;		// the work method to use
  bool			d_special_case_mono_to_stereo;

  // random stats
  int			d_nunderuns;		// count of underruns
  int			d_nsuspends;		// count of suspends
  bool			d_ok_to_block;      // defaults to "true", controls blocking/non-block I/O

  void output_error_msg (const char *msg, int err);
  void bail (const char *msg, int err) throw (std::runtime_error);

public:
  audio_alsa_sink (int sampling_rate, const std::string device_name,
		   bool ok_to_block);

  ~audio_alsa_sink ();
  
  bool check_topology (int ninputs, int noutputs);

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);


protected:
  bool write_buffer (const void *buffer, unsigned nframes, unsigned sizeof_frame);

  int work_s16 (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);

  int work_s16_1x2 (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);

  int work_s32 (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);

  int work_s32_1x2 (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_AUDIO_ALSA_SINK_H */

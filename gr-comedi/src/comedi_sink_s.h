/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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
#ifndef INCLUDED_COMEDI_SINK_H
#define INCLUDED_COMEDI_SINK_H

#include <gr_sync_block.h>
#include <string>
#include <comedilib.h>
#include <stdexcept>

class comedi_sink_s;
typedef boost::shared_ptr<comedi_sink_s> comedi_sink_s_sptr;

/*!
 * \brief make a COMEDI sink.
 *
 * \param sampling_freq	sampling rate in Hz
 * \param dev COMEDI device name, e.g., "/dev/comedi0"
 */
comedi_sink_s_sptr
comedi_make_sink_s (int sampling_freq,
		      const std::string dev = "/dev/comedi0");

/*!
 * \brief sink using COMEDI
 *
 * The sink has one input stream of signed short integers.
 *
 * Input samples must be in the range [-32768,32767].
 */
class comedi_sink_s : public gr_sync_block {
  friend comedi_sink_s_sptr
  comedi_make_sink_s (int sampling_freq, const std::string dev);

  // typedef for pointer to class work method
  typedef int (comedi_sink_s::*work_t)(int noutput_items,
					 gr_vector_const_void_star &input_items,
					 gr_vector_void_star &output_items);

  unsigned int		d_sampling_freq;
  std::string		d_device_name;

  comedi_t		*d_dev;
  int			d_subdevice;
  int			d_n_chan;
  void			*d_map;
  int			d_buffer_size;
  unsigned		d_buf_front;
  unsigned		d_buf_back;

  // random stats
  int			d_nunderuns;		// count of underruns

  void output_error_msg (const char *msg, int err);
  void bail (const char *msg, int err) throw (std::runtime_error);

 protected:
  comedi_sink_s (int sampling_freq, const std::string device_name);

 public:
  ~comedi_sink_s ();
  
  bool check_topology (int ninputs, int noutputs);

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_COMEDI_SINK_H */

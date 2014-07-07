/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_COMEDI_SOURCE_S_IMPL_H
#define INCLUDED_COMEDI_SOURCE_S_IMPL_H

#include <gnuradio/comedi/source_s.h>
#include <string>
#include <comedilib.h>
#include <stdexcept>

namespace gr {
  namespace comedi {

    class source_s_impl : public source_s
    {
    private:
      // typedef for pointer to class work method
      typedef int(source_s::*work_t)(int noutput_items,
				     gr_vector_const_void_star &input_items,
				     gr_vector_void_star &output_items);

      unsigned int d_sampling_freq;
      std::string  d_device_name;

      comedi_t *d_dev;
      int       d_subdevice;
      int       d_n_chan;
      void     *d_map;
      int       d_buffer_size;
      unsigned  d_buf_front;
      unsigned  d_buf_back;

      // random stats
      int d_noverruns;		// count of overruns

      void output_error_msg(const char *msg, int err);
      void bail(const char *msg, int err) throw (std::runtime_error);

    public:
      source_s_impl(int sampling_freq, const std::string device_name);
      ~source_s_impl();

      bool check_topology(int ninputs, int noutputs);

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace comedi */
} /* namespace gr */

#endif /* INCLUDED_COMEDI_SOURCE_S_IMPL_H */

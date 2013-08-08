/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_TAGGED_FILE_SINK_IMPL_H
#define INCLUDED_GR_TAGGED_FILE_SINK_IMPL_H

#include <gnuradio/blocks/tagged_file_sink.h>
#include <cstdio>  // for FILE

namespace gr {
  namespace blocks {

    class tagged_file_sink_impl : public tagged_file_sink
    {
    private:
      enum {
	NOT_IN_BURST = 0,
	IN_BURST
      };

      size_t   d_itemsize;
      int      d_state;
      FILE    *d_handle;
      int      d_n;
      double   d_sample_rate;
      uint64_t d_last_N;
      double   d_timeval;

    public:
      tagged_file_sink_impl(size_t itemsize, double samp_rate);
      ~tagged_file_sink_impl();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_GR_TAGGED_FILE_SINK_IMPL_H */

/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_TAGGED_STREAM_MULTIPLY_LENGTH_IMPL_H
#define INCLUDED_TAGGED_STREAM_MULTIPLY_LENGTH_IMPL_H

#include <vector>
#include <gnuradio/blocks/tagged_stream_multiply_length.h>

namespace gr {
  namespace blocks {

    class tagged_stream_multiply_length_impl : public tagged_stream_multiply_length
    {
     private:
      pmt::pmt_t d_lengthtag;
      double d_scalar;
      size_t d_itemsize;

     public:
      tagged_stream_multiply_length_impl(size_t itemsize, const std::string &lengthtagname, double scalar);
      ~tagged_stream_multiply_length_impl();

      int general_work(int noutput_items,
	       gr_vector_int &ninput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

      void set_scalar(double scalar){
        d_scalar = scalar;
        }

      void set_scalar_pmt(pmt::pmt_t msg){
        set_scalar(pmt::to_double(msg));
        }

    };

  } // namespace blocks
} // namespace gr

#endif


/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2013 Free Software Foundation, Inc.
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "unpack_k_bits_bb_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <iostream>

namespace gr {
  namespace blocks {

    unpack_k_bits_bb::sptr
    unpack_k_bits_bb::make(unsigned k)
    {
      return gnuradio::get_initial_sptr
        (new unpack_k_bits_bb_impl(k));
    }

    unpack_k_bits_bb_impl::unpack_k_bits_bb_impl(unsigned k)
      : sync_interpolator("unpack_k_bits_bb",
                             io_signature::make(1, 1, sizeof(unsigned char)),
                             io_signature::make(1, 1, sizeof(unsigned char)),
                             k),
        d_k(k)
    {
      if(d_k == 0)
        throw std::out_of_range("interpolation must be > 0");
    }

    unpack_k_bits_bb_impl::~unpack_k_bits_bb_impl()
    {
    }

    int
    unpack_k_bits_bb_impl::work(int noutput_items,
                                gr_vector_const_void_star &input_items,
                                gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *)input_items[0];
      unsigned char *out = (unsigned char *)output_items[0];
  
      int n = 0;
      for(unsigned int i = 0; i < noutput_items/d_k; i++) {
        unsigned int t = in[i];
        for(int j = d_k - 1; j >= 0; j--)
          out[n++] = (t >> j) & 0x01;
      }

      assert(n == noutput_items);
      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */

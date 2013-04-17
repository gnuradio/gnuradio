/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "threshold_ff_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    threshold_ff::sptr
    threshold_ff::make(float lo, float hi, float initial_state)
    {
      return gnuradio::get_initial_sptr
        (new threshold_ff_impl(lo, hi, initial_state));
    }

    threshold_ff_impl::threshold_ff_impl(float lo, float hi,
                                         float initial_state)
      : sync_block("threshold_ff",
                      io_signature::make(1, 1, sizeof(float)),
                      io_signature::make(1, 1, sizeof(float))),
        d_lo(lo), d_hi(hi), d_last_state(initial_state)
    {
    }

    threshold_ff_impl::~threshold_ff_impl()
    {
    }

    int
    threshold_ff_impl::work(int noutput_items,
                            gr_vector_const_void_star &input_items,
                            gr_vector_void_star &output_items)
    {
      const float *in = (const float *)input_items[0];
      float *out = (float *)output_items[0];

      for(int i = 0; i < noutput_items; i++) {
        if(in[i] > d_hi) {
          out[i] = 1.0;
          d_last_state = 1.0;
        }
        else if(in[i] < d_lo) {
          out[i] = 0.0;
          d_last_state = 0.0;
        }
        else
          out[i] = d_last_state;
      }

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */

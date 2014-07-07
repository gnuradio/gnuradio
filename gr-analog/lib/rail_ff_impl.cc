/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
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

#include "rail_ff_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>

namespace gr {
  namespace analog {

    rail_ff::sptr
    rail_ff::make(float lo, float hi)
    {
      return gnuradio::get_initial_sptr
	(new rail_ff_impl(lo, hi));
    }

    rail_ff_impl::rail_ff_impl(float lo, float hi)
      : sync_block("rail_ff",
		      io_signature::make(1, 1, sizeof(float)),
		      io_signature::make(1, 1, sizeof(float))),
	d_lo(lo), d_hi(hi)
    {
      set_clipping();
    }

    rail_ff_impl::~rail_ff_impl()
    {
    }

    void
    rail_ff_impl::set_lo(float lo)
    {
      d_lo = lo;
      set_clipping();
    }

    void
    rail_ff_impl::set_hi(float hi)
    {
      d_hi = hi;
      set_clipping();
    }

    void
    rail_ff_impl::set_clipping()
    {
      d_mid = (d_lo + d_hi)/2;
      d_clip = d_hi - d_mid;
    }

    int
    rail_ff_impl::work(int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
    {
      const float *in  = (const float*)input_items[0];
      float *out = (float*)output_items[0];

      for(int i = 0; i < noutput_items; i++) {
	out[i] = d_mid + gr::branchless_clip(in[i] - d_mid, d_clip);
      }

      return noutput_items;
    }

  } /* namespace analog */
} /* namespace gr */

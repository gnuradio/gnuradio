/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#include "rotator_cc_impl.h"
#include <gnuradio/io_signature.h>

#include <cmath>

namespace gr {
  namespace blocks {

    rotator_cc::sptr
    rotator_cc::make(double phase_inc)
    {
      return gnuradio::get_initial_sptr
        (new rotator_cc_impl(phase_inc));
    }

    rotator_cc_impl::rotator_cc_impl(double phase_inc)
      : sync_block("rotator_cc",
                      io_signature::make(1, 1, sizeof(gr_complex)),
                      io_signature::make(1, 1, sizeof(gr_complex)))
    {
      set_phase_inc(phase_inc);
    }

    rotator_cc_impl::~rotator_cc_impl()
    {
    }

    void
    rotator_cc_impl::set_phase_inc(double phase_inc)
    {
      d_r.set_phase_incr( exp(gr_complex(0, phase_inc)) );
    }

    int
    rotator_cc_impl::work(int noutput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *)input_items[0];
      gr_complex *out = (gr_complex *)output_items[0];

#if 0
      for (int i=0; i<noutput_items; i++)
      	out[i] = d_r.rotate(in[i]);
#else
      d_r.rotateN(out, in, noutput_items);
#endif

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */

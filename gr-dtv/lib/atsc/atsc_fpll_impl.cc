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

#include "atsc_fpll_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <gnuradio/sincos.h>

namespace gr {
  namespace dtv {

    atsc_fpll::sptr
    atsc_fpll::make(float rate)
    {
      return gnuradio::get_initial_sptr
	(new atsc_fpll_impl(rate));
    }

    atsc_fpll_impl::atsc_fpll_impl(float rate)
      : sync_block("dtv_atsc_fpll",
                   io_signature::make(1, 1, sizeof(gr_complex)),
                   io_signature::make(1, 1, sizeof(float)))
    {
      d_afc.set_taps(1.0-exp(-1.0/rate/5e-6));
      d_nco.set_freq((-3e6 + 0.309e6)/rate*2*M_PI);
      d_nco.set_phase(0.0);
    }

    atsc_fpll_impl::~atsc_fpll_impl()
    {
    }

    int
    atsc_fpll_impl::work(int noutput_items,
                         gr_vector_const_void_star &input_items,
                         gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *)input_items[0];
      float *out = (float *)output_items[0];

      for (int k = 0; k < noutput_items; k++) {
        float a_cos, a_sin;

        d_nco.step();                  // increment phase
        d_nco.sincos(&a_sin, &a_cos);  // compute cos and sin

        // Mix out carrier and output I-only signal
        gr_complex result = in[k]*gr_complex(a_sin, a_cos);
        out[k] = result.real();

        // Update phase/freq error
        gr_complex filtered = d_afc.filter(result);
        float x = gr::fast_atan2f(filtered.imag(), filtered.real());

        // avoid slamming filter with big transitions
        static const float limit = M_PI/2.0;
        if (x > limit)
          x = limit;
        else if (x < -limit)
          x = -limit;

        static const float alpha = 0.01;
        static const float beta = alpha*alpha/4.0;
        d_nco.adjust_phase(alpha*x);
        d_nco.adjust_freq(beta*x);
      }

      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
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

#include "hrpt_pll_cf_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <gnuradio/sincos.h>

namespace gr {
  namespace noaa {

#define M_TWOPI (2*M_PI)

    float
    phase_wrap(float phase)
    {
      while (phase < -M_PI)
	phase += M_TWOPI;
      while (phase > M_PI)
	phase -= M_TWOPI;

      return phase;
    }

    hrpt_pll_cf::sptr
    hrpt_pll_cf::make(float alpha, float beta, float max_offset)
    {
      return gnuradio::get_initial_sptr
	(new hrpt_pll_cf_impl(alpha, beta, max_offset));
    }

    hrpt_pll_cf_impl::hrpt_pll_cf_impl(float alpha, float beta, float max_offset)
      : sync_block("noaa_hrpt_pll_cf",
		      io_signature::make(1, 1, sizeof(gr_complex)),
		      io_signature::make(1, 1, sizeof(float))),
	d_alpha(alpha), d_beta(beta), d_max_offset(max_offset),
	d_phase(0.0), d_freq(0.0)
    {
    }

    hrpt_pll_cf_impl::~hrpt_pll_cf_impl()
    {
    }

    int
    hrpt_pll_cf_impl::work(int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *)input_items[0];
      float *out = (float *)output_items[0];

      for(int i = 0; i < noutput_items; i++) {

	// Generate and mix out carrier
	float re, im;
	gr::sincosf(d_phase, &im, &re);
	out[i] = (in[i]*gr_complex(re, -im)).imag();

	// Adjust PLL phase/frequency
	float error = phase_wrap(gr::fast_atan2f(in[i].imag(), in[i].real()) - d_phase);
	d_freq  = gr::branchless_clip(d_freq + error*d_beta, d_max_offset);
	d_phase = phase_wrap(d_phase + error*d_alpha + d_freq);
      }

      return noutput_items;
    }

  } /* namespace noaa */
} /* namespace gr */

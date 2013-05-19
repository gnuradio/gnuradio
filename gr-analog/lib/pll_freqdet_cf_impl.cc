/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2011 Free Software Foundation, Inc.
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

#include "pll_freqdet_cf_impl.h"
#include <gnuradio/io_signature.h>
#include <math.h>
#include <gnuradio/math.h>

namespace gr {
  namespace analog {

#ifndef M_TWOPI
#define M_TWOPI (2.0f*M_PI)
#endif

    pll_freqdet_cf::sptr
    pll_freqdet_cf::make(float loop_bw, float max_freq, float min_freq)
    {
      return gnuradio::get_initial_sptr
	(new pll_freqdet_cf_impl(loop_bw, max_freq, min_freq));
    }

    pll_freqdet_cf_impl::pll_freqdet_cf_impl(float loop_bw, float max_freq, float min_freq)
      : sync_block("pll_freqdet_cf",
		      io_signature::make(1, 1, sizeof(gr_complex)),
		      io_signature::make(1, 1, sizeof(float))),
	blocks::control_loop(loop_bw, max_freq, min_freq)
    {
    }

    pll_freqdet_cf_impl::~pll_freqdet_cf_impl()
    {
    }

    float
    pll_freqdet_cf_impl::mod_2pi(float in)
    {
      if(in > M_PI)
	return in - M_TWOPI;
      else if(in < -M_PI)
	return in + M_TWOPI;
      else
	return in;
    }

    float
    pll_freqdet_cf_impl::phase_detector(gr_complex sample, float ref_phase)
    {
      float sample_phase;
      sample_phase = gr::fast_atan2f(sample.imag(), sample.real());
      return mod_2pi(sample_phase - ref_phase);
    }

    int
    pll_freqdet_cf_impl::work(int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
    {
      const gr_complex *iptr = (gr_complex*)input_items[0];
      float *optr = (float*)output_items[0];

      float error;
      int size = noutput_items;

      while(size-- > 0) {
	*optr++ = d_freq;

	error = phase_detector(*iptr++, d_phase);

	advance_loop(error);
	phase_wrap();
	frequency_limit();
      }
      return noutput_items;
    }

    void
    pll_freqdet_cf_impl::set_loop_bandwidth(float bw)
    {
      blocks::control_loop::set_loop_bandwidth(bw);
    }

    void
    pll_freqdet_cf_impl::set_damping_factor(float df)
    {
      blocks::control_loop::set_damping_factor(df);
    }

    void
    pll_freqdet_cf_impl::set_alpha(float alpha)
    {
      blocks::control_loop::set_alpha(alpha);
    }

    void
    pll_freqdet_cf_impl::set_beta(float beta)
    {
      blocks::control_loop::set_beta(beta);
    }

    void
    pll_freqdet_cf_impl::set_frequency(float freq)
    {
      blocks::control_loop::set_frequency(freq);
    }

    void
    pll_freqdet_cf_impl::set_phase(float phase)
    {
      blocks::control_loop::set_phase(phase);
    }

    void
    pll_freqdet_cf_impl::set_min_freq(float freq)
    {
      blocks::control_loop::set_min_freq(freq);
    }

    void
    pll_freqdet_cf_impl::set_max_freq(float freq)
    {
      blocks::control_loop::set_max_freq(freq);
    }


    float
    pll_freqdet_cf_impl::get_loop_bandwidth() const
    {
      return blocks::control_loop::get_loop_bandwidth();
    }

    float
    pll_freqdet_cf_impl::get_damping_factor() const
    {
      return blocks::control_loop::get_damping_factor();
    }

    float
    pll_freqdet_cf_impl::get_alpha() const
    {
      return blocks::control_loop::get_alpha();
    }

    float
    pll_freqdet_cf_impl::get_beta() const
    {
      return blocks::control_loop::get_beta();
    }

    float
    pll_freqdet_cf_impl::get_frequency() const
    {
      return blocks::control_loop::get_frequency();
    }

    float
    pll_freqdet_cf_impl::get_phase() const
    {
      return blocks::control_loop::get_phase();
    }

    float
    pll_freqdet_cf_impl::get_min_freq() const
    {
      return blocks::control_loop::get_min_freq();
    }

    float
    pll_freqdet_cf_impl::get_max_freq() const
    {
      return blocks::control_loop::get_max_freq();
    }

  } /* namespace analog */
} /* namespace gr */

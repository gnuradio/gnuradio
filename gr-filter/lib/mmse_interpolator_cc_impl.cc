/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2010,2012 Free Software Foundation, Inc.
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

#include <gnuradio/io_signature.h>
#include "mmse_interpolator_cc_impl.h"
#include <stdexcept>

namespace gr {
  namespace filter {

    mmse_interpolator_cc::sptr
    mmse_interpolator_cc::make(float phase_shift, float interp_ratio)
    {
      return gnuradio::get_initial_sptr(
          new mmse_interpolator_cc_impl(phase_shift, interp_ratio));
    }

    mmse_interpolator_cc_impl::mmse_interpolator_cc_impl
                                     (float phase_shift, float interp_ratio)
      : block("mmse_interpolator_cc",
		 io_signature::make(1, 1, sizeof(gr_complex)),
		 io_signature::make(1, 1, sizeof(gr_complex))),
	d_mu (phase_shift), d_mu_inc (interp_ratio),
	d_interp(new mmse_fir_interpolator_cc())
    {
      GR_LOG_WARN(d_logger, "mmse_interpolator is deprecated. Please use mmse_resampler instead.");

      if(interp_ratio <=  0)
	throw std::out_of_range("interpolation ratio must be > 0");
      if(phase_shift <  0  || phase_shift > 1)
	throw std::out_of_range("phase shift ratio must be > 0 and < 1");

      set_inverse_relative_rate(d_mu_inc);
    }

    mmse_interpolator_cc_impl::~mmse_interpolator_cc_impl()
    {
      delete d_interp;
    }

    void
    mmse_interpolator_cc_impl::forecast(int noutput_items,
				     gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      for(unsigned i=0; i < ninputs; i++) {
	ninput_items_required[i] =
	  (int)ceil((noutput_items * d_mu_inc) + d_interp->ntaps());
      }
    }

    int
    mmse_interpolator_cc_impl::general_work(int noutput_items,
				     gr_vector_int &ninput_items,
				     gr_vector_const_void_star &input_items,
				     gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];

      int ii = 0; // input index
      int oo = 0; // output index

      while(oo < noutput_items) {
        out[oo++] = d_interp->interpolate(&in[ii], static_cast<float>(d_mu));

	double s = d_mu + d_mu_inc;
	double f = floor(s);
	int incr = (int)f;
	d_mu = s - f;
	ii += incr;
      }

      consume_each(ii);

      return noutput_items;
    }

    float
    mmse_interpolator_cc_impl::mu() const
    {
      return static_cast<float>(d_mu);
    }

    float
    mmse_interpolator_cc_impl::interp_ratio() const
    {
      return static_cast<float>(d_mu_inc);
    }

    void
    mmse_interpolator_cc_impl::set_mu(float mu)
    {
      d_mu = static_cast<double>(mu);
    }

    void
    mmse_interpolator_cc_impl::set_interp_ratio(float interp_ratio)
    {
      d_mu_inc = static_cast<double>(interp_ratio);
    }

  } /* namespace filter */
} /* namespace gr */

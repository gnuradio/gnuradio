/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2010,2012-2013 Free Software Foundation, Inc.
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
#include "fractional_resampler_cc_impl.h"
#include <stdexcept>

namespace gr {
  namespace filter {

    fractional_resampler_cc::sptr
    fractional_resampler_cc::make(float phase_shift, float resamp_ratio)
    {
      return gnuradio::get_initial_sptr
        (new fractional_resampler_cc_impl(phase_shift, resamp_ratio));
    }

    fractional_resampler_cc_impl::fractional_resampler_cc_impl
                                     (float phase_shift, float resamp_ratio)
      : block("fractional_resampler_cc",
              io_signature::make2(1, 2, sizeof(gr_complex), sizeof(float)),
              io_signature::make(1, 1, sizeof(gr_complex))),
	d_mu(phase_shift), d_mu_inc(resamp_ratio),
	d_resamp(new mmse_fir_interpolator_cc())
    {
      if(resamp_ratio <=  0)
	throw std::out_of_range("resampling ratio must be > 0");
      if(phase_shift <  0  || phase_shift > 1)
	throw std::out_of_range("phase shift ratio must be > 0 and < 1");

      set_relative_rate(1.0 / resamp_ratio);
      message_port_register_in(pmt::intern("msg_in"));
      set_msg_handler(pmt::intern("msg_in"), boost::bind(
              &fractional_resampler_cc_impl::handle_msg, this, _1));
    }

    fractional_resampler_cc_impl::~fractional_resampler_cc_impl()
    {
      delete d_resamp;
    }

    void
    fractional_resampler_cc_impl::handle_msg(pmt::pmt_t msg) {
      if(!pmt::is_dict(msg))
        return;
      // set resamp_ratio or mu by message dict
      if(pmt::dict_has_key(msg, pmt::intern("resamp_ratio"))) {
        set_resamp_ratio(
          pmt::to_float(
            pmt::dict_ref(msg, pmt::intern("resamp_ratio"),
            pmt::from_float(resamp_ratio()))
          )
        );
      }
      if(pmt::dict_has_key(msg, pmt::intern("mu"))) {
        set_mu(
          pmt::to_float(
            pmt::dict_ref(msg, pmt::intern("mu"),
            pmt::from_float(mu()))
          )
        );
      }
    }

    void
    fractional_resampler_cc_impl::forecast(int noutput_items,
                                           gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      for(unsigned i=0; i < ninputs; i++) {
	ninput_items_required[i] =
	  (int)ceil((noutput_items * d_mu_inc) + d_resamp->ntaps());
      }
    }

    int
    fractional_resampler_cc_impl::general_work(int noutput_items,
                                               gr_vector_int &ninput_items,
                                               gr_vector_const_void_star &input_items,
                                               gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];

      int ii = 0; // input index
      int oo = 0; // output index

      if(ninput_items.size() == 1) {
        while(oo < noutput_items) {
          out[oo++] = d_resamp->interpolate(&in[ii], d_mu);

          double s = d_mu + d_mu_inc;
          double f = floor(s);
          int incr = (int)f;
          d_mu = s - f;
          ii += incr;
        }

        consume_each(ii);
        return noutput_items;
      }

      else {
        const float *rr = (const float*)input_items[1];
        while(oo < noutput_items) {
          out[oo++] = d_resamp->interpolate(&in[ii], d_mu);
          d_mu_inc = rr[ii];

          double s = d_mu + d_mu_inc;
          double f = floor(s);
          int incr = (int)f;
          d_mu = s - f;
          ii += incr;
        }

        set_relative_rate(1.0 / d_mu_inc);
        consume_each(ii);
        return noutput_items;
      }
    }

    float
    fractional_resampler_cc_impl::mu() const
    {
      return d_mu;
    }

    float
    fractional_resampler_cc_impl::resamp_ratio() const
    {
      return d_mu_inc;
    }

    void
    fractional_resampler_cc_impl::set_mu(float mu)
    {
      d_mu = mu;
    }

    void
    fractional_resampler_cc_impl::set_resamp_ratio(float resamp_ratio)
    {
      d_mu_inc = resamp_ratio;
    }

  } /* namespace filter */
} /* namespace gr */

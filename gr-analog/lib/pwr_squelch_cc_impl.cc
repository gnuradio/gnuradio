/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2010,2012 Free Software Foundation, Inc.
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

#include "pwr_squelch_cc_impl.h"

namespace gr {
  namespace analog {

    pwr_squelch_cc::sptr
    pwr_squelch_cc::make(double threshold, double alpha,
			 int ramp, bool gate)
    {
      return gnuradio::get_initial_sptr
	(new pwr_squelch_cc_impl(threshold, alpha, ramp, gate));
    }

    pwr_squelch_cc_impl::pwr_squelch_cc_impl(double threshold, double alpha,
					     int ramp, bool gate)
      :	block("pwr_squelch_cc",
		 io_signature::make(1, 1, sizeof(gr_complex)),
		 io_signature::make(1, 1, sizeof(gr_complex))),
	squelch_base_cc_impl("pwr_squelch_cc", ramp, gate),
	d_pwr(0), d_iir(alpha)
    {
      set_threshold(threshold);
    }

    pwr_squelch_cc_impl::~pwr_squelch_cc_impl()
    {
    }

    std::vector<float>
    pwr_squelch_cc_impl::squelch_range() const
    {
      std::vector<float> r(3);
      r[0] = -50.0;			// min	FIXME
      r[1] = +50.0;			// max	FIXME
      r[2] = (r[1] - r[0]) / 100;	// step size

      return r;
    }

    void
    pwr_squelch_cc_impl::update_state(const gr_complex &in)
    {
      d_pwr = d_iir.filter(in.real()*in.real()+in.imag()*in.imag());
    }

    void
    pwr_squelch_cc_impl::set_threshold(double db)
    {
        gr::thread::scoped_lock l(d_setlock);
        d_threshold = std::pow(10.0, db/10);
    }

    void
    pwr_squelch_cc_impl::set_alpha(double alpha)
    {
        gr::thread::scoped_lock l(d_setlock);
        d_iir.set_taps(alpha);
    }

  } /* namespace analog */
} /* namespace gr */

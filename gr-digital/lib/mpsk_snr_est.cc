/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <gnuradio/digital/mpsk_snr_est.h>
#include <stdexcept>
#include <cstdio>

namespace gr {
  namespace digital {

    mpsk_snr_est::mpsk_snr_est(double alpha)
    {
      d_signal = 0;
      d_noise = 0;
      set_alpha(alpha);
    }

    mpsk_snr_est::~mpsk_snr_est()
    {}

    void
    mpsk_snr_est::set_alpha(double alpha)
    {
      d_alpha = alpha;
      d_beta = 1.0-alpha;
    }

    double
    mpsk_snr_est::alpha() const
    {
      return d_alpha;
    }

    int
    mpsk_snr_est::update(int noutput_items,
			 const gr_complex *input)
    {
      throw std::runtime_error("mpsk_snr_est: Unimplemented");
    }

    double
    mpsk_snr_est::snr()
    {
      throw std::runtime_error("mpsk_snr_est: Unimplemented");
    }

    double
    mpsk_snr_est::signal()
    {
      return 10.0*log10(d_signal);
    }

    double
    mpsk_snr_est::noise()
    {
      return 10.0*log10(d_noise);
    }


    /*****************************************************************/


    mpsk_snr_est_simple::mpsk_snr_est_simple(double alpha) :
      mpsk_snr_est(alpha)
    {
      d_y1 = 0;
      d_y2 = 0;
      d_counter = 0;
    }

    int
    mpsk_snr_est_simple::update(int noutput_items,
				const gr_complex *input)
    {
      int i = 0;
      if(d_counter == 0) {
        d_y1 = abs(input[0]);
        d_y2 = 0;
        d_counter++;
        i++;
      }

      for(; i < noutput_items; i++) {
        double x = abs(input[i]);
        double m = d_y1 + (x - d_y1)/d_counter;
        d_y2 = d_y2 + (x - d_y1)*(x - m);
        d_y1 = m;
        d_counter++;
      }
      return noutput_items;
    }

    double
    mpsk_snr_est_simple::snr()
    {
      d_signal = d_y1;
      d_noise = 2 * d_y2 / (d_counter - 1);

      return 10.0*log10(d_signal/d_noise);
    }


    /*****************************************************************/


    mpsk_snr_est_skew::mpsk_snr_est_skew(double alpha) :
      mpsk_snr_est(alpha)
    {
      d_y1 = 0;
      d_y2 = 0;
      d_y3 = 0;
      d_counter = 1;
    }

    int
    mpsk_snr_est_skew::update(int noutput_items,
			      const gr_complex *input)
    {
      for(int i = 0; i < noutput_items; i++) {
	double y1 = abs(input[i]);
	d_y1 = d_alpha*y1 + d_beta*d_y1;

        double y2 = real(input[i]*input[i]);
	d_y2 = d_alpha*y2 + d_beta*d_y2;

	// online algorithm for calculating skewness
	// See:
	// http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Higher-order_statistics
        double d = abs(input[i]) - d_y1;
        double d_i = d / (i+1);
        d_y3 = (d*d_i*i)*d_i*(i-1) - 3.0*d_i*d_y2;
      }
      return noutput_items;
    }

    double
    mpsk_snr_est_skew::snr()
    {
      double y3 = d_y3*d_y3 / (d_y2*d_y2*d_y2);
      d_signal = d_y1*d_y1;
      double x = d_signal - d_y2;
      d_noise = 2*(x + y3*d_signal);

      return 10.0*log10(d_signal/d_noise);
    }


    /*****************************************************************/


    mpsk_snr_est_m2m4::mpsk_snr_est_m2m4(double alpha) :
      mpsk_snr_est(alpha)
    {
      d_y1 = 0;
      d_y2 = 0;
    }

    int
    mpsk_snr_est_m2m4::update(int noutput_items,
			      const gr_complex *input)
    {
      for(int i = 0; i < noutput_items; i++) {
	double y1 = abs(input[i])*abs(input[i]);
	d_y1 = d_alpha*y1 + d_beta*d_y1;

	double y2 = abs(input[i])*abs(input[i])*abs(input[i])*abs(input[i]);
	d_y2 = d_alpha*y2 + d_beta*d_y2;
      }
      return noutput_items;
    }

    double
    mpsk_snr_est_m2m4::snr()
    {
      double y1_2 = d_y1*d_y1;
      d_signal = sqrt(2*y1_2 - d_y2);
      d_noise = d_y1 - sqrt(2*y1_2 - d_y2);
      return 10.0*log10(d_signal / d_noise);
    }


    /*****************************************************************/


    snr_est_m2m4::snr_est_m2m4(double alpha, double ka, double kw) :
      mpsk_snr_est(alpha)
    {
      d_y1 = 0;
      d_y2 = 0;
      d_ka = ka;
      d_kw = kw;
    }

    int
    snr_est_m2m4::update(int noutput_items,
			 const gr_complex *input)
    {
      for(int i = 0; i < noutput_items; i++) {
	double y1 = abs(input[i])*abs(input[i]);
	d_y1 = d_alpha*y1 + d_beta*d_y1;

	double y2 = abs(input[i])*abs(input[i])*abs(input[i])*abs(input[i]);
	d_y2 = d_alpha*y2 + d_beta*d_y2;
      }
      return noutput_items;
    }

    double
    snr_est_m2m4::snr()
    {
      double M2 = d_y1;
      double M4 = d_y2;
      d_signal = M2*(d_kw - 2) +
	sqrt((4.0-d_ka*d_kw)*M2*M2 + M4*(d_ka+d_kw-4.0)) /
	(d_ka + d_kw - 4.0);
      d_noise = M2 - d_signal;

      return 10.0*log10(d_signal / d_noise);
    }


    /*****************************************************************/


    mpsk_snr_est_svr::mpsk_snr_est_svr(double alpha) :
      mpsk_snr_est(alpha)
    {
      d_y1 = 0;
      d_y2 = 0;
    }

    int
    mpsk_snr_est_svr::update(int noutput_items,
			     const gr_complex *input)
    {
      for(int i = 0; i < noutput_items; i++) {
	double x = abs(input[i+1]);
	double x1 = abs(input[i]);
	double y1 = (x*x)*(x1*x1);
	d_y1 = d_alpha*y1 + d_beta*d_y1;

	double y2 = x*x*x*x;
	d_y2 = d_alpha*y2 + d_beta*d_y2;
      }
      return noutput_items;
    }

    double
    mpsk_snr_est_svr::snr()
    {
      double x = d_y1 / (d_y2 - d_y1);
      return 10.0*log10(x - 1 + sqrt(x*(x-1)));
    }

  } /* namespace digital */
} /* namespace gr */

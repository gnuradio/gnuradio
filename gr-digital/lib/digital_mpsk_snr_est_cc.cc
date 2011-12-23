/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#include <digital_mpsk_snr_est_cc.h>
#include <gr_io_signature.h>
#include <cstdio>

digital_mpsk_snr_est_cc_sptr
digital_make_mpsk_snr_est_cc(snr_est_type_t type,
			     double alpha)
{
  return gnuradio::get_initial_sptr(new digital_mpsk_snr_est_cc(type, alpha));
}

digital_mpsk_snr_est_cc::digital_mpsk_snr_est_cc(snr_est_type_t type,
						 double alpha)
  : gr_sync_block ("mpsk_snr_est_cc",
		   gr_make_io_signature(1, 1, sizeof(gr_complex)),
		   gr_make_io_signature(1, 1, sizeof(gr_complex))),
    d_type(type), d_y1(0), d_y2(0), d_y3(0), d_y4(0)
{
  set_type(type);
  set_alpha(alpha);
  set_history(2);
}

digital_mpsk_snr_est_cc::~digital_mpsk_snr_est_cc()
{
}

int
digital_mpsk_snr_est_cc::est_simple(int noutput_items,
				    gr_vector_const_void_star &input_items,
				    gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];

  for (int i = 0; i < noutput_items; i++){
    double y1 = abs(in[i]);
    d_y1 = d_alpha*y1 + d_beta*d_y1;

    double y2 = real(in[i]*in[i]);
    d_y2 = d_alpha*y2 + d_beta*d_y2;
  }
  return noutput_items;
}

double
digital_mpsk_snr_est_cc::est_simple_snr()
{
  double y1_2 = d_y1*d_y1;
  double y3 = y1_2 - d_y2 + 1e-20;
  return 10.0*log10(y1_2/y3);
}

int
digital_mpsk_snr_est_cc::est_skew(int noutput_items,
				  gr_vector_const_void_star &input_items,
				  gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];

  for (int i = 0; i < noutput_items; i++){
    double y1 = abs(in[i]);
    d_y1 = d_alpha*y1 + d_beta*d_y1;

    double y2 = real(in[i]*in[i]);
    d_y2 = d_alpha*y2 + d_beta*d_y2;

    // online algorithm for calculating skewness
    // See:
    // http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Higher-order_statistics
    double d = abs(in[i]) - d_y1;
    double d_i = d / (i+1);
    double y3 = (d*d_i*i)*d_i*(i-1) - 3.0*d_i*d_y2;
    d_y3 = d_alpha*y3 + d_beta*d_y3;
  }
  return noutput_items;
}

double
digital_mpsk_snr_est_cc::est_skew_snr()
{
  double y3 = d_y3*d_y3 / (d_y2*d_y2*d_y2);
  double y1_2 = d_y1*d_y1;
  double x = y1_2 - d_y2;
  return 10.0*log10(y1_2 / (x + y3*y1_2));
}

int
digital_mpsk_snr_est_cc::est_m2m4(int noutput_items,
				  gr_vector_const_void_star &input_items,
				  gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];

  for (int i = 0; i < noutput_items; i++){
    double y1 = abs(in[i])*abs(in[i]);
    d_y1 = d_alpha*y1 + d_beta*d_y1;

    double y2 = abs(in[i])*abs(in[i])*abs(in[i])*abs(in[i]);
    d_y2 = d_alpha*y2 + d_beta*d_y2;
  }
  return noutput_items;
}

double
digital_mpsk_snr_est_cc::est_m2m4_snr()
{
  double y1_2 = d_y1*d_y1;
  return 10.0*log10(2.0*sqrt(2*y1_2 - d_y2) / (d_y1 - sqrt(2*y1_2 - d_y2)));
}


int
digital_mpsk_snr_est_cc::est_svn(int noutput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];

  for (int i = 0; i < noutput_items; i++){
    double x = abs(in[i]);
      double x1 = abs(in[i-1]);
    double y1 = (x*x)*(x1*x1);
    d_y1 = d_alpha*y1 + d_beta*d_y1;
    
    double y2 = x*x*x*x;
    d_y2 = d_alpha*y2 + d_beta*d_y2;
  }
  return noutput_items;
}

double
digital_mpsk_snr_est_cc::est_svn_snr()
{
  double x = d_y1 / (d_y2 - d_y1);
  return 10.0*log10(2.*((x-1) + sqrt(x*(x-1))));
}

int
digital_mpsk_snr_est_cc::work(int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
{
  return (*this.*d_estimator)(noutput_items, input_items, output_items);
}

double
digital_mpsk_snr_est_cc::snr()
{
  return (*this.*d_calculator)();
}

snr_est_type_t
digital_mpsk_snr_est_cc::type() const
{
  return d_type;
}

double
digital_mpsk_snr_est_cc::alpha() const
{
  return d_alpha;
}

void
digital_mpsk_snr_est_cc::set_type(snr_est_type_t t)
{
  d_type = t;
  d_y1 = 0;
  d_y2 = 0;
  d_y3 = 0;
  d_y4 = 0;

  switch (d_type) {
  case(SNR_EST_SIMPLE):
    d_estimator = &digital_mpsk_snr_est_cc::est_simple;
    d_calculator = &digital_mpsk_snr_est_cc::est_simple_snr;
    break;
  case(SNR_EST_SKEW):
    d_estimator = &digital_mpsk_snr_est_cc::est_skew;
    d_calculator = &digital_mpsk_snr_est_cc::est_skew_snr;
    break;
  case(SNR_EST_M2M4):
    d_estimator = &digital_mpsk_snr_est_cc::est_m2m4;
    d_calculator = &digital_mpsk_snr_est_cc::est_m2m4_snr;
    break;
  case(SNR_EST_SVN):
    d_estimator = &digital_mpsk_snr_est_cc::est_svn;
    d_calculator = &digital_mpsk_snr_est_cc::est_svn_snr;
    break;
  default:
    throw std::invalid_argument("digital_mpsk_snr_est_cc: unknown type specified.\n");
  }
}

void
digital_mpsk_snr_est_cc::set_alpha(double alpha)
{
  d_alpha = alpha;
  d_beta = 1.0-alpha;
}

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

#include <digital_probe_mpsk_snr_est_c.h>
#include <gr_io_signature.h>
#include <cstdio>

digital_probe_mpsk_snr_est_c_sptr
digital_make_probe_mpsk_snr_est_c(snr_est_type_t type,
				  int msg_nsamples,
				  double alpha)
{
  return gnuradio::get_initial_sptr(
	    new digital_probe_mpsk_snr_est_c(type, msg_nsamples, alpha));
}

digital_probe_mpsk_snr_est_c::digital_probe_mpsk_snr_est_c(
					   snr_est_type_t type,
					   int msg_nsamples,
					   double alpha)
  : gr_sync_block ("probe_mpsk_snr_est_c",
		   gr_make_io_signature(1, 1, sizeof(gr_complex)),
		   gr_make_io_signature(0, 0, 0))
{
  d_snr_est = NULL;

  d_type = type;
  d_nsamples = msg_nsamples;
  d_count = 0;
  set_alpha(alpha);

  set_type(type);

  // at least 1 estimator has to look back
  set_history(2);

  d_key = pmt::pmt_string_to_symbol("snr");
}

digital_probe_mpsk_snr_est_c::~digital_probe_mpsk_snr_est_c()
{
  if(d_snr_est)
    delete d_snr_est;
}

int
digital_probe_mpsk_snr_est_c::work(int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex*)input_items[0];
  return d_snr_est->update(noutput_items, in);
}

double
digital_probe_mpsk_snr_est_c::snr()
{
  if(d_snr_est)
    return d_snr_est->snr();
  else
    throw std::runtime_error("digital_probe_mpsk_snr_est_c:: No SNR estimator defined.\n");
}

snr_est_type_t
digital_probe_mpsk_snr_est_c::type() const
{
  return d_type;
}

int
digital_probe_mpsk_snr_est_c::msg_nsample() const
{
  return d_nsamples;
}

double
digital_probe_mpsk_snr_est_c::alpha() const
{
  return d_alpha;
}

void
digital_probe_mpsk_snr_est_c::set_type(snr_est_type_t t)
{
  d_type = t;

  if(d_snr_est)
    delete d_snr_est;

  switch (d_type) {
  case(SNR_EST_SIMPLE):
    d_snr_est = new digital_impl_mpsk_snr_est_simple(d_alpha);
    break;
  case(SNR_EST_SKEW):
    d_snr_est = new digital_impl_mpsk_snr_est_skew(d_alpha);
    break;
  case(SNR_EST_M2M4):
    d_snr_est = new digital_impl_mpsk_snr_est_m2m4(d_alpha);
    break;
  case(SNR_EST_SVR):
    d_snr_est = new digital_impl_mpsk_snr_est_svr(d_alpha);
    break;
  default:
    throw std::invalid_argument("digital_probe_mpsk_snr_est_c: unknown type specified.\n");
  }
}

void
digital_probe_mpsk_snr_est_c::set_msg_nsample(int n)
{
  if(n > 0) {
    d_nsamples = n;
    d_count = 0;    // reset state
  }
  else
    throw std::invalid_argument("digital_probe_mpsk_snr_est_c: msg_nsamples can't be <= 0\n");
}

void
digital_probe_mpsk_snr_est_c::set_alpha(double alpha)
{
  if((alpha >= 0) && (alpha <= 1.0)) {
    d_alpha = alpha;
    if(d_snr_est)
      d_snr_est->set_alpha(d_alpha);
  }
  else
    throw std::invalid_argument("digital_probe_mpsk_snr_est_c: alpha must be in [0,1]\n");    
}

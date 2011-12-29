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
		   gr_make_io_signature(1, 1, sizeof(gr_complex)))
{
  d_snr_est = NULL;

  d_type = type;
  set_alpha(alpha);

  set_type(type);

  // at least 1 estimator has to look back
  set_history(2);
}

digital_mpsk_snr_est_cc::~digital_mpsk_snr_est_cc()
{
  if(d_snr_est)
    delete d_snr_est;
}

int
digital_mpsk_snr_est_cc::work(int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
{
  // This is a pass-through block; copy input to output
  memcpy(output_items[0], input_items[0],
	 noutput_items * sizeof(gr_complex));

  // Update the SNR estimate registers from the current 
  return d_snr_est->update(noutput_items, input_items);
}

double
digital_mpsk_snr_est_cc::snr()
{
  if(d_snr_est)
    return d_snr_est->snr();
  else
    throw std::runtime_error("digital_mpsk_snr_est_cc:: No SNR estimator defined.\n");
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
    throw std::invalid_argument("digital_mpsk_snr_est_cc: unknown type specified.\n");
  }
}

void
digital_mpsk_snr_est_cc::set_alpha(double alpha)
{
  d_alpha = alpha;
  if(d_snr_est)
    d_snr_est->set_alpha(d_alpha);
}

/* -*- c++ -*- */
/*
 * Copyright 2008,2010 Free Software Foundation, Inc.
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

#include <gr_iqcomp_cc.h>
#include <gr_io_signature.h>

gr_iqcomp_cc_sptr
gr_make_iqcomp_cc (float mu)
{
  return gnuradio::get_initial_sptr(new gr_iqcomp_cc (mu));
}

gr_iqcomp_cc::gr_iqcomp_cc (float mu)
  : gr_sync_block ("iqcomp_cc",
		   gr_make_io_signature (1, 1, sizeof (gr_complex)),
		   gr_make_io_signature (1, 1, sizeof (gr_complex))),
    d_mu (mu)
{
  d_wi=0.0;
  d_wq=0.0;
}

int
gr_iqcomp_cc::work (int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items)
{
  const gr_complex *iptr = (gr_complex *) input_items[0];
  // gr_complex *optr = (gr_complex *) output_items[0];

  for(int i = 0 ; i < noutput_items ; i++) {
    float i_out = iptr[i].real() - iptr[i].imag() * d_wq;
    float q_out = iptr[i].imag() - iptr[i].real() * d_wi;
    d_wi += d_mu * q_out * iptr[i].real();
    d_wq += d_mu * i_out * iptr[i].imag();
  }
  return noutput_items;
}

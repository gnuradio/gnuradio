/* -*- c++ -*- */
/*
 * Copyright 2005,2010 Free Software Foundation, Inc.
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
#include <gr_rms_cf.h>
#include <gr_io_signature.h>
#include <cmath>

gr_rms_cf_sptr
gr_make_rms_cf(double alpha)
{
  return gnuradio::get_initial_sptr(new gr_rms_cf(alpha));
}

gr_rms_cf::gr_rms_cf (double alpha)
  : gr_sync_block ("rms_cf",
		   gr_make_io_signature(1, 1, sizeof(gr_complex)),
		   gr_make_io_signature(1, 1, sizeof(float))),
    d_iir(alpha)
{

}

gr_rms_cf::~gr_rms_cf()
{
}


int
gr_rms_cf::work(int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  float *out = (float *) output_items[0];

  for (int i = 0; i < noutput_items; i++){
    double mag_sqrd = in[i].real()*in[i].real() + in[i].imag()*in[i].imag();
    double f = d_iir.filter(mag_sqrd);
    out[i] = sqrt(f);
  }

  return noutput_items;
}

void
gr_rms_cf::set_alpha(double alpha)
{
  d_iir.set_taps(alpha);
}

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
#include <gr_probe_avg_mag_sqrd_c.h>
#include <gr_io_signature.h>
#include <cmath>

gr_probe_avg_mag_sqrd_c_sptr
gr_make_probe_avg_mag_sqrd_c(double threshold_db, double alpha)
{
  return gnuradio::get_initial_sptr(new gr_probe_avg_mag_sqrd_c(threshold_db, alpha));
}

gr_probe_avg_mag_sqrd_c::gr_probe_avg_mag_sqrd_c (double threshold_db, double alpha)
  : gr_sync_block ("probe_avg_mag_sqrd_c",
		   gr_make_io_signature(1, 1, sizeof(gr_complex)),
		   gr_make_io_signature(0, 0, 0)),
    d_iir(alpha), d_unmuted(false), d_level(0)
{
  set_threshold (threshold_db);
}

gr_probe_avg_mag_sqrd_c::~gr_probe_avg_mag_sqrd_c()
{
}


int
gr_probe_avg_mag_sqrd_c::work(int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];

  for (int i = 0; i < noutput_items; i++){
    double mag_sqrd = in[i].real()*in[i].real() + in[i].imag()*in[i].imag();
    d_iir.filter(mag_sqrd);	// computed for side effect: prev_output()
  }

  d_unmuted = d_iir.prev_output() >= d_threshold;
  d_level = d_iir.prev_output();
  return noutput_items;
}

double
gr_probe_avg_mag_sqrd_c::threshold() const
{
  return 10 * std::log10(d_threshold);
}

void
gr_probe_avg_mag_sqrd_c::set_threshold(double decibels)
{
  // convert to absolute threshold (mag squared)
  d_threshold = std::pow(10.0, decibels/10);
}

void
gr_probe_avg_mag_sqrd_c::set_alpha(double alpha)
{
  d_iir.set_taps(alpha);
}

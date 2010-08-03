/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2010 Free Software Foundation, Inc.
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

#include <gr_io_signature.h>
#include <gr_fractional_interpolator_ff.h>
#include <gri_mmse_fir_interpolator.h>
#include <stdexcept>

// Public constructor
gr_fractional_interpolator_ff_sptr gr_make_fractional_interpolator_ff(float phase_shift, float interp_ratio)
{
  return gnuradio::get_initial_sptr(new gr_fractional_interpolator_ff(phase_shift, interp_ratio));
}

gr_fractional_interpolator_ff::gr_fractional_interpolator_ff(float phase_shift, float interp_ratio)
  : gr_block ("fractional_interpolator_ff",
	      gr_make_io_signature (1, 1, sizeof (float)),
	      gr_make_io_signature (1, 1, sizeof (float))),
    d_mu (phase_shift), d_mu_inc (interp_ratio), d_interp(new gri_mmse_fir_interpolator())
{
  if (interp_ratio <=  0)
    throw std::out_of_range ("interpolation ratio must be > 0");
  if (phase_shift <  0  || phase_shift > 1)
    throw std::out_of_range ("phase shift ratio must be > 0 and < 1");

  set_relative_rate (1.0 / interp_ratio);
}

gr_fractional_interpolator_ff::~gr_fractional_interpolator_ff()
{
  delete d_interp;
}

void
gr_fractional_interpolator_ff::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned ninputs = ninput_items_required.size();
  for (unsigned i=0; i < ninputs; i++)

    ninput_items_required[i] =
      (int) ceil((noutput_items * d_mu_inc) + d_interp->ntaps());
}

int
gr_fractional_interpolator_ff::general_work(int noutput_items,
					    gr_vector_int &ninput_items,
					    gr_vector_const_void_star &input_items,
					    gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  float *out = (float *) output_items[0];

  int 	ii = 0;				// input index
  int  	oo = 0;				// output index

  while (oo < noutput_items) {

    out[oo++] = d_interp->interpolate(&in[ii], d_mu);

    double s = d_mu + d_mu_inc;
    double f = floor (s);
    int incr = (int) f;
    d_mu = s - f;
    ii += incr;
  }

  consume_each (ii);

  return noutput_items;
}

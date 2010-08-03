/* -*- c++ -*- */
/*
 * Copyright 2006,2010 Free Software Foundation, Inc.
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

#include <gr_pa_2x2_phase_combiner.h>
#include <gr_io_signature.h>

gr_pa_2x2_phase_combiner_sptr
gr_make_pa_2x2_phase_combiner()
{
  return gnuradio::get_initial_sptr(new gr_pa_2x2_phase_combiner());
}

gr_pa_2x2_phase_combiner::gr_pa_2x2_phase_combiner ()
  : gr_sync_block ("pa_2x2_phase_combiner",
		   gr_make_io_signature (1, 1, NM * sizeof (gr_complex)),
		   gr_make_io_signature (1, 1, sizeof (float)))
{
  set_theta(0);
}

void
gr_pa_2x2_phase_combiner::set_theta(float theta)
{
  d_theta = theta;
  gr_complex j = gr_complex(0,1);
  d_phase[0] = exp(j * (float) (M_PI * (sin(theta) + cos(theta))));
  d_phase[1] = exp(j * (float) (M_PI * cos(theta)));
  d_phase[2] = exp(j * (float) (M_PI * sin(theta)));
  d_phase[3] = exp(j * (float) 0.0);
}

int
gr_pa_2x2_phase_combiner::work (int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];

  for (int i = 0; i < noutput_items; i++){
    gr_complex	acc = 0;
    acc += in[0] * d_phase[0];
    acc += in[1] * d_phase[1];
    acc += in[2] * d_phase[2];
    acc += in[3] * d_phase[3];
    out[i] = acc;
    in += 4;
  }

  return noutput_items;
}

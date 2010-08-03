/* -*- c++ -*- */
/*
 * Copyright 2007,2009,2010 Free Software Foundation, Inc.
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

#include <gr_dpll_bb.h>
#include <gr_io_signature.h>
#include <cstdio>

gr_dpll_bb_sptr
gr_make_dpll_bb (float period, float gain)
{
  return gnuradio::get_initial_sptr(new gr_dpll_bb (period, gain));
}

gr_dpll_bb::gr_dpll_bb (float period, float gain)
  : gr_sync_block ("dpll_bb",
		   gr_make_io_signature (1, 1, sizeof (char)),
		   gr_make_io_signature (1, 1, sizeof (char))),
    d_restart(0),d_pulse_phase(0)
{
  d_pulse_frequency = 1.0/period;
  d_gain = gain;
  d_decision_threshold = 1.0 - 0.5*d_pulse_frequency;
#if 1
  fprintf(stderr,"frequency = %f period = %f gain = %f threshold = %f\n",
	  d_pulse_frequency,
	  period,
	  d_gain,
	  d_decision_threshold);
#endif
}

int
gr_dpll_bb::work (int noutput_items,
	      gr_vector_const_void_star &input_items,
	      gr_vector_void_star &output_items)
{
  const char *iptr = (const char *) input_items[0];
  char *optr = (char *) output_items[0];

  for (int i = 0; i < noutput_items; i++){
    optr[i]= 0;
    if(iptr[i] == 1) {
      if (d_restart == 0) {
	d_pulse_phase = 1;
      } else {
	if (d_pulse_phase > 0.5) d_pulse_phase += d_gain*(1.0-d_pulse_phase);
	else d_pulse_phase -= d_gain*d_pulse_phase;
      }
      d_restart = 3;
    }
    if (d_pulse_phase > d_decision_threshold) {
      d_pulse_phase -= 1.0;
      if (d_restart > 0) {
	d_restart -= 1;
	optr[i] = 1;
      }
    }
    d_pulse_phase += d_pulse_frequency;
  }
  return noutput_items;
}

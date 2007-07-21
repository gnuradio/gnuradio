/* -*- c++ -*- */
/*
 * Copyright 2004,2006 Free Software Foundation, Inc.
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

#include <gr_squelch_base_ff.h>
#include <gr_io_signature.h>

gr_squelch_base_ff::gr_squelch_base_ff(const char *name, int ramp, bool gate) : 
	gr_block(name,
	         gr_make_io_signature(1, 1, sizeof(float)),
	         gr_make_io_signature(1, 1, sizeof(float)))
{
  set_ramp(ramp);
  set_gate(gate);
  d_state = ST_MUTED;
  d_envelope = d_ramp ? 0.0 : 1.0;
  d_ramped = 0;
}

int gr_squelch_base_ff::general_work(int noutput_items,
				     gr_vector_int &ninput_items,
				     gr_vector_const_void_star &input_items,
				     gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  float *out = (float *) output_items[0];

  int j = 0;

  for (int i = 0; i < noutput_items; i++) {
    update_state(in[i]);

    // Adjust envelope based on current state
    switch(d_state) {
      case ST_MUTED:
        if (!mute()) 
          d_state = d_ramp ? ST_ATTACK : ST_UNMUTED; // If not ramping, go straight to unmuted
        break;

      case ST_UNMUTED:
        if (mute())
          d_state = d_ramp ? ST_DECAY : ST_MUTED;    // If not ramping, go straight to muted
        break;

      case ST_ATTACK:
        d_envelope = 0.5-std::cos(M_PI*(++d_ramped)/d_ramp)/2.0; // FIXME: precalculate window for speed
        if (d_ramped >= d_ramp) { // use >= in case d_ramp is set to lower value elsewhere
	  d_state = ST_UNMUTED;
	  d_envelope = 1.0;
        }
        break;

      case ST_DECAY:
        d_envelope = 0.5-std::cos(M_PI*(--d_ramped)/d_ramp)/2.0; // FIXME: precalculate window for speed
        if (d_ramped == 0.0)
	  d_state = ST_MUTED;
        break;
    };
       
    // If unmuted, copy input times envelope to output
    // Otherwise, if not gating, copy zero to output
    if (d_state != ST_MUTED)
      out[j++] = in[i]*d_envelope;
    else
      if (!d_gate)
          out[j++] = 0.0;
  }

  consume_each(noutput_items);  // Use all the inputs
  return j;		        // But only report outputs copied
}

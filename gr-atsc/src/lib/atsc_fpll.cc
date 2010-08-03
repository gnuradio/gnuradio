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
#include <config.h>
#endif

#include <atsc_fpll.h>
#include <gr_io_signature.h>
#include <atsc_consts.h>
#include <algorithm>
#include "fpll_btloop_coupling.h"
#include <gr_math.h>

atsc_fpll_sptr
atsc_make_fpll()
{
  return gnuradio::get_initial_sptr(new atsc_fpll());
}


/*
 * I strongly suggest that you not mess with these...
 *
 * They are strongly coupled into the symbol timing code and
 * their value also sets the level of the symbols going
 * into the equalizer and viterbi decoder.
 */
static const float FPLL_AGC_REFERENCE = 2.5 * FPLL_BTLOOP_COUPLING_CONST;
static const float FPLL_AGC_RATE = 0.25e-6;



atsc_fpll::atsc_fpll()
  : gr_sync_block("atsc_fpll",
		  gr_make_io_signature(1, 1, sizeof(float)),
		  gr_make_io_signature(1, 1, sizeof(float))),
		  initial_phase(0)
{
  initial_freq = 5.75e6 - 3e6 + 0.31e6 + 5e3; // a_initial_freq;
  agc.set_rate (FPLL_AGC_RATE);
  agc.set_reference (FPLL_AGC_REFERENCE);
  initialize();
}


void
atsc_fpll::initialize ()
{
  float Fs = 19.2e6;

  float alpha = 1 - exp(-1.0 / Fs / 5e-6);

  afci.set_taps (alpha);
  afcq.set_taps (alpha);

  printf("Setting initial_freq: %f\n",initial_freq);
  nco.set_freq (initial_freq / Fs * 2 * M_PI);
  nco.set_phase (initial_phase);
}


int
atsc_fpll::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  float *out = (float *) output_items[0];

  for (int k = 0; k < noutput_items; k++){

    float a_cos, a_sin;

    float input = agc.scale (in[k]);

    nco.step ();                // increment phase
    nco.sincos (&a_sin, &a_cos);  // compute cos and sin

    float I = input * a_sin;
    float Q = input * a_cos;

    out[k] = I;

    float filtered_I = afci.filter (I);
    float filtered_Q = afcq.filter (Q);

    // phase detector

    // float x = atan2 (filtered_Q, filtered_I);
    float x = gr_fast_atan2f(filtered_Q, filtered_I);

    // avoid slamming filter with big transitions

    static const float limit = M_PI / 2;

    if (x > limit)
      x = limit;
    else if (x < -limit)
      x = -limit;

    // static const float alpha = 0.037;   // Max value
    // static const float alpha = 0.005;   // takes about 5k samples to pull in, stddev = 323
    // static const float alpha = 0.002;   // takes about 15k samples to pull in, stddev =  69
                                           //  or about 120k samples on noisy data,
    static const float alpha = 0.001;
    static const float beta = alpha * alpha / 4;

    nco.adjust_phase (alpha * x);
    nco.adjust_freq (beta * x);

  }

  return noutput_items;
}




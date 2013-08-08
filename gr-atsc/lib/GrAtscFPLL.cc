/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#include <gnuradio/atsc/GrAtscFPLL.h>
#include <algorithm>
#include "fpll_btloop_coupling.h"

/*
 * I strongly suggest that you not mess with these...
 *
 * They are strongly coupled into the symbol timing code and
 * their value also sets the level of the symbols going
 * into the equalizer and viterbi decoder.
 */
static const float FPLL_AGC_REFERENCE = 2.5 * FPLL_BTLOOP_COUPLING_CONST;
static const float FPLL_AGC_RATE = 0.25e-6;


GrAtscFPLL::GrAtscFPLL (double a_initial_freq)
  : VrSigProc (1, sizeof (iType), sizeof (oType)),
    initial_phase(0), debug_no_update(false)
{
  initial_freq = a_initial_freq;
  agc.set_rate (FPLL_AGC_RATE);
  agc.set_reference (FPLL_AGC_REFERENCE);

  if (_FPLL_DIAG_OUTPUT_){
    fp = fopen ("fpll.out", "w");
    if (fp == 0){
      perror ("fpll.out");
      exit (1);
    }
  }

}

void
GrAtscFPLL::initialize ()
{
  float Fs = getInputSamplingFrequencyN (0);

  float alpha = 1 - exp(-1.0 / Fs / 5e-6);

  afci.set_taps (alpha);
  afcq.set_taps (alpha);

  nco.set_freq (initial_freq / Fs * 2 * M_PI);
  nco.set_phase (initial_phase);
}

int
GrAtscFPLL::work (VrSampleRange output, void *ao[],
		  VrSampleRange inputs[], void *ai[])
{
  iType	 *in = ((iType **)ai)[0];
  oType  *out = ((oType **)ao)[0];

  unsigned int	k;

  for (k = 0; k < output.size; k++){

    float a_cos, a_sin;

    float input = agc.scale (in[k]);

    nco.step ();		// increment phase
    nco.sincos (a_sin, a_cos);	// compute cos and sin

    float I = input * a_sin;
    float Q = input * a_cos;

    out[k] = I;

    float filtered_I = afci.filter (I);
    float filtered_Q = afcq.filter (Q);

    // phase detector

    float x = atan2 (filtered_Q, filtered_I);

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


    if (!debug_no_update){
      nco.adjust_phase (alpha * x);
      nco.adjust_freq (beta * x);
    }

    if (_FPLL_DIAG_OUTPUT_){
#if 0	// lots of data...
      float	iodata[8];
      iodata[0] = nco.get_freq () * getSamplingFrequency () * (1.0 / (2 * M_PI));
      iodata[1] = in[k];
      iodata[2] = input;
      iodata[3] = I;
      iodata[4] = Q;
      iodata[5] = filtered_I;
      iodata[6] = filtered_Q;
      iodata[7] = x;
      if (fwrite (iodata, sizeof (iodata), 1, fp) != 1){
	perror ("fwrite: fpll");
	exit (1);
      }
#else	// just the frequency
      float	iodata[1];
      iodata[0] = nco.get_freq () * getSamplingFrequency () * (1.0 / (2 * M_PI));
      if (fwrite (iodata, sizeof (iodata), 1, fp) != 1){
	perror ("fwrite: fpll");
	exit (1);
      }
#endif
    }
  }

  return output.size;
}


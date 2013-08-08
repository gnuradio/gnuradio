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

#include <gnuradio/atsc/GrAtscConvert2xTo20.h>
#include <gnuradio/atsc/consts.h>
#include <cmath>
#include <cstdio>

static const int    N_OUTPUTS = 1;
static const double DEC_RATIO = (2.0 * ATSC_SYMBOL_RATE) / 20e6;	// ~ 1.076

GrAtscConvert2xTo20::GrAtscConvert2xTo20 ()
  : VrDecimatingSigProc<float,float> (N_OUTPUTS, (int) rint (DEC_RATIO))
{
  d_next_input = 0;
  d_frac_part = 0;
  history = 2 * d_interp.ntaps ();	// some slack
}

GrAtscConvert2xTo20::~GrAtscConvert2xTo20 ()
{
  // Nop
}

void
GrAtscConvert2xTo20::pre_initialize ()
{
  fprintf (stderr,
	   "GrAtscConvert2xTo20: input freq = %g\n", getInputSamplingFrequencyN(0));
  fprintf (stderr,
	   "GrAtscConvert2xTo20: DEC_RATIO = %g\n", DEC_RATIO);
  fprintf (stderr,
	   "GrAtscConvert2xTo20: argument to setSamplingFrequency = %g\n",
	   getInputSamplingFrequencyN(0) / DEC_RATIO);

  int	r;
  r = setSamplingFrequency (getInputSamplingFrequencyN (0) / DEC_RATIO);

  fprintf (stderr, "GrAtscConvert2xTo20: result = %d\n", r);

  fprintf (stderr, "GrAtscConvert2xTo20: getSamplingFrequency = %g\n",
	   getSamplingFrequency ());
}


int
GrAtscConvert2xTo20::forecast (VrSampleRange output,
			       VrSampleRange inputs[])
{
  assert (numberInputs == 1);	// I hate these free references to
				// superclass's instance variables...

  inputs[0].index = d_next_input;
  inputs[0].size =
    ((long unsigned int) (output.size * DEC_RATIO) + history - 1);

  return 0;
}

int
GrAtscConvert2xTo20::work (VrSampleRange output, void *ao[],
			   VrSampleRange inputs[], void *ai[])
{
  float	*in = ((float **) ai)[0];
  float *out = ((float **) ao)[0];

  sync (output.index);

  unsigned long  si = 0;		// source index
  unsigned long	 oi = 0;		// output index
  double frac_part = d_frac_part;

  for (oi = 0; oi < output.size; oi++){
    assert (si + d_interp.ntaps () < inputs[0].size);
    out[oi] = d_interp.interpolate (&in[si], (1. - frac_part));

    double s = frac_part + DEC_RATIO;
    double float_incr = floor (s);
    frac_part = s - float_incr;
    int incr = (int) float_incr;
    si += incr;
  }

  d_next_input += si;
  d_frac_part = frac_part;
  return output.size;
}

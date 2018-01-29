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

#include <gnuradio/atsc/GrAtscBitTimingLoop2.h>
#include <algorithm>
#include <gnuradio/atsc/consts.h>
#include <stdio.h>
#include <assert.h>


static const int	DEC = 2;	// nominal decimation factor

static const unsigned	AVG_WINDOW_LEN = 256;
static const float	TIMING_RATE_CONST = 1e-5;    // FIXME document interaction with AGC


GrAtscBitTimingLoop2::GrAtscBitTimingLoop2 ()
  : VrDecimatingSigProc<float,float> (1, DEC),
    next_input(0), dc (0.0002), mu (0.0), last_right(0), use_right_p (true)
{
  history = 100;  	// spare input samples in case we need them.

#ifdef _BT_DIAG_OUTPUT_
  fp_loop = fopen ("loop.out", "w");
  if (fp_loop == 0){
    perror ("loop.out");
    exit (1);
  }

  fp_ps = fopen ("ps.out", "w");
  if (fp_ps == 0){
    perror ("ps.out");
    exit (1);
  }
#endif

}

//
// We are nominally a 2x decimator, but our actual rate varies slightly
// depending on the difference between the transmitter and receiver
// sampling clocks.  Hence, we need to compute our input ranges
// explicitly.

int
GrAtscBitTimingLoop2::forecast(VrSampleRange output,
			      VrSampleRange inputs[]) {
  /* dec:1 ratio with history */
  for(unsigned int i=0;i<numberInputs;i++) {
    inputs[i].index=next_input;
    inputs[i].size=output.size*decimation + history-1;
  }
  return 0;
}

inline float
GrAtscBitTimingLoop2::filter_error (float e)
{
  return e;	// identity function
}

int
GrAtscBitTimingLoop2::work (VrSampleRange output, void *ao[],
			   VrSampleRange inputs[], void *ai[])
{
  iType	 *in = ((iType **)ai)[0];
  oType  *out = ((oType **)ao)[0];

  // Force in-order computation of output stream.
  // This is required because of our slightly variable decimation factor
  sync (output.index);


  // We are tasked with producing output.size output samples.
  // We will consume approximately 2 * output.size input samples.


  unsigned int	ii = 0;		// input index
  unsigned int	k;		// output index

  // We look at a window of 3 samples that we call left (oldest),
  // middle, right (newest).  Each time through the loop, the previous
  // right becomes the new left, and the new samples are middle and
  // right.
  //
  // The basic game plan is to drive the average difference between
  // right and left to zero.  Given that all transitions are
  // equiprobable (the data is white) and that the composite matched
  // filter is symmetric (raised cosine) it turns out that in the
  // average, if we drive that difference to zero, (implying that the
  // average slope at the middle point is zero), we'll be sampling
  // middle at the maximum or minimum point in the pulse.

  iType	left;
  iType middle;
  iType	right = last_right;

  for (k = 0; k < output.size; k++){

    left = right;

    iType middle_raw = produce_sample (in, ii);
    iType middle_dc = dc.filter (middle_raw);
    middle = middle_raw - middle_dc;

    iType right_raw = produce_sample (in, ii);
    iType right_dc = dc.filter (right_raw);
    right = right_raw - right_dc;

    if (use_right_p)	// produce our output
      out[k] = right;
    else
      out[k] = middle;
  }

#ifdef _BT_DIAG_OUTPUT_
  float	iodata[8];
  iodata[0] = 0;
  iodata[1] = out[k];
  iodata[2] = 0;
  iodata[3] = 0;
  iodata[4] = 0;
  iodata[5] = mu;
  iodata[6] = 0;
  iodata[7] = 0;	// spare
  if (fwrite (iodata, sizeof (iodata), 1, fp_loop) != 1){
    perror ("fwrite: loop");
    exit (1);
  }
#endif


  last_right = right;
  next_input += ii;	// update next_input so forecast can get us what we need
  return output.size;
}

/*!
 * Produce samples equally spaced in time that are referenced
 * to the transmitter's sample clock, not ours.
 *
 * See pp 523-527 of "Digital Communication Receivers", Meyr,
 * Moeneclaey and Fechtel, Wiley, 1998.
 */

GrAtscBitTimingLoop2::iType
GrAtscBitTimingLoop2::produce_sample (const iType *in, unsigned int &index)
{
  iType n = intr.interpolate (&in[index], mu);

  index++;
  return n;
}


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

#include <cmath>
#include <gnuradio/atsc/GrAtscBitTimingLoop.h>
#include "fpll_btloop_coupling.h"
#include <algorithm>
#include <gnuradio/atsc/consts.h>
#include <stdio.h>
#include <assert.h>

using std::abs;

static const int     DEC = 2;	// nominal decimation factor

/*
 * I strongly suggest that you not mess with these...
 */
static const double   DEFAULT_TIMING_RATE = 2.19e-4 / FPLL_BTLOOP_COUPLING_CONST;
static const double   DEFAULT_LOOP_TAP    = 0.05;


GrAtscBitTimingLoop::GrAtscBitTimingLoop ()
  : VrDecimatingSigProc<float,float> (1, DEC),
    next_input(0), w (1.0), mu (0.5), last_right(0),
    debug_no_update (false)
{
  d_timing_rate = DEFAULT_TIMING_RATE;
  loop.set_taps (DEFAULT_LOOP_TAP);

  history = 1500;  	// spare input samples in case we need them.

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
GrAtscBitTimingLoop::forecast(VrSampleRange output,
			      VrSampleRange inputs[]) {
  /* dec:1 ratio with history */
  for(unsigned int i=0;i<numberInputs;i++) {
    inputs[i].index=next_input;
    inputs[i].size=output.size*decimation + history-1;
  }
  return 0;
}

inline double
GrAtscBitTimingLoop::filter_error (double e)
{
  static const double limit = 50 * FPLL_BTLOOP_COUPLING_CONST;

  // first limit

  if (e > limit)
    e = limit;
  else if (e < -limit)
    e = -limit;

  return loop.filter (e);
}

int
GrAtscBitTimingLoop::work (VrSampleRange output, void *ao[],
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
    middle = produce_sample (in, ii);
    right = produce_sample (in, ii);

    // assert (ii < inputs[0].size);
    if (!(ii < inputs[0].size)){
      fprintf (stderr, "ii < inputs[0].size\n");
      fprintf (stderr, "ii = %d, inputs[0].size = %lu, k = %d, output.size = %lu\n",
	       ii, inputs[0].size, k, output.size);
      assert (0);
    }


    out[k] = middle;	// produce our output

    double timing_error = -middle * ((double) right - left);

    // update_timing_control_word

    double filtered_timing_error = filter_error (timing_error);

    if (!debug_no_update){
      mu += filtered_timing_error * d_timing_rate;
    }

#ifdef _BT_DIAG_OUTPUT_
    float	iodata[8];
    iodata[0] = left;
    iodata[1] = middle;
    iodata[2] = right;
    iodata[3] = timing_error;
    iodata[4] = filtered_timing_error;
    iodata[5] = mu;
    iodata[6] = w;
    iodata[7] = 0;
    if (fwrite (iodata, sizeof (iodata), 1, fp_loop) != 1){
      perror ("fwrite: loop");
      exit (1);
    }
#endif

  }

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

GrAtscBitTimingLoop::iType
GrAtscBitTimingLoop::produce_sample (const iType *in, unsigned int &index)
{
  // update mu and index as function of control word, w

  double sum = mu + w;
  double f = floor (sum);
  int incr = (int) f;		// mostly 1, rarely 0 or 2
  mu = sum - f;

  assert (0 <= incr && incr <= 2);
  assert (0.0 <= mu && mu <= 1.0);

  index += incr;

  iType n = intr.interpolate (&in[index], mu);

#if defined(_BT_DIAG_OUTPUT_) && 0
  float	iodata[4];
  iodata[0] = incr;
  iodata[1] = mu;
  iodata[2] = w;
  iodata[3] = 0;
  if (fwrite (iodata, sizeof (iodata), 1, fp_ps) != 1){
    perror ("fwrite: ps");
    exit (1);
  }
#endif

  return n;
}

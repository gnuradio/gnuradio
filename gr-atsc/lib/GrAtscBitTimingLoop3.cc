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

#include <gnuradio/atsc/GrAtscBitTimingLoop3.h>
#include <cmath>
#include <cstdio>
#include <assert.h>

using std::abs;


static const int NOUTPUTS = 2;

GrAtscBitTimingLoop3::GrAtscBitTimingLoop3 (double ratio_of_rx_clock_to_symbol_freq)
  : VrDecimatingSigProc<float,float> (NOUTPUTS, (int) rint (ratio_of_rx_clock_to_symbol_freq)),
    d_interp (ratio_of_rx_clock_to_symbol_freq), d_next_input(0),
    d_rx_clock_to_symbol_freq (ratio_of_rx_clock_to_symbol_freq)

{
  assert (ratio_of_rx_clock_to_symbol_freq >= 1.8);	// sanity check

  history = 1500;  	// spare input samples in case we need them.
}

//
// We are nominally a 2x decimator, but our actual rate varies slightly
// depending on the difference between the transmitter and receiver
// sampling clocks.  Hence, we need to compute our input ranges
// explicitly.

int
GrAtscBitTimingLoop3::forecast(VrSampleRange output,
			       VrSampleRange inputs[]) {
  assert (numberInputs == 1);

  /* dec:1 ratio with history */
  inputs[0].index = d_next_input;
  inputs[0].size =
    ((unsigned long) (output.size * d_rx_clock_to_symbol_freq) + history - 1);

  return 0;
}


int
GrAtscBitTimingLoop3::work (VrSampleRange output, void *ao[],
			    VrSampleRange inputs[], void *ai[])
{
  iType	 *in = ((iType **)ai)[0];
  oDataType *out_sample = ((oDataType **)ao)[0];
  oTagType  *out_tag =    ((oTagType **) ao)[1];

  // Force in-order computation of output stream.
  // This is required because of our slightly variable decimation factor
  sync (output.index);

  // We are tasked with producing output.size output samples.
  // We will consume approximately 2 * output.size input samples.

  int		si = 0;		// source index
  unsigned int	k;		// output index

  float		interp_sample;
  int		symbol_index;
  double	timing_adjustment = 0;
  bool		seg_locked;
  oTagType	tag;

  memset (&tag, 0, sizeof (tag));

  for (k = 0; k < output.size; k++){

    if (!d_interp.update (in, inputs[0].size, &si, timing_adjustment, &interp_sample)){
      fprintf (stderr, "GrAtscBitTimingLoop3: ran short on data...\n");
      break;
    }

    d_sssr.update (interp_sample, &seg_locked, &symbol_index, &timing_adjustment);
    out_sample[k] = interp_sample;
    tag.valid = seg_locked;
    tag.symbol_num = symbol_index;
    out_tag[k] = tag;
  }

  d_next_input += si;	// update next_input so forecast can get us what we need
  return k;
}

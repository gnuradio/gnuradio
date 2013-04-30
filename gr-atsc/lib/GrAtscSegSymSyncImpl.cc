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

#include <gnuradio/atsc/GrAtscSegSymSyncImpl.h>
#include <gnuradio/atsc/GrAtscSegSymSyncImpl_export.h>
#include <cmath>
#include <assert.h>

GrAtscSegSymSyncImpl::GrAtscSegSymSyncImpl (
    double nominal_ratio_of_rx_clock_to_symbol_freq)
  : d_interp (nominal_ratio_of_rx_clock_to_symbol_freq)
{
  // set approximate decimation rate for superclass's benefit
  decimation = (int) rint (nominal_ratio_of_rx_clock_to_symbol_freq);

  history = 1500;  	// spare input samples in case we need them.

  d_sssr.reset ();
  d_interp.reset ();
  d_next_input = 0;
  d_rx_clock_to_symbol_freq = nominal_ratio_of_rx_clock_to_symbol_freq;
}

GrAtscSegSymSyncImpl::~GrAtscSegSymSyncImpl ()
{
  // Nop
}

void
GrAtscSegSymSyncImpl::pre_initialize ()
{
  setSamplingFrequency (
     getInputSamplingFrequencyN (0) / d_rx_clock_to_symbol_freq);
}

int
GrAtscSegSymSyncImpl::forecast (VrSampleRange output,
				VrSampleRange inputs[])
{
  assert (numberInputs == 1);	// I hate these free references to
				// superclass's instance variables...

  inputs[0].index = d_next_input;
  inputs[0].size =
    ((long unsigned int) (output.size * d_rx_clock_to_symbol_freq)
     + history - 1);

  return 0;
}

int
GrAtscSegSymSyncImpl::work (VrSampleRange output, void *ao[],
			    VrSampleRange inputs[], void *ai[])
{
#if 0
  float	        *input_samples  = ((float **) ai)[0];
  float         *output_samples = ((float **) ao)[0];
  atsc::syminfo *output_info    = ((atsc::syminfo **) ao)[1];

  // FIXME finish...
#endif
  assert (0);

  return output.size;
}

void
GrAtscSegSymSyncImpl::reset ()
{
  d_sssr.reset ();
  d_interp.reset ();
}


/*
 * Exported constructor.
 * Doesn't expose any of the internals or our compile time dependencies.
 */

GrAtscSegSymSync *
create_GrAtscSegSymSyncImpl (double nominal_ratio_of_rx_clock_to_symbol_freq)
{
  return new GrAtscSegSymSyncImpl (nominal_ratio_of_rx_clock_to_symbol_freq);
}

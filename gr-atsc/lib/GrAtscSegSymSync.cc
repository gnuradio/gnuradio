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

#include <gnuradio/atsc/GrAtscSegSymSync.h>
#include <gnuradio/atsc/GrAtscSegSymSyncImpl_export.h>
#include <iostream>
#include <assert.h>

static const int DECIMATION = 2;	// close enough for super class's use
static const int N_OUTPUTS = 2;

GrAtscSegSymSync::GrAtscSegSymSync ()
  : VrDecimatingSigProc<float,float> (N_OUTPUTS, DECIMATION)
{
  if (sizeof (float) != sizeof (atsc::syminfo)){
    cerr << "GrAtscSegSymSync: sizeof (float) != sizeof (atsc::syminfo)\n";
    assert (0);
  }
}

GrAtscSegSymSync::~GrAtscSegSymSync ()
{
  // Nop
}


GrAtscSegSymSync *
GrAtscSegSymSync::create (double nominal_ratio_of_rx_clock_to_symbol_freq)
{
  return create_GrAtscSegSymSyncImpl (nominal_ratio_of_rx_clock_to_symbol_freq);
}

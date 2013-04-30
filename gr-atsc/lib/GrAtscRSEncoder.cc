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

#include <gnuradio/atsc/GrAtscRSEncoder.h>

// typedefs for fundamental i/o types

typedef atsc_mpeg_packet_no_sync	iType;
typedef atsc_mpeg_packet_rs_encoded	oType;

static const int NUMBER_OF_OUTPUTS = 1;	// # of output streams (almost always one)


GrAtscRSEncoder::GrAtscRSEncoder ()
  : VrHistoryProc<iType,oType> (NUMBER_OF_OUTPUTS)
{
  // 1 + number of extra input elements at which we look.  This is
  // used by the superclass's forecast routine to get us the correct
  // range on our inputs.
  // We're one-to-one input-to-output so set it to 1.
  history = 1;

  // any other init here.
}

GrAtscRSEncoder::~GrAtscRSEncoder ()
{
  // Anything that isn't automatically cleaned up...
}

/*
 * This is the real work horse.  In general this interface can handle
 * multiple streams of input and output, but we almost always
 * use a single input and output stream.
 */

int
GrAtscRSEncoder::work (VrSampleRange output, void *ao[],
			VrSampleRange inputs[], void *ai[])
{
  // construct some nicer i/o pointers to work with.

  iType *in  = ((iType **) ai)[0];
  oType *out = ((oType **) ao)[0];


  // We must produce output.size units of output.

  for (unsigned int i = 0; i < output.size; i++){

    // ensure that on the way in, the error bit is clear
    // [assertion is not valid, because the randomizer has already scrambled the bits]
    // assert ((in[i].data[0] & MPEG_TRANSPORT_ERROR_BIT) == 0);

    assert (in[i].pli.regular_seg_p ());
    out[i].pli = in[i].pli;			// copy pipeline info...
    rs_encoder.encode (out[i], in[i]);
  }

  // Return the number of units we produced.
  // Note that for all intents and purposes, it is an error to
  // produce less than you are asked for.

  return output.size;
}

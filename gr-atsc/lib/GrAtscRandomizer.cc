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

#include <gnuradio/atsc/GrAtscRandomizer.h>

// typedefs for fundamental i/o types

typedef atsc_mpeg_packet		iType;
typedef atsc_mpeg_packet_no_sync	oType;

static const int NUMBER_OF_OUTPUTS = 1;	// # of output streams (almost always one)


GrAtscRandomizer::GrAtscRandomizer ()
  : VrHistoryProc<iType,oType> (NUMBER_OF_OUTPUTS),
    field2 (false), segno (0)
{
  // 1 + number of extra input elements at which we look.  This is
  // used by the superclass's forecast routine to get us the correct
  // range on our inputs.
  // We're one-to-one input-to-output so set it to 1.
  history = 1;

  // any other init here.
}

GrAtscRandomizer::~GrAtscRandomizer ()
{
  // Anything that isn't automatically cleaned up...
}

/*
 * This is the real work horse.  In general this interface can handle
 * multiple streams of input and output, but we almost always
 * use a single input and output stream.
 */

int
GrAtscRandomizer::work (VrSampleRange output, void *ao[],
			VrSampleRange inputs[], void *ai[])
{
  // If we have state that persists across invocations (e.g., we have
  // instance variables that we modify), we must use the sync method
  // to indicate to the scheduler that our output must be computed in
  // order.  This doesn't keep other things from being run in
  // parallel, it just means that at any given time, there is only a
  // single thread working this code, and that the scheduler will
  // ensure that we are asked to produce output that is contiguous and
  // that will be presented to us in order of increasing time.

  // We have state, the current contents of the LFSR in the randomizer, hence
  // we must use sync.

  sync (output.index);

  // construct some nicer i/o pointers to work with.

  iType *in  = ((iType **) ai)[0];
  oType *out = ((oType **) ao)[0];


  // We must produce output.size units of output.

  for (unsigned int i = 0; i < output.size; i++){

    // initialize plinfo for downstream
    //
    // We do this here because the randomizer is effectively
    // the head of the tx processing chain
    //
    out[i].pli.set_regular_seg (field2, segno);
    segno++;
    if (segno == 312){
      segno = 0;
      field2 = !field2;
    }

    assert ((in[i].data[1] & MPEG_TRANSPORT_ERROR_BIT) == 0);

    if (out[i].pli.first_regular_seg_p ())
      rand.reset ();

    rand.randomize (out[i], in[i]);
  }

  // Return the number of units we produced.
  // Note that for all intents and purposes, it is an error to
  // produce less than you are asked for.

  return output.size;
}

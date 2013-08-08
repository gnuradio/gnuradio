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

#include <gnuradio/atsc/GrAtscTrellisEncoder.h>

// typedefs for fundamental i/o types

typedef atsc_mpeg_packet_rs_encoded	iType;
typedef atsc_data_segment		oType;

static const int NUMBER_OF_OUTPUTS = 1;	// # of output streams (almost always one)


GrAtscTrellisEncoder::GrAtscTrellisEncoder ()
  : VrHistoryProc<iType,oType> (NUMBER_OF_OUTPUTS), last_start(-1)
{
  // 1 + number of extra input elements at which we look.  This is
  // used by the superclass's forecast routine to get us the correct
  // range on our inputs.
  //
  // We need our input to be aligned on a 12-segment boundary,
  // to ensure satisfaction, ask for 11 more
  history = 1 + (atsci_trellis_encoder::NCODERS - 1);

  // any other init here.

  // Let the bottom end know we must produce output in multiples of 12 segments.
  setOutputSize (atsci_trellis_encoder::NCODERS);
}

GrAtscTrellisEncoder::~GrAtscTrellisEncoder ()
{
  // Anything that isn't automatically cleaned up...
}

/*
 * This is the real work horse.  In general this interface can handle
 * multiple streams of input and output, but we almost always
 * use a single input and output stream.
 */

int
GrAtscTrellisEncoder::work (VrSampleRange output, void *ao[],
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

  // We have state, the current state of the encoders, hence
  // we must use sync.

  sync (output.index);

  // construct some nicer i/o pointers to work with.

  iType *in  = ((iType **) ai)[0];
  oType *out = ((oType **) ao)[0];


#if 0
  cerr << "@@@ GrAtscTrellisEncoder: output.index = " << output.index
       << " output.size = " << output.size
       << " sum = " << output.index + output.size
       << " \t[in  = " << in << "]"
       << endl;
#endif

  assert (output.size % atsci_trellis_encoder::NCODERS == 0);


  // find the first mod 12 boundary to begin decoding
  int start;
  for (start = 0; start < atsci_trellis_encoder::NCODERS; start++){
    plinfo::sanity_check (in[start].pli);
    assert (in[start].pli.regular_seg_p ());
    if ((in[start].pli.segno () % atsci_trellis_encoder::NCODERS) == 0)
      break;
  }

  if (start == atsci_trellis_encoder::NCODERS){
    // we didn't find a mod 12 boundary.  There's some kind of problem
    // upstream of us (not yet sync'd??)
    cerr << "!!!GrAtscTrellisEncoder: no mod-12 boundary found\7\n";
    start = 0;
  }
  else if (start != last_start){
    cerr << "GrAtscTrellisEncoder: new starting offset = " << start
	 << " output.index = " << output.index << endl;
    last_start = start;
  }


  // FIXME paranoid check for problem
  for (unsigned int i = 0; i < output.size; i++){
    plinfo::sanity_check (in[i + start].pli);
  }

  // We must produce output.size units of output.

  for (unsigned int i = 0; i < output.size; i += atsci_trellis_encoder::NCODERS){
    // primitive does 12 segments at a time.
    // pipeline info is handled in the primitive.
    encoder.encode (&out[i], &in[i + start]);
  }

#if 0
  // FIXME paranoid check for problem
  for (unsigned int i = 0; i < output.size; i++){
    plinfo::sanity_check (out[i].pli);
    assert (out[i].pli.regular_seg_p ());
  }
#endif

  // Return the number of units we produced.
  // Note that for all intents and purposes, it is an error to
  // produce less than you are asked for.

  return output.size;
}

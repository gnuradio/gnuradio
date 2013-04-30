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

#include <gnuradio/atsc/GrAtscEqualizer.h>
#include <gnuradio/atsc/equalizer_impl.h>

// typedefs for fundamental i/o types

typedef float		dataType;
typedef atsc::syminfo	tagType;

static const int NUMBER_OF_OUTPUTS = 2;	// # of output streams


GrAtscEqualizer::GrAtscEqualizer (atsci_equalizer *equalizer)
  : VrHistoryProc<dataType,dataType> (NUMBER_OF_OUTPUTS)
{
  // due to limitation of runtime, all inputs must be the same size
  assert (sizeof (dataType) == sizeof (tagType));

  d_equalizer = equalizer;

  // 1 + number of extra input elements at which we look.  This is
  // used by the superclass's forecast routine to get us the correct
  // range on our inputs.
  //
  // Set this to the answer returned by the equalizer primitive we were passed.
  history = d_equalizer->ntaps ();
}

GrAtscEqualizer::~GrAtscEqualizer ()
{
  // Anything that isn't automatically cleaned up...

  delete d_equalizer;
}


/*
 * non-standard forecast routine that handles getting the correct amount of
 * history for the data input as well as ensuring correct alignment of
 * the data and tags.
 */

int
GrAtscEqualizer::forecast (VrSampleRange output,
			   VrSampleRange inputs[])
{
  assert (numberInputs == 2);

  int ntaps = d_equalizer->ntaps ();
  int npretaps = d_equalizer->npretaps ();

  assert (ntaps >= 1);
  assert (npretaps >= 0 && npretaps < ntaps);

  inputs[0].index = output.index;		// the equalizer data
  inputs[0].size  = output.size + ntaps - 1;	// history on data

  // FIXME if there's a problem, it's probably on the next line...
  int offset = ntaps - npretaps - 1;

  assert (offset >= 0 && offset < ntaps);

  inputs[1].index = output.index + offset; 	// align equalizer tags
  inputs[1].size = output.size;			// N.B., no extra history on tags

  return 0;
}

/*
 * This is the real work horse.  We consume 2 input streams
 * and produce 2 output streams.
 */

int
GrAtscEqualizer::work (VrSampleRange output, void *ao[],
		       VrSampleRange inputs[], void *ai[])
{
  // assert (numberInputs == 2);

  // If we have state that persists across invocations (e.g., we have
  // instance variables that we modify), we must use the sync method
  // to indicate to the scheduler that our output must be computed in
  // order.  This doesn't keep other things from being run in
  // parallel, it just means that at any given time, there is only a
  // single thread working this code, and that the scheduler will
  // ensure that we are asked to produce output that is contiguous and
  // that will be presented to us in order of increasing time.

  // We have state, hence we must use sync.

  sync (output.index);

  // construct some nicer i/o pointers to work with.

  dataType *input_samples  = ((dataType **) ai)[0];
  tagType  *input_tags     = ((tagType **)  ai)[1];
  dataType *output_samples = ((dataType **) ao)[0];
  tagType  *output_tags    = ((tagType **)  ao)[1];


  // peform the actual equalization

  d_equalizer->filter (input_samples, input_tags,
		       output_samples, output.size);

  // write the output tags

  for (unsigned int i = 0; i < output.size; i++)
    output_tags[i] = input_tags[i];

  // Return the number of units we produced.

  return output.size;
}

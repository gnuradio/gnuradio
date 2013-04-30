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

#include <gnuradio/atsc/GrAtscFieldSyncMux.h>
#include <gnuradio/atsc/pnXXX_impl.h>


// typedefs for fundamental i/o types

typedef atsc_data_segment	iType;
typedef atsc_data_segment	oType;

static const int NUMBER_OF_OUTPUTS = 1;	// # of output streams (almost always one)

static const int N_SAVED_SYMBOLS = GrAtscFieldSyncMux::N_SAVED_SYMBOLS;

static void
init_field_sync_common (unsigned char *p, int mask,
			const unsigned char saved_symbols[N_SAVED_SYMBOLS])
{
  static const unsigned char bin_map[2] = { 1, 6 };  // map binary values to 1 of 8 levels

  int  i = 0;

  p[i++] = bin_map[1];			// data segment sync pulse
  p[i++] = bin_map[0];
  p[i++] = bin_map[0];
  p[i++] = bin_map[1];

  for (int j = 0; j < 511; j++)		// PN511
    p[i++] = bin_map[atsc_pn511[j]];

  for (int j = 0; j < 63; j++)		// PN63
    p[i++] = bin_map[atsc_pn63[j]];

  for (int j = 0; j < 63; j++)		// PN63, toggled on field 2
    p[i++] = bin_map[atsc_pn63[j] ^ mask];

  for (int j = 0; j < 63; j++)		// PN63
    p[i++] = bin_map[atsc_pn63[j]];

  p[i++] = bin_map[0];			// 24 bits of VSB8 mode identifiera
  p[i++] = bin_map[0];
  p[i++] = bin_map[0];
  p[i++] = bin_map[0];

  p[i++] = bin_map[1];
  p[i++] = bin_map[0];
  p[i++] = bin_map[1];
  p[i++] = bin_map[0];

  p[i++] = bin_map[0];
  p[i++] = bin_map[1];
  p[i++] = bin_map[0];
  p[i++] = bin_map[1];

  p[i++] = bin_map[1];
  p[i++] = bin_map[1];
  p[i++] = bin_map[1];
  p[i++] = bin_map[1];

  p[i++] = bin_map[0];
  p[i++] = bin_map[1];
  p[i++] = bin_map[0];
  p[i++] = bin_map[1];

  p[i++] = bin_map[1];
  p[i++] = bin_map[0];
  p[i++] = bin_map[1];
  p[i++] = bin_map[0];


  for (int j = 0; j < 92; j++)		// 92 more bits
    p[i++] = bin_map[atsc_pn63[j % 63]];

  // now copy the last 12 symbols of the previous segment

  for (int j = 0; j < N_SAVED_SYMBOLS; j++)
    p[i++] = saved_symbols[j];

  assert (i == ATSC_DATA_SEGMENT_LENGTH);
}

inline static void
init_field_sync_1 (atsc_data_segment *s,
		   const unsigned char saved_symbols[N_SAVED_SYMBOLS])
{
  init_field_sync_common (&s->data[0], 0, saved_symbols);
}

inline static void
init_field_sync_2 (atsc_data_segment *s,
		   const unsigned char saved_symbols[N_SAVED_SYMBOLS])

{
  init_field_sync_common (&s->data[0], 1, saved_symbols);
}

static void
save_last_symbols (unsigned char saved_symbols[N_SAVED_SYMBOLS],
		   const atsc_data_segment &seg)
{
  for (int i = 0; i < N_SAVED_SYMBOLS; i++)
    saved_symbols[i] = seg.data[i + ATSC_DATA_SEGMENT_LENGTH - N_SAVED_SYMBOLS];
}


inline static bool
last_regular_seg_p (const plinfo &pli)
{
  return pli.regular_seg_p () && (pli.segno () == ATSC_DSEGS_PER_FIELD - 1);
}


GrAtscFieldSyncMux::GrAtscFieldSyncMux ()
  : VrHistoryProc<iType,oType> (NUMBER_OF_OUTPUTS),
    d_current_index (0), d_already_output_field_sync (false)
{
  // 1 + number of extra input elements at which we look.  This is
  // used by the superclass's forecast routine to get us the correct
  // range on our inputs.
  history = 1;

  // any other init here.
}

GrAtscFieldSyncMux::~GrAtscFieldSyncMux ()
{
  // Anything that isn't automatically cleaned up...
}

void
GrAtscFieldSyncMux::pre_initialize ()
{
  // we jack our output sampling frequency up to account for inserted field syncs
  setSamplingFrequency (getInputSamplingFrequencyN (0) * 313./312.);
}

/*
 * we need a non-standard version of forecast because our output isn't
 * exactly 1:1 with our input.
 */

int
GrAtscFieldSyncMux::forecast (VrSampleRange output, VrSampleRange inputs[])
{
  for(unsigned int i = 0; i < numberInputs; i++) {
    inputs[i].index = d_current_index;
    inputs[i].size = output.size;
  }
  return 0;
}

/*
 * This is the real work horse.  In general this interface can handle
 * multiple streams of input and output, but we almost always
 * use a single input and output stream.
 */
int
GrAtscFieldSyncMux::work (VrSampleRange output, void *ao[],
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

  // We have state, hence we must use sync.

  sync (output.index);

  // construct some nicer i/o pointers to work with.

  iType *in  = ((iType **) ai)[0];
  oType *out = ((oType **) ao)[0];


  // We must produce output.size units of output.

  unsigned int index = 0;
  for (unsigned int outdex = 0; outdex < output.size; outdex++){

    assert (in[index].pli.regular_seg_p ());

    if (!in[index].pli.first_regular_seg_p ()){
      out[outdex] = in[index];			// just copy in to out

      if (last_regular_seg_p (in[index].pli))
	save_last_symbols (d_saved_symbols, in[index]);

      index++;
    }
    else {					// first_regular_seg_p
      if (!d_already_output_field_sync){
	// write out field sync...
	atsc_data_segment	field_sync;

	if (in[index].pli.in_field1_p ())
	  init_field_sync_1 (&field_sync, d_saved_symbols);
	else
	  init_field_sync_2 (&field_sync, d_saved_symbols);

	// note that index doesn't advance in this branch
	out[outdex] = field_sync;
	d_already_output_field_sync = true;
      }
      else {
	// already output field sync, now output first regular segment
	out[outdex] = in[index];
	index++;
	d_already_output_field_sync = false;
      }
    }
  }

  d_current_index += index;

  // Return the number of units we produced.
  // Note that for all intents and purposes, it is an error to
  // produce less than you are asked for.

  return output.size;
}

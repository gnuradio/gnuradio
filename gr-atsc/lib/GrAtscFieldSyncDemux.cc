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
#include <gnuradio/atsc/GrAtscFieldSyncDemux.h>
#include <gnuradio/atsc/consts.h>
#include <gnuradio/atsc/types.h>
#include <gnuradio/atsc/syminfo_impl.h>
#include <stdio.h>
#include <assert.h>

using std::abs;

static const int	DEC = ATSC_DATA_SEGMENT_LENGTH;	// nominal decimation factor

GrAtscFieldSyncDemux::GrAtscFieldSyncDemux ()
  : VrDecimatingSigProc<float,atsc_soft_data_segment> (1, DEC),
    d_locked (false), d_in_field2(true), d_segment_number(0), d_next_input(0),
    d_lost_index (0)
{
  history = 2 * ATSC_DATA_SEGMENT_LENGTH;  // spare input samples in case we need them.
}

GrAtscFieldSyncDemux::~GrAtscFieldSyncDemux ()
{
}

int
GrAtscFieldSyncDemux::forecast (VrSampleRange output,
				VrSampleRange inputs[]) {
  /* dec:1 ratio with history */

  assert (numberInputs == 2);

  for (unsigned int i = 0; i < numberInputs; i++) {
    inputs[i].index = d_next_input;
    inputs[i].size = output.size * decimation + history - 1;
  }
  return 0;
}

inline static bool
tag_is_seg_sync_or_field_sync (atsc::syminfo tag)
{
  return tag.symbol_num == 0 && tag.valid;
}

int
GrAtscFieldSyncDemux::work (VrSampleRange output, void *ao[],
			    VrSampleRange inputs[], void *ai[])
{
  float         *input_samples = (float *) ai[0];
  atsc::syminfo *input_tags    = (atsc::syminfo *) ai[1];
  atsc_soft_data_segment  *out = ((atsc_soft_data_segment **)ao)[0];

  sync (output.index);

  unsigned int	ii = 0;		// input index

  // Are we in sync?
  if (!tag_is_seg_sync_or_field_sync (input_tags[0])){	    // No ...

    if (d_locked){
      d_locked = false;
      d_lost_index = inputs[0].index + ii;
      cerr << "GrAtscFieldSyncDemux: lost sync at  "
	   << d_lost_index << endl;
    }

    // ... search for beginning of a field sync

    // cerr << "GrAtscFieldSyncDemux: searching for sync at "
    //      << inputs[0].index + ii << endl;

    for (ii = 1; ii < inputs[0].size; ii++){
      if (atsc::tag_is_start_field_sync (input_tags[ii])){
	// found one
	d_locked = true;

	const char *str;
	if (atsc::tag_is_start_field_sync_1 (input_tags[ii]))
	  str = "FIELD-1";
	else if (atsc::tag_is_start_field_sync_2 (input_tags[ii]))
	  str = "FIELD-2";
	else
	  str = "SEGMENT";

	cerr << "GrAtscFieldSyncDemux: synced (" << str << ") at "
	     << inputs[0].index + ii
	     << " [delta = " << inputs[0].index + ii - d_lost_index
	     << "]\n";

	d_next_input += ii;	// update for forecast
	return 0;		// no work completed so far
      }
    }
    // no non-NORMAL tag found
    d_next_input += ii;		// update for forecast
    return 0;			// no work completed so far
  }

  // We are in sync.  Produce output...

  unsigned int	k = 0;		// output index

  while (k < output.size){

    if (inputs[0].size - ii < (unsigned) ATSC_DATA_SEGMENT_LENGTH){
      // We're out of input data.
      cerr << "GrAtscFieldSyncDemux: ran out of input data\n";
      d_next_input += ii;	// update for forecast
      return k;			// return amount of work completed so far
    }

    if (!tag_is_seg_sync_or_field_sync (input_tags[ii])){
      // lost sync...
      // cerr << "GrAtscFieldSyncDemux: lost sync at "
      //    << inputs[0].index + ii << endl;

      d_next_input += ii;	// update for forecast
      return k;			// return amount of work completed so far
    }

    if (atsc::tag_is_start_field_sync_1 (input_tags[ii])){
      d_in_field2 = false;
      d_segment_number = 0;
      ii += ATSC_DATA_SEGMENT_LENGTH;	// skip over field sync
      continue;
    }

    if (atsc::tag_is_start_field_sync_2 (input_tags[ii])){
      d_in_field2 = true;
      d_segment_number = 0;
      ii += ATSC_DATA_SEGMENT_LENGTH;	// skip over field sync
      continue;
    }

    if (d_segment_number >= ATSC_DSEGS_PER_FIELD){
      // something's wrong...
      cerr << "GrAtscFieldSyncDemux: segment number overflow\n";
      d_segment_number = 0;
    }

    out[k].pli.set_regular_seg (d_in_field2, d_segment_number++);
    for (int jj = 0; jj < ATSC_DATA_SEGMENT_LENGTH; jj++)
      out[k].data[jj] = input_samples[ii + jj];
    ii += ATSC_DATA_SEGMENT_LENGTH;
    k++;
  }

  d_next_input += ii;		// update for forecast
  return k;			// return amount of work completed
}


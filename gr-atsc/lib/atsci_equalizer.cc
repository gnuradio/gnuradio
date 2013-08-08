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

#include <gnuradio/atsc/equalizer_impl.h>
#include <algorithm>
#include <iostream>
#include <gnuradio/atsc/types.h>

using std::cerr;
using std::endl;
using std::min;


// total number of symbols (including field sync) / field
static const int SYMBOLS_PER_FIELD =
  (ATSC_DSEGS_PER_FIELD + 1) * ATSC_DATA_SEGMENT_LENGTH;


atsci_equalizer::atsci_equalizer ()
{
  d_locked_p = false;
  d_offset_from_last_field_sync = 0;
  d_current_field = 0;
}

atsci_equalizer::~atsci_equalizer ()
{
}

void
atsci_equalizer::reset ()
{
  d_locked_p = false;
  d_offset_from_last_field_sync = 0;
  d_current_field = 0;
}

/*
 * Errrr.... Define to 1 if compiler handles tail recursion without pushing
 * unnecessary stack frames, else define to 0 for lame compilers.
 */
#define	WINNING_COMPILER	0


/*
 * divide and conquer...
 *
 * Note that this could be refactored to take advantage of the
 * symbol_num that is contained in the input_tags.  Then we wouldn't
 * have to be counting here.
 *
 * Today's strategy: get it working.
 */

void
atsci_equalizer::filter (const float         *input_samples,
			const atsc::syminfo *input_tags,
			float               *output_samples,
			int                  nsamples)
{
 lame_compiler_kludge:

  if (!d_locked_p){

    // look for a field sync

    int	i;
    for (i = 0; i < nsamples; i++){
      if (atsc::tag_is_start_field_sync (input_tags[i]))
	break;
    }

    // whether we found one or not, everything up to it should
    // be run through the normal path

    if (i != 0)
      filter_normal (input_samples, output_samples, i);

    if (i == nsamples)		// no field sync found, still not locked.
      return;

    // OK, we've just transitioned to the locked state.

    d_locked_p = true;
    d_offset_from_last_field_sync = 0;

    // handle locked case recursively

    if (WINNING_COMPILER)
      filter (&input_samples[i], &input_tags[i],
	      &output_samples[i], nsamples - i);
    else {
      input_samples += i;
      input_tags += i;
      output_samples += i;
      nsamples -= i;
      goto lame_compiler_kludge;
    }

    return;
  }

  // We're in the locked state.
  //
  // Figure out where we are with respect to a data segment boundary
  // and do the right thing.  Note that in the interested of performance,
  // we don't scan all the tags looking for trouble.  We only check
  // them where we expect them to be non-NORMAL.  Worst case, it'll take
  // us a field to notice that something went wrong...

  if (d_offset_from_last_field_sync % SYMBOLS_PER_FIELD == 0){	// we should be looking
                                                                //   at a field sync
    if (atsc::tag_is_start_field_sync_1 (input_tags[0]))
      d_current_field = 0;

    else if (atsc::tag_is_start_field_sync_2 (input_tags[0]))
      d_current_field = 1;

    else {	// we're lost... no field sync where we expected it

      cerr << "!!! atsci_equalizer: expected field sync, didn't find one\n";

      d_locked_p = false;
      d_offset_from_last_field_sync = 0;

      if (WINNING_COMPILER)
	filter (input_samples, input_tags, output_samples, nsamples);
      else
	goto lame_compiler_kludge;

      return;
    }

    // OK, everything's cool.  We're looking at a field sync.

    int n = min (ATSC_DATA_SEGMENT_LENGTH, nsamples);

    filter_field_sync (input_samples, output_samples, n, 0, d_current_field);

    d_offset_from_last_field_sync = n;
    nsamples -= n;

    if (nsamples > 0){
      if (WINNING_COMPILER)
	filter (&input_samples[n], &input_tags[n],
		&output_samples[n], nsamples);
      else {
	input_samples += n;
	input_tags += n;
	output_samples += n;
	goto lame_compiler_kludge;
      }
    }
    return;
  }

  if (d_offset_from_last_field_sync < ATSC_DATA_SEGMENT_LENGTH){ // we're in the middle of a field sync
    int n = min (ATSC_DATA_SEGMENT_LENGTH - d_offset_from_last_field_sync, nsamples);

    filter_field_sync (input_samples, output_samples, n,
		       d_offset_from_last_field_sync, d_current_field);

    d_offset_from_last_field_sync += n;
    nsamples -= n;

    if (nsamples > 0){
      if (WINNING_COMPILER)
	filter (&input_samples[n], &input_tags[n],
		&output_samples[n], nsamples);
      else {
	input_samples += n;
	input_tags += n;
	output_samples += n;
	goto lame_compiler_kludge;
      }
    }
    return;
  }

  // OK, we're not in a field sync.  We're either in a data segment sync or in the clear...

  int seg_offset = d_offset_from_last_field_sync % ATSC_DATA_SEGMENT_LENGTH;

  assert (seg_offset >= 0);

  if (seg_offset < 4){	// somewhere in a data seg sync.
    int n = min (4 - seg_offset, nsamples);

    filter_data_seg_sync (input_samples, output_samples, n, seg_offset);

    d_offset_from_last_field_sync += n;
    nsamples -= n;

    if (nsamples > 0){
      if (WINNING_COMPILER)
	filter (&input_samples[n], &input_tags[n],
		&output_samples[n], nsamples);
      else {
	input_samples += n;
	input_tags += n;
	output_samples += n;
	goto lame_compiler_kludge;
      }
    }
    return;
  }

  // otherwise... we're in the normal zone

  int n = min (ATSC_DATA_SEGMENT_LENGTH - seg_offset, nsamples);

  filter_normal (input_samples, output_samples, n);

  d_offset_from_last_field_sync += n;
  nsamples -= n;

  if (nsamples <= 0)
    return;

  if (WINNING_COMPILER)
    filter (&input_samples[n], &input_tags[n],
	    &output_samples[n], nsamples);
  else {
    input_samples += n;
    input_tags += n;
    output_samples += n;
    goto lame_compiler_kludge;
  }
}

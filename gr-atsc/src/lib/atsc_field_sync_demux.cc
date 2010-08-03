/* -*- c++ -*- */
/*
 * Copyright 2006,2010 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cmath>
#include <atsc_field_sync_demux.h>
#include <gr_io_signature.h>
#include <atsc_types.h>
#include <atsc_consts.h>
#include <atsci_syminfo.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>

using std::cerr;
using std::endl;


static const int        DEC = ATSC_DATA_SEGMENT_LENGTH; // nominal decimation factor


atsc_field_sync_demux_sptr
atsc_make_field_sync_demux()
{
  return gnuradio::get_initial_sptr(new atsc_field_sync_demux());
}

atsc_field_sync_demux::atsc_field_sync_demux()
  : gr_block("atsc_field_sync_demux",
		  gr_make_io_signature(2, 2, sizeof(float)),
		  gr_make_io_signature(1, 1, sizeof(atsc_soft_data_segment))),
		  d_locked(false), d_in_field2(true), d_segment_number(0),
		  d_next_input(0), d_lost_index(0), d_inputs0_index(0), 
                  d_inputs0_size(0), d_consume(0)
{
  reset();
}

inline static bool
tag_is_seg_sync_or_field_sync (atsc::syminfo tag)
{
  return tag.symbol_num == 0 && tag.valid;
}

void
atsc_field_sync_demux::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned ninputs = ninput_items_required.size();
  for (unsigned i = 0; i < ninputs; i++) {
    ninput_items_required[i] = noutput_items * DEC + 2 * DEC ;

  d_inputs0_index = d_next_input;
  d_inputs0_size = noutput_items * DEC + 2 * DEC ;
  }
}

int
atsc_field_sync_demux::general_work (int noutput_items,
                                 gr_vector_int &ninput_items,
                                 gr_vector_const_void_star &input_items,
                                 gr_vector_void_star &output_items)
{
  int   r = work (noutput_items, input_items, output_items);
    consume_each (d_consume);
    // printf("Consumed: %d, produced: %d\n",d_consume,r);
    // we consume input even if no output is produced
    // while looking for sync
  return r;
}


int
atsc_field_sync_demux::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  float *in = (float *) input_items[0];
  atsc::syminfo *input_tags    = (atsc::syminfo *) input_items[1];
  atsc_soft_data_segment *out = (atsc_soft_data_segment *) output_items[0];

  assert(sizeof(float) == sizeof(atsc::syminfo));

  unsigned int  ii = 0;         // input index

  // Are we in sync?
  if (!tag_is_seg_sync_or_field_sync (input_tags[0])){      // No ...

    if (d_locked){
      d_locked = false;
      d_lost_index = d_inputs0_index + ii;
      cerr << "atsc_field_sync_demux: lost sync at  "
           << d_lost_index << endl;
    }

    // ... search for beginning of a field sync

    // cerr << "atsc_field_sync_demux: searching for sync at "
    //      << d_inputs0_index + ii << endl;

    for (ii = 1; ii < d_inputs0_size; ii++){
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

        cerr << "atsc_field_sync_demux: synced (" << str << ") at "
             << d_inputs0_index + ii
             << " [delta = " << d_inputs0_index + ii - d_lost_index
             << "]\n";

        d_next_input += ii;     // update for forecast
	d_consume = ii;
        return 0;               // no work completed so far
      }
    }
    // no non-NORMAL tag found
    d_next_input += ii;         // update for forecast
    d_consume = ii;
    // printf("ii: %d, d_next_input: %d\n",ii,d_next_input);
    return 0;                   // no work completed so far
  }

  // We are in sync.  Produce output...

  int  k = 0;          // output index

  while (k < noutput_items){

    if (d_inputs0_size - ii <  static_cast<unsigned int>(ATSC_DATA_SEGMENT_LENGTH)){
      // We're out of input data.
      cerr << "atsc_field_sync_demux: ran out of input data\n";
      d_next_input += ii;       // update for forecast
      return k;                 // return amount of work completed so far
    }

    if (!tag_is_seg_sync_or_field_sync (input_tags[ii])){
      // lost sync...
      cerr << "atsc_field_sync_demux: lost sync at "
           << d_inputs0_index + ii << endl;

      d_next_input += ii;       // update for forecast
      return k;                 // return amount of work completed so far
    }

    if (atsc::tag_is_start_field_sync_1 (input_tags[ii])){
      d_in_field2 = false;
      d_segment_number = 0;
      ii += ATSC_DATA_SEGMENT_LENGTH;   // skip over field sync
      continue;
    }

    if (atsc::tag_is_start_field_sync_2 (input_tags[ii])){
      d_in_field2 = true;
      d_segment_number = 0;
      ii += ATSC_DATA_SEGMENT_LENGTH;   // skip over field sync
      continue;
    }

    if (d_segment_number >= ATSC_DSEGS_PER_FIELD){
      // something's wrong...
      cerr << "atsc_field_sync_demux: segment number overflow\n";
      d_segment_number = 0;
    }

    out[k].pli.set_regular_seg (d_in_field2, d_segment_number++);
    for (int jj = 0; jj < ATSC_DATA_SEGMENT_LENGTH; jj++)
      out[k].data[jj] = in[ii + jj];
    ii += ATSC_DATA_SEGMENT_LENGTH;
    k++;
  }

  d_next_input += ii;           // update for forecast
  d_consume = ii;
  return k;                     // return amount of work completed

}





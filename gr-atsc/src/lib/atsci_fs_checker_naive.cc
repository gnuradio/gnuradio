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

#include <atsci_fs_checker_naive.h>
#include <atsci_syminfo.h>
#include <atsci_pnXXX.h>
#include <iostream>
#include <cstring>

using std::cerr;
using std::endl;

static const int PN511_ERROR_LIMIT = 20;	// max number of bits wrong
static const int PN63_ERROR_LIMIT =   5;

unsigned char atsci_fs_checker_naive::s_511[LENGTH_511];
unsigned char atsci_fs_checker_naive::s_63[LENGTH_2ND_63];

static void
init_s_511 (unsigned char *p)
{
  *p++ = 1;	// data segment sync pattern
  *p++ = 0;
  *p++ = 0;
  *p++ = 1;

  for (int i = 0; i < 511; i++){
    p[i] = atsc_pn511[i];
  }
}

static void
init_s_63 (unsigned char *p)
{
  for (int i = 0; i < 63; i++){
    p[i] = atsc_pn63[i];
  }
}

atsci_fs_checker_naive::atsci_fs_checker_naive ()
{
  init_s_511 (s_511);
  init_s_63 (s_63);
  reset ();
}

atsci_fs_checker_naive::~atsci_fs_checker_naive ()
{
}

void
atsci_fs_checker_naive::reset ()
{
  d_index = 0;
  memset (d_sample_sr, 0, sizeof (d_sample_sr));
  memset (d_tag_sr, 0, sizeof (d_tag_sr));
  memset (d_bit_sr, 0, sizeof (d_bit_sr));
  d_field_num = 0;
  d_segment_num = 0;
}

void
atsci_fs_checker_naive::filter (float input_sample, atsc::syminfo input_tag,
			       float *output_sample, atsc::syminfo *output_tag)
{
  atsc::syminfo	proto_tag = d_tag_sr[d_index];	// oldest tag in the queue

  if (proto_tag.symbol_num == 0){		// check for field sync pattern
    
    d_segment_num = (d_segment_num + 1) & atsc::SI_SEGMENT_NUM_MASK;  // increment

    // check for a hit on the PN 511 pattern
    int	errors = 0;
    int	start = wrap (d_index + OFFSET_511);

    for (int i = 0; i < LENGTH_511 && errors < PN511_ERROR_LIMIT; i++)
      errors += d_bit_sr[wrap (start + i)] ^ s_511[i];

    if (errors < PN511_ERROR_LIMIT){	// 511 pattern is good.
				        // determine if this is field 1 or field 2
      errors = 0;
      start = wrap (d_index + OFFSET_2ND_63);
      for (int i = 0; i < LENGTH_2ND_63; i++)
	errors += d_bit_sr[wrap (start + i)] ^ s_63[i];

      // we should have either field 1 (== PN63) or field 2 (== ~PN63)

      if (errors <= PN63_ERROR_LIMIT){
	d_segment_num = atsc::SI_FIELD_SYNC_SEGMENT_NUM;	// this is FIELD_SYNC_1
	d_field_num = 0;
      }
      else if (errors >= (LENGTH_2ND_63 - PN63_ERROR_LIMIT)){
	d_segment_num = atsc::SI_FIELD_SYNC_SEGMENT_NUM;	// this is FIELD_SYNC_2
	d_field_num = 1;
      }
      else {
	// should be extremely rare.
	cerr << "!!! atsci_fs_checker_naive: PN63 error count = " << errors << endl;
      }
    }
  }

  proto_tag.segment_num = d_segment_num;	// fill in segment number and field number	
  proto_tag.field_num = d_field_num;

  // return oldest sample
  *output_sample = d_sample_sr[d_index];
  *output_tag    = proto_tag;

  // overwrite with newest sample;
  d_sample_sr[d_index] = input_sample;
  d_bit_sr[d_index] = input_sample < 0 ? 0 : 1;
  d_tag_sr[d_index] = input_tag;
  d_index = incr (d_index);
}

int
atsci_fs_checker_naive::delay () const
{
  return SRSIZE;
}

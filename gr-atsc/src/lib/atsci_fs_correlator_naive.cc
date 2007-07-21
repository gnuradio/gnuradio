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

#include <atsci_fs_correlator_naive.h>
#include <atsci_sync_tag.h>
#include <atsci_pnXXX.h>
#include <iostream>
#include <cstring>

using std::cerr;
using std::endl;

static const int PN511_ERROR_LIMIT = 20;	// max number of bits wrong
static const int PN63_ERROR_LIMIT =   5;

unsigned char atsci_fs_correlator_naive::s_511[LENGTH_511];
unsigned char atsci_fs_correlator_naive::s_63[LENGTH_2ND_63];

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

atsci_fs_correlator_naive::atsci_fs_correlator_naive ()
{
  init_s_511 (s_511);
  init_s_63 (s_63);
  reset ();
}

atsci_fs_correlator_naive::~atsci_fs_correlator_naive ()
{
}

void
atsci_fs_correlator_naive::reset ()
{
  d_index = 0;
  memset (d_sample_sr, 0, sizeof (d_sample_sr));
  memset (d_bit_sr, 0, sizeof (d_bit_sr));
}

void
atsci_fs_correlator_naive::filter (float input_sample,
				  float *output_sample, float *output_tag)
{
  // check for a hit on the PN 511 pattern
  int	errors = 0;
  int	start = wrap (d_index + OFFSET_511);

  for (int i = 0; i < LENGTH_511 && errors < PN511_ERROR_LIMIT; i++)
    errors += d_bit_sr[wrap (start + i)] ^ s_511[i];

  if (errors >= PN511_ERROR_LIMIT)
    *output_tag = atsc_sync_tag::NORMAL;

  else {	// 511 pattern is good.  determine if this is field 1 or field 2
    errors = 0;
    start = wrap (d_index + OFFSET_2ND_63);
    for (int i = 0; i < LENGTH_2ND_63; i++)
      errors += d_bit_sr[wrap (start + i)] ^ s_63[i];

    // we should have either field 1 (== PN63) or field 2 (== ~PN63)
    if (errors <= PN63_ERROR_LIMIT)
      *output_tag = atsc_sync_tag::START_FIELD_SYNC_1;

    else if (errors >= (LENGTH_2ND_63 - PN63_ERROR_LIMIT))
      *output_tag = atsc_sync_tag::START_FIELD_SYNC_2;

    else {
      // should be extremely rare.
      cerr << "!!! atsci_fs_correlator_naive: PN63 error count = " << errors << endl;
      *output_tag = atsc_sync_tag::NORMAL;
    }
  }

  // return oldest sample
  *output_sample = d_sample_sr[d_index];

  // overwrite with newest sample;
  d_sample_sr[d_index] = input_sample;
  d_bit_sr[d_index] = input_sample < 0 ? 0 : 1;
  d_index = incr (d_index);
}

int
atsci_fs_correlator_naive::delay () const
{
  return SRSIZE;
}

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

#include "qa_atsci_equalizer_nop.h"
#include <gnuradio/atsc/equalizer_impl.h>
#include <gnuradio/atsc/equalizer_nop_impl.h>
#include <gnuradio/atsc/pnXXX_impl.h>
#include <gnuradio/atsc/types.h>
#include <cppunit/TestAssert.h>
#include <assert.h>
#include <iostream>
#include <string.h>

using std::cerr;
using std::endl;


static const int SYMBOLS_PER_FIELD = (ATSC_DSEGS_PER_FIELD + 1) * ATSC_DATA_SEGMENT_LENGTH;
static const int PAD = 500;
static const int NFIELDS = 2;

static const int INPUT_SIZE = PAD + (NFIELDS * SYMBOLS_PER_FIELD);

static float
bin_map (int bit)
{
  return bit ? +5 : -5;
}

static void
init_tags (atsc::syminfo *tags, int segnum, int fieldnum)
{
  int	i;

  for (i = 0; i < ATSC_DATA_SEGMENT_LENGTH; i++){
    tags[i].symbol_num = i;
    tags[i].segment_num = segnum;
    tags[i].field_num = fieldnum;
    tags[i].valid = 1;
  }
}

static void
init_data_seg (float *p, atsc::syminfo *tags, int v, int segnum, int fieldnum)
{
  init_tags (tags, segnum, fieldnum);

  int	i = 0;

  p[i++] = bin_map (1);			// data segment sync pulse
  p[i++] = bin_map (0);
  p[i++] = bin_map (0);
  p[i++] = bin_map (1);

  for (; i < ATSC_DATA_SEGMENT_LENGTH; i++)
    p[i] = (float) (i + v);
}

static void
init_field_sync_tags (atsc::syminfo *tags, int fieldnum)
{
  init_tags (tags, atsc::SI_FIELD_SYNC_SEGMENT_NUM, fieldnum);
}

static void
init_field_sync_common (float *p, int mask)

{
  int  i = 0;

  p[i++] = bin_map (1);			// data segment sync pulse
  p[i++] = bin_map (0);
  p[i++] = bin_map (0);
  p[i++] = bin_map (1);

  for (int j = 0; j < 511; j++)		// PN511
    p[i++] = bin_map (atsc_pn511[j]);

  for (int j = 0; j < 63; j++)		// PN63
    p[i++] = bin_map (atsc_pn63[j]);

  for (int j = 0; j < 63; j++)		// PN63, toggled on field 2
    p[i++] = bin_map (atsc_pn63[j] ^ mask);

  for (int j = 0; j < 63; j++)		// PN63
    p[i++] = bin_map (atsc_pn63[j]);

  p[i++] = bin_map (0);			// 24 bits of VSB8 mode identifiera
  p[i++] = bin_map (0);
  p[i++] = bin_map (0);
  p[i++] = bin_map (0);

  p[i++] = bin_map (1);
  p[i++] = bin_map (0);
  p[i++] = bin_map (1);
  p[i++] = bin_map (0);

  p[i++] = bin_map (0);
  p[i++] = bin_map (1);
  p[i++] = bin_map (0);
  p[i++] = bin_map (1);

  p[i++] = bin_map (1);
  p[i++] = bin_map (1);
  p[i++] = bin_map (1);
  p[i++] = bin_map (1);

  p[i++] = bin_map (0);
  p[i++] = bin_map (1);
  p[i++] = bin_map (0);
  p[i++] = bin_map (1);

  p[i++] = bin_map (1);
  p[i++] = bin_map (0);
  p[i++] = bin_map (1);
  p[i++] = bin_map (0);


  for (int j = 0; j < 92; j++)		// 92 more bits
    p[i++] = bin_map (atsc_pn63[j % 63]);

  // now copy the last 12 symbols of the previous segment
  // bogus fill

  for (int j = 0; j < 12; j++)
    p[i++] = bin_map (j & 1);

  assert (i == ATSC_DATA_SEGMENT_LENGTH);
}

void
qa_atsci_equalizer_nop::setUp ()
{
  eq = new atsci_equalizer_nop ();
}

void
qa_atsci_equalizer_nop::tearDown ()
{
  delete eq;
  eq = 0;
}

static void
setup_test_data (float *data, atsc::syminfo *tags)
{
  int	mask = 0;
  int	i = 0;

  for (i = 0; i < PAD; i++){
    data[i] = (float) i;
    tags[i].symbol_num = 13 + i;
    tags[i].segment_num = 17;
    tags[i].field_num = 0;
    tags[i].valid = 1;
  }

  for (int nfields = 0; nfields < NFIELDS; nfields++){
    init_field_sync_common (&data[i], mask);
    init_field_sync_tags (&tags[i], mask);
    i += ATSC_DATA_SEGMENT_LENGTH;

    for (int segnum = 0; segnum < 312; segnum++){
      init_data_seg (&data[i], &tags[i], i, segnum, mask);
      i += ATSC_DATA_SEGMENT_LENGTH;
    }
    mask ^= 1;
  }
  assert (i == INPUT_SIZE);
}

void
qa_atsci_equalizer_nop::t0 ()
{
  // these are dynamically allocated because they are bigger
  // than the default stack limit.

  float         *input_data = new float[INPUT_SIZE];
  atsc::syminfo *input_tags = new atsc::syminfo[INPUT_SIZE];
  float         *output_data = new float[INPUT_SIZE];

  try {

    memset (input_data, 0, sizeof(*input_data)*INPUT_SIZE);
    memset (input_tags, 0, sizeof(*input_tags)*INPUT_SIZE);
    memset (output_data, 0, sizeof(*output_data)*INPUT_SIZE);

    setup_test_data (input_data, input_tags);

    eq->filter (input_data, input_tags,
		output_data, INPUT_SIZE);


    // now check that data values got copied correctly

    for (int i = 0; i < INPUT_SIZE; i++){

      if (input_tags[i].segment_num == atsc::SI_FIELD_SYNC_SEGMENT_NUM){
	// ignore entire field sync data segment
      }
      else if (input_tags[i].symbol_num <= 3){
	// ignore 4 symbols of data segment sync (+5, -5, -5, +5)
      }
      else {
	if (output_data[i] != (float) i){
	  cerr << "output_data[" << i << "] == " << output_data[i] << endl;
	  CPPUNIT_ASSERT_DOUBLES_EQUAL ((float) i, output_data[i], 1e-6);
	}
      }
    }

    delete [] input_data;
    delete [] input_tags;
    delete [] output_data;
  }

  catch ( ... ){
    delete [] input_data;
    delete [] input_tags;
    delete [] output_data;
  }
}

void
qa_atsci_equalizer_nop::t1 ()
{
  // think of another test...
}

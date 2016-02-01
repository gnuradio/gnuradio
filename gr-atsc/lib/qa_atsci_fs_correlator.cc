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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "qa_atsci_fs_correlator.h"
#include <gnuradio/atsc/fs_correlator_impl.h>
#include <gnuradio/atsc/create_atsci_fs_correlator.h>
#include <gnuradio/atsc/sync_tag_impl.h>
#include <stdlib.h>
#include <algorithm>
#include <gnuradio/atsc/pnXXX_impl.h>
#include <gnuradio/atsc/types.h>
#include <cppunit/TestAssert.h>
#include <assert.h>
#include <gnuradio/random.h>

static gr::random rndm;

static float
uniform ()
{
  return 2.0 * (rndm.ran1() - 0.5);	// uniformly (-1, 1)
}


static float
bin_map (int bit)
{
  return bit ? +5 : -5;
}

static void
init_field_sync_common (float *p, int mask)

{
  int  i = 0;

  p[i++] = bin_map(1);			// data segment sync pulse
  p[i++] = bin_map(0);
  p[i++] = bin_map(0);
  p[i++] = bin_map(1);

  for (int j = 0; j < 511; j++)		// PN511
    p[i++] = bin_map(atsc_pn511[j]);

  for (int j = 0; j < 63; j++)		// PN63
    p[i++] = bin_map(atsc_pn63[j]);

  for (int j = 0; j < 63; j++)		// PN63, toggled on field 2
    p[i++] = bin_map(atsc_pn63[j] ^ mask);

  for (int j = 0; j < 63; j++)		// PN63
    p[i++] = bin_map(atsc_pn63[j]);

  p[i++] = bin_map(0);			// 24 bits of VSB8 mode identifiera
  p[i++] = bin_map(0);
  p[i++] = bin_map(0);
  p[i++] = bin_map(0);

  p[i++] = bin_map(1);
  p[i++] = bin_map(0);
  p[i++] = bin_map(1);
  p[i++] = bin_map(0);

  p[i++] = bin_map(0);
  p[i++] = bin_map(1);
  p[i++] = bin_map(0);
  p[i++] = bin_map(1);

  p[i++] = bin_map(1);
  p[i++] = bin_map(1);
  p[i++] = bin_map(1);
  p[i++] = bin_map(1);

  p[i++] = bin_map(0);
  p[i++] = bin_map(1);
  p[i++] = bin_map(0);
  p[i++] = bin_map(1);

  p[i++] = bin_map(1);
  p[i++] = bin_map(0);
  p[i++] = bin_map(1);
  p[i++] = bin_map(0);


  for (int j = 0; j < 92; j++)		// 92 more bits
    p[i++] = bin_map(atsc_pn63[j % 63]);

  // now copy the last 12 symbols of the previous segment
  // bogus pad for this test...

  for (int j = 0; j < 12; j++)
    p[i++] = bin_map(j & 1);

  assert (i == ATSC_DATA_SEGMENT_LENGTH);
}

inline static void
init_field_sync_1 (float *s)
{
  init_field_sync_common (s, 0);
}

inline static void
init_field_sync_2 (float *s)
{
  init_field_sync_common (s, 1);
}

static void
cause_errors (float *p, int nerrs1, int nerrs2)
{
  static const int offset1 =   4;	// offset to PN 511
  static const int offset2 = 578;	// offset to 2nd PN 63

  for (int i = 0; i < nerrs1; i++){	// flip nerrs1 bits in PN 511
    p[i + offset1] = -p[i + offset1];
  }

  for (int i = 0; i < nerrs2; i++){	// flip nerrs2 bits in PN 63
    p[i + offset2] = -p[i + offset2];
  }
}


void
qa_atsci_fs_correlator::setUp ()
{
  fsc = create_atsci_fs_correlator ();
}

void
qa_atsci_fs_correlator::tearDown ()
{
  delete fsc;
  fsc = 0;
}


// check sample fifo

void
qa_atsci_fs_correlator::t0 ()
{
  int	delay = fsc->delay ();
  int	i;
  float	output_sample, output_tag;

  for (i = 0; i < delay; i++){
    fsc->filter ((float) i, &output_sample, &output_tag);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0, output_sample, 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (atsc_sync_tag::NORMAL, output_tag, 1e-6);
  }

  for (; i < delay + 5000; i++){
    fsc->filter ((float) i, &output_sample, &output_tag);
    CPPUNIT_ASSERT_DOUBLES_EQUAL ((float) (i - delay), output_sample, 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (atsc_sync_tag::NORMAL, output_tag, 1e-6);
  }
}

void
qa_atsci_fs_correlator::util (int which_field, int nerrs1, int nerrs2)
{
  static const int PAD = 303;
  static const int ISIZE = 4096;
  int	delay = fsc->delay ();
  float	output_sample, output_tag;
  int	i;
  float	input[ISIZE];

  fsc->reset ();	// known starting conditions

  // build input

  for (i = 0; i < PAD; i++)
    input[i] = uniform () * 7;

  init_field_sync_common (&input[i], which_field);
  cause_errors (&input[i], nerrs1, nerrs2);
  i += ATSC_DATA_SEGMENT_LENGTH;

  for (; i < ISIZE; i++)
    input[i] = uniform () * 7;

  // run the input and check

  for (i = 0; i < ISIZE; i++){
    fsc->filter (input[i], &output_sample, &output_tag);
    if (i == delay + PAD){	// should be field sync
      if (which_field == 0)
	CPPUNIT_ASSERT_DOUBLES_EQUAL (atsc_sync_tag::START_FIELD_SYNC_1, output_tag, 1e-6);
      else
	CPPUNIT_ASSERT_DOUBLES_EQUAL (atsc_sync_tag::START_FIELD_SYNC_2, output_tag, 1e-6);
    }
    else {
      CPPUNIT_ASSERT_DOUBLES_EQUAL (atsc_sync_tag::NORMAL, output_tag, 1e-6);
    }
  }
}


void
qa_atsci_fs_correlator::t1 ()
{
  util (0, 0, 0);
}

void
qa_atsci_fs_correlator::t2 ()
{
  util (1, 0, 0);
}

void
qa_atsci_fs_correlator::t3 ()
{
  for (int nerrs1 = 0; nerrs1 < 20; nerrs1++){
    for (int nerrs2 = 0; nerrs2 < 5; nerrs2++){
      util (0, nerrs1, nerrs2);
    }
  }
}

void
qa_atsci_fs_correlator::t4 ()
{
  for (int nerrs1 = 0; nerrs1 < 5; nerrs1++){
    for (int nerrs2 = 0; nerrs2 < 5; nerrs2++){
      util (1, nerrs1, nerrs2);
    }
  }
}


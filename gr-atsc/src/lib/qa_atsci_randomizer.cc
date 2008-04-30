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

#include <qa_atsci_randomizer.h>

#include <cppunit/TestAssert.h>
#include <string.h>

static unsigned int expected_initial_states[] = {
  0x018f,
  0xd3db,
  0xbaf1,
  0x8e64,
  0x4732,
  0x2399,
  0xc2d0,
  0x6168
};

static unsigned char expected_initial_values[] = {
  0xc0,
  0x6d,
  0x3f,
  0x99,
  0x38,
  0x6a,
  0x29,
  0x52
};

#define	NELEMENTS(ary) ((sizeof (ary)) / sizeof (ary[0]))

void
qa_atsci_randomizer::t0_compare_output_maps ()
{
  for (int i = 0; i < 0x10000; i++){
    unsigned char slow = atsci_randomizer::slow_output_map(i);
    unsigned char fast = atsci_randomizer::fast_output_map(i);
    CPPUNIT_ASSERT_EQUAL (slow, fast);
  }
}

void
qa_atsci_randomizer::t1_initial_states ()
{
  // LFSR should start with expected states

  for (unsigned int i = 0; i < NELEMENTS (expected_initial_values); i++){
    unsigned int got = randomizer.state();
    randomizer.clk ();
    CPPUNIT_ASSERT_EQUAL (expected_initial_states[i], got);
  }
}

void
qa_atsci_randomizer::t2_initial_values ()
{
  // LFSR should start with expected values

  for (unsigned int i = 0; i < NELEMENTS (expected_initial_values); i++){
    unsigned char got = randomizer.output_and_clk ();
    CPPUNIT_ASSERT_EQUAL (expected_initial_values[i], got);
  }
}

void
qa_atsci_randomizer::t3_reset ()
{
  // LFSR should start with expected values

  for (unsigned int i = 0; i < NELEMENTS (expected_initial_values); i++){
    unsigned char got = randomizer.output_and_clk ();
    CPPUNIT_ASSERT_EQUAL (expected_initial_values[i], got);
  }

  randomizer.reset ();	// reset LFSR
  
  // and now should repeat expected values

  for (unsigned int i = 0; i < NELEMENTS (expected_initial_values); i++){
    unsigned char got = randomizer.output_and_clk ();
    CPPUNIT_ASSERT_EQUAL (expected_initial_values[i], got);
  }
}

void
qa_atsci_randomizer::t4_high_level ()
{
  static const int	N = 5;

  atsc_mpeg_packet		in[N];
  atsc_mpeg_packet_no_sync	middle[N];
  atsc_mpeg_packet		out[N];
  
  memset (in, 0, sizeof (in));
  memset (middle, 0, sizeof (middle));
  memset (out, 0, sizeof (out));

  // setup input test data

  int	counter = 0xff;
  for (int n = 0; n < N; n++){
    in[n].data[0] = MPEG_SYNC_BYTE;
    for (int i = 0; i < ATSC_MPEG_DATA_LENGTH; i++)
      in[n].data[i + 1] = counter++;
  }

  // randomize N packets

  for (int n = 0; n < N; n++)
    randomizer.randomize (middle[n], in[n]);

  // reset LFSR and derandomize N packets

  randomizer.reset ();	// reset LFSR

  for (int n = 0; n < N; n++)
    randomizer.derandomize (out[n], middle[n]);

  // compare packets

  for (int n = 0; n < N; n++){
    CPPUNIT_ASSERT (in[n] == out[n]);
  }
}


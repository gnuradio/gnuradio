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

#include <cppunit/TestAssert.h>
#include "qa_atsci_trellis_encoder.h"
#include <cstdio>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define	NELEM(x) (sizeof (x) / sizeof (x[0]))


static const int NCODERS = atsci_trellis_encoder::NCODERS;

void
qa_atsci_trellis_encoder::t0 ()
{
#if 0		// generate i/o test data for t1

  atsc_mpeg_packet_rs_encoded	in[NCODERS];
  atsc_data_segment		out[NCODERS];


  memset (in,  0, sizeof (in));
  memset (out, 0, sizeof (out));

  srandom (1);

  printf ("@@@ INPUT @@@\n");
  for (int i = 0; i < NCODERS; i++){
    for (unsigned int j = 0; j < NELEM (in[i].data); j++){
      int t = (random () >> 8) & 0xff;	// 8 random bits
      in[i].data[j] = t;
      printf ("%d\n", t);
    }
  }

  enc.reset ();
  enc.encode (out, in);

  printf ("@@@ OUTPUT @@@\n");
  for (int i = 0; i < NCODERS; i++){
    for (unsigned int j = 0; j < NELEM (out[i].data); j++){
       printf ("%d\n", out[i].data[j]);
    }
  }
#endif
}

void
qa_atsci_trellis_encoder::t1 ()
{
  atsc_mpeg_packet_rs_encoded	in[NCODERS];
  atsc_data_segment		expected_out[NCODERS];
  atsc_data_segment		actual_out[NCODERS];
  static const unsigned char 	raw_input[NCODERS * NELEM (in[0].data)] = {
#include "qa_atsci_trellis_encoder_t1_input.dat"
  };
  static const unsigned char	raw_output[NCODERS * NELEM (expected_out[0].data)] = {
#include "qa_atsci_trellis_encoder_t1_output.dat"
  };


  // load up input
  const unsigned char *r = &raw_input[0];
  for (int i = 0; i < NCODERS; i++){
    in[i].pli.set_regular_seg (false, i);
    for (unsigned int j = 0; j < NELEM (in[i].data); j++){
      in[i].data[j] = *r++;
    }
  }

  // load up expected output
  r = &raw_output[0];
  for (int i = 0; i < NCODERS; i++){
    expected_out[i].pli.set_regular_seg (false, i);
    for (unsigned int j = 0; j < NELEM (expected_out[i].data); j++){
      expected_out[i].data[j] = *r++;
    }
  }

  memset (&actual_out, 0, sizeof (actual_out));		// ensure zero

  enc.reset ();
  enc.encode (actual_out, in);				// trellis code test data

  for (int i = 0; i < NCODERS; i++){			// check the result
    CPPUNIT_ASSERT (expected_out[i] == actual_out[i]);
    CPPUNIT_ASSERT (expected_out[i].pli == actual_out[i].pli);
  }
}

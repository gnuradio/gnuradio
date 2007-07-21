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
#include <qa_atsci_basic_trellis_encoder.h>
#include <stdlib.h>
#include <stdio.h>

void
qa_atsci_basic_trellis_encoder::t0 ()
{
  const static unsigned char in[14] = {
    0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 2, 3
  };

  const static unsigned char expected_out[14] = {
    0, 2, 3, 1, 1, 3, 0, 1, 2, 0, 0, 2, 5, 2
  };

  for (unsigned i = 0; i < sizeof (in); i++)
    CPPUNIT_ASSERT_EQUAL ((int) expected_out[i], enc.encode (in[i]));

  // reset encoder and test again.

  enc.reset ();
  for (unsigned i = 0; i < sizeof (in); i++)
    CPPUNIT_ASSERT_EQUAL ((int) expected_out[i], enc.encode (in[i]));
}


void
qa_atsci_basic_trellis_encoder::t1 ()
{
  // Print data to externally check distribution.
  // looks OK.
#if 0
  srandom (27);

  for (int i = 0; i < 8192; i++){
    int	t = (random () >> 10) & 0x3;	// 2 random bits
    printf ("%d  %d\n", t, enc.encode (t));
  }
#endif
}

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

#include <cppunit/TestAssert.h>
#include "qa_convolutional_interleaver.h"

void
qa_convolutional_interleaver::t0 ()
{
  static int input[16] = {
     1,  2,  3,  4,
     5,  6,  7,  8,
     9, 10, 11, 12,
    13, 14, 15, 16
  };

  static int output[16] = {
     1,  0, 0, 0,
     5,  2, 0, 0,
     9,  6, 3, 0,
    13, 10, 7, 4
  };

  // test interleaver
  intl = new convolutional_interleaver<int>(true, 4, 1);

  for (int i = 0; i < 16; i++)
    CPPUNIT_ASSERT_EQUAL (output[i], intl->transform (input[i]));
}

void
qa_convolutional_interleaver::t1 ()
{
  static int input[16] = {
     1,  0, 0, 0,
     5,  2, 0, 0,
     9,  6, 3, 0,
    13, 10, 7, 4
  };

  static int output[16] = {
     0,  0, 0, 0,
     0,  0, 0, 0,
     0,  0, 0, 0,
     1,  2, 3, 4
  };

  // test deinterleaver
  intl = new convolutional_interleaver<int>(false, 4, 1);

  for (int i = 0; i < 16; i++)
    CPPUNIT_ASSERT_EQUAL (output[i], intl->transform (input[i]));
}

void
qa_convolutional_interleaver::t2 ()
{
  intl = new convolutional_interleaver<int>(true, 4, 1);
  deintl = new convolutional_interleaver<int>(false, 4, 1);

  int	icount = 6000;
  int	dcount = 6000;

  int	end_to_end_delay = intl->end_to_end_delay ();
  for (int i = 0; i < end_to_end_delay; i++){
    CPPUNIT_ASSERT_EQUAL (0, deintl->transform (intl->transform (icount++)));
  }

  for (int i = 0; i < 3 * end_to_end_delay; i++){
    CPPUNIT_ASSERT_EQUAL (dcount++, deintl->transform (intl->transform (icount++)));
  }
}

void
qa_convolutional_interleaver::t3 ()
{
  intl = new convolutional_interleaver<int>(true, 4, 2);
  deintl = new convolutional_interleaver<int>(false, 4, 2);

  int	icount = 6000;
  int	dcount = 6000;

  int	end_to_end_delay = intl->end_to_end_delay ();
  for (int i = 0; i < end_to_end_delay; i++){
    CPPUNIT_ASSERT_EQUAL (0, deintl->transform (intl->transform (icount++)));
  }

  for (int i = 0; i < 3 * end_to_end_delay; i++){
    CPPUNIT_ASSERT_EQUAL (dcount++, deintl->transform (intl->transform (icount++)));
  }
}

void
qa_convolutional_interleaver::t4 ()
{
  intl = new convolutional_interleaver<int>(true, 52, 4);
  deintl = new convolutional_interleaver<int>(false, 52, 4);

  int	icount = 6000;
  int	dcount = 6000;

  int	end_to_end_delay = intl->end_to_end_delay ();
  CPPUNIT_ASSERT_EQUAL (10608, end_to_end_delay);

  for (int i = 0; i < end_to_end_delay; i++){
    CPPUNIT_ASSERT_EQUAL (0, deintl->transform (intl->transform (icount++)));
  }

  for (int i = 0; i < 3 * end_to_end_delay; i++){
    CPPUNIT_ASSERT_EQUAL (dcount++, deintl->transform (intl->transform (icount++)));
  }
}

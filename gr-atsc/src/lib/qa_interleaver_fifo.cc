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
#include <qa_interleaver_fifo.h>

void
qa_interleaver_fifo::t0 ()
{
  fifo = new interleaver_fifo<int>(0);

  for (int i = 10; i < 20; i++)
    CPPUNIT_ASSERT_EQUAL (i, fifo->stuff (i));
}

void
qa_interleaver_fifo::t1 ()
{
  fifo = new interleaver_fifo<int>(1);

  CPPUNIT_ASSERT_EQUAL (0, fifo->stuff (2));
  
  for (int i = 1; i < 10; i++)
    CPPUNIT_ASSERT_EQUAL (i * 2, fifo->stuff ((i + 1) * 2));
}

void
qa_interleaver_fifo::t2 ()
{
  fifo = new interleaver_fifo<int>(4);

  CPPUNIT_ASSERT_EQUAL (0, fifo->stuff (1));
  CPPUNIT_ASSERT_EQUAL (0, fifo->stuff (2));
  CPPUNIT_ASSERT_EQUAL (0, fifo->stuff (3));
  CPPUNIT_ASSERT_EQUAL (0, fifo->stuff (4));

  for (int i = 5; i < 20; i++)
    CPPUNIT_ASSERT_EQUAL (i - 4, fifo->stuff (i));
}

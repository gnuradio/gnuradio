/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#include <gri_lfsr.h>
#include <qa_gri_lfsr.h>
#include <cppunit/TestAssert.h>
#include <stdio.h>

void
qa_gri_lfsr::test_lfsr ()
{
  int mask = 0x19;
  int seed = 0x01;
  int length = 5;

  gri_lfsr lfsr1(mask,seed,length);
  gri_lfsr lfsr2(mask,seed,length);
  
  unsigned char expected[] = {1, 0, 1, 1, 0, 1, 0, 1, 0, 0};

  for(unsigned int i=0; i<31; i++){
    lfsr1.next_bit();
  }

  // test that after one lfsr cycle we still match out uncycled lfsr
  for (unsigned int i = 0; i < 41; i++) {
    CPPUNIT_ASSERT_EQUAL((int) lfsr1.next_bit(), (int) lfsr2.next_bit());
  }

  // test the known correct values at the given shift offset
  for(unsigned int i=0; i<10; i++){
    CPPUNIT_ASSERT_EQUAL((int) lfsr1.next_bit(), (int) expected[i]);
  }

  // test for register length too long
  CPPUNIT_ASSERT_THROW(gri_lfsr(mask, seed, 32), std::invalid_argument);
}


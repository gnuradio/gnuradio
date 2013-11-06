/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#include <qa_fast_atan2f.h>
#include <gnuradio/math.h>
#include <cppunit/TestAssert.h>
#include <cmath>

void
qa_fast_atan2f::t1()
{
  static const unsigned int N = 100;
  float c_atan2;
  float gr_atan2f;
  
  for(float i = -N/2; i < N/2; i++) {
    for(float j =-N/2; i < N/2; i++) {
      float x = i/10.0;
      float y = j/10.0;
      c_atan2 = atan2(x, y);
      
      gr_atan2f = gr::fast_atan2f(x, y);
      
      CPPUNIT_ASSERT_DOUBLES_EQUAL(c_atan2, gr_atan2f, 0.0001);
    }
  }
}


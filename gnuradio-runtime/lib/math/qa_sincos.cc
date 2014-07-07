/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <qa_sincos.h>
#include <gnuradio/sincos.h>
#include <cppunit/TestAssert.h>
#include <cmath>

void
qa_sincos::t1()
{
  static const unsigned int N = 1000;
  double c_sin, c_cos;
  double gr_sin, gr_cos;

  for(unsigned i = 0; i < N; i++) {
    double x = i/100.0;
    c_sin = sin(x);
    c_cos = cos(x);

    gr::sincos(x, &gr_sin, &gr_cos);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(c_sin, gr_sin, 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(c_cos, gr_cos, 0.0001);
  }
}

void
qa_sincos::t2()
{
  static const unsigned int N = 1000;
  float c_sin, c_cos;
  float gr_sin, gr_cos;

  for(unsigned i = 0; i < N; i++) {
    float x = i/100.0;
    c_sin = sinf(x);
    c_cos = cosf(x);

    gr::sincosf(x, &gr_sin, &gr_cos);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(c_sin, gr_sin, 0.0001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(c_cos, gr_cos, 0.0001);
  }
}

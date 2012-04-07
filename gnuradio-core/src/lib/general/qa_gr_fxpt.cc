/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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
#include <qa_gr_fxpt.h>
#include <gr_fxpt.h>
#include <cppunit/TestAssert.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

static const float SIN_COS_TOLERANCE = 1e-5;

void
qa_gr_fxpt::t0 ()
{
  CPPUNIT_ASSERT_DOUBLES_EQUAL (M_PI/2, gr_fxpt::fixed_to_float (0x40000000), SIN_COS_TOLERANCE);
  CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0,    gr_fxpt::fixed_to_float (0x00000000), SIN_COS_TOLERANCE);
  CPPUNIT_ASSERT_DOUBLES_EQUAL (-M_PI,  gr_fxpt::fixed_to_float (0x80000000), SIN_COS_TOLERANCE);

  if (0){
    /*
     * These are disabled because of some precision issues.
     *
     * Different compilers seem to have different opinions on whether
     * the calulations are done single or double (or extended)
     * precision.  Any of the answers are fine for our real purpose, but
     * sometimes the answer is off by a few bits at the bottom.
     * Hence, the disabled check.
     */
    CPPUNIT_ASSERT_EQUAL ((gr_int32) 0x40000000, gr_fxpt::float_to_fixed (M_PI/2));
    CPPUNIT_ASSERT_EQUAL ((gr_int32) 0,          gr_fxpt::float_to_fixed (0));
    CPPUNIT_ASSERT_EQUAL ((gr_int32) 0x80000000, gr_fxpt::float_to_fixed (-M_PI));
  }
}

void
qa_gr_fxpt::t1 ()
{

  CPPUNIT_ASSERT_DOUBLES_EQUAL ( 0,           gr_fxpt::sin (0x00000000), SIN_COS_TOLERANCE);
  CPPUNIT_ASSERT_DOUBLES_EQUAL ( 0.707106781, gr_fxpt::sin (0x20000000), SIN_COS_TOLERANCE);
  CPPUNIT_ASSERT_DOUBLES_EQUAL ( 1,           gr_fxpt::sin (0x40000000), SIN_COS_TOLERANCE);
  CPPUNIT_ASSERT_DOUBLES_EQUAL ( 0.707106781, gr_fxpt::sin (0x60000000), SIN_COS_TOLERANCE);
  CPPUNIT_ASSERT_DOUBLES_EQUAL ( 0,           gr_fxpt::sin (0x7fffffff), SIN_COS_TOLERANCE);
  CPPUNIT_ASSERT_DOUBLES_EQUAL ( 0,           gr_fxpt::sin (0x80000000), SIN_COS_TOLERANCE);
  CPPUNIT_ASSERT_DOUBLES_EQUAL ( 0,           gr_fxpt::sin (0x80000001), SIN_COS_TOLERANCE);
  CPPUNIT_ASSERT_DOUBLES_EQUAL (-1,           gr_fxpt::sin (-0x40000000), SIN_COS_TOLERANCE);
  CPPUNIT_ASSERT_DOUBLES_EQUAL (-0.707106781, gr_fxpt::sin (-0x20000000), SIN_COS_TOLERANCE);


  for (float p = -M_PI; p < M_PI; p += 2 * M_PI / 3600){
    float expected = sin (p);
    float actual = gr_fxpt::sin (gr_fxpt::float_to_fixed (p));
    CPPUNIT_ASSERT_DOUBLES_EQUAL (expected, actual, SIN_COS_TOLERANCE);
  }
}

void
qa_gr_fxpt::t2 ()
{
  for (float p = -M_PI; p < M_PI; p += 2 * M_PI / 3600){
    float expected = cos (p);
    float actual = gr_fxpt::cos (gr_fxpt::float_to_fixed (p));
    CPPUNIT_ASSERT_DOUBLES_EQUAL (expected, actual, SIN_COS_TOLERANCE);
  }
}

void
qa_gr_fxpt::t3 ()
{
  for (float p = -M_PI; p < M_PI; p += 2 * M_PI / 3600){
    float expected_sin = sin (p);
    float expected_cos = cos (p);
    float actual_sin;
    float actual_cos;
    gr_fxpt::sincos (gr_fxpt::float_to_fixed (p), &actual_sin, &actual_cos);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (expected_sin, actual_sin, SIN_COS_TOLERANCE);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (expected_cos, actual_cos, SIN_COS_TOLERANCE);
  }
}

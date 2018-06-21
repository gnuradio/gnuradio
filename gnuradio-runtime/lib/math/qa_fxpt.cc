/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
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

#include <gnuradio/fxpt.h>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

static const float SIN_COS_TOLERANCE = 1e-5;

BOOST_AUTO_TEST_CASE(t0) {
  BOOST_CHECK(std::abs(M_PI/2 - gr::fxpt::fixed_to_float(0x40000000)) <= SIN_COS_TOLERANCE);
  BOOST_CHECK(std::abs(0.0 -    gr::fxpt::fixed_to_float(0x00000000)) <= SIN_COS_TOLERANCE);
  BOOST_CHECK(std::abs(-M_PI -  gr::fxpt::fixed_to_float(0x80000000)) <= SIN_COS_TOLERANCE);

  if(0) {
    /*
     * These are disabled because of some precision issues.
     *
     * Different compilers seem to have different opinions on whether
     * the calculations are done single or double (or extended)
     * precision.  Any of the answers are fine for our real purpose, but
     * sometimes the answer is off by a few bits at the bottom.
     * Hence, the disabled check.
     */
    BOOST_CHECK_EQUAL((int32_t)0x40000000, gr::fxpt::float_to_fixed(M_PI/2));
    BOOST_CHECK_EQUAL((int32_t)0,          gr::fxpt::float_to_fixed(0));
    BOOST_CHECK_EQUAL((int32_t)0x80000000, gr::fxpt::float_to_fixed(-M_PI));
  }
}

BOOST_AUTO_TEST_CASE(t1) {
  BOOST_CHECK(std::abs( 0 -           gr::fxpt::sin(0x00000000)) <= SIN_COS_TOLERANCE);
  BOOST_CHECK(std::abs( 0.707106781 - gr::fxpt::sin(0x20000000)) <= SIN_COS_TOLERANCE);
  BOOST_CHECK(std::abs( 1 -           gr::fxpt::sin(0x40000000)) <= SIN_COS_TOLERANCE);
  BOOST_CHECK(std::abs( 0.707106781 - gr::fxpt::sin(0x60000000)) <= SIN_COS_TOLERANCE);
  BOOST_CHECK(std::abs( 0 -           gr::fxpt::sin(0x7fffffff)) <= SIN_COS_TOLERANCE);
  BOOST_CHECK(std::abs( 0 -           gr::fxpt::sin(0x80000000)) <= SIN_COS_TOLERANCE);
  BOOST_CHECK(std::abs( 0 -           gr::fxpt::sin(0x80000001)) <= SIN_COS_TOLERANCE);
  BOOST_CHECK(std::abs(-1 -           gr::fxpt::sin(-0x40000000)) <= SIN_COS_TOLERANCE);
  BOOST_CHECK(std::abs(-0.707106781 - gr::fxpt::sin(-0x20000000)) <= SIN_COS_TOLERANCE);

  for(float p = -M_PI; p < M_PI; p += 2 * M_PI / 3600) {
    float expected = sin(p);
    float actual = gr::fxpt::sin(gr::fxpt::float_to_fixed (p));
    BOOST_CHECK(std::abs(expected - actual) <= SIN_COS_TOLERANCE);
  }
}

BOOST_AUTO_TEST_CASE(t2) {
  for(float p = -M_PI; p < M_PI; p += 2 * M_PI / 3600) {
    float expected = cos(p);
    float actual = gr::fxpt::cos(gr::fxpt::float_to_fixed(p));
    BOOST_CHECK(std::abs(expected - actual) <= SIN_COS_TOLERANCE);
  }
}

BOOST_AUTO_TEST_CASE(t3) {
  for(float p = -M_PI; p < M_PI; p += 2 * M_PI / 3600) {
    float expected_sin = sin(p);
    float expected_cos = cos(p);
    float actual_sin;
    float actual_cos;
    gr::fxpt::sincos(gr::fxpt::float_to_fixed (p), &actual_sin, &actual_cos);
    BOOST_CHECK(std::abs(expected_sin - actual_sin) <= SIN_COS_TOLERANCE);
    BOOST_CHECK(std::abs(expected_cos - actual_cos) <= SIN_COS_TOLERANCE);
  }
}

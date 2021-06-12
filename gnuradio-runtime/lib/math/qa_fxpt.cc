/* -*- c++ -*- */
/*
 * Copyright 2004,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/fxpt.h>
#include <gnuradio/math.h>
#include <unistd.h>
#include <boost/test/unit_test.hpp>
#include <cmath>

static const float SIN_COS_TOLERANCE = 1e-5;

BOOST_AUTO_TEST_CASE(t0)
{
    BOOST_CHECK(std::abs(GR_M_PI / 2 - gr::fxpt::fixed_to_float(0x40000000)) <=
                SIN_COS_TOLERANCE);
    BOOST_CHECK(std::abs(0.0 - gr::fxpt::fixed_to_float(0x00000000)) <=
                SIN_COS_TOLERANCE);
    BOOST_CHECK(std::abs(-GR_M_PI - gr::fxpt::fixed_to_float(0x80000000)) <=
                SIN_COS_TOLERANCE);

    if (0) {
        /*
         * These are disabled because of some precision issues.
         *
         * Different compilers seem to have different opinions on whether
         * the calculations are done single or double (or extended)
         * precision.  Any of the answers are fine for our real purpose, but
         * sometimes the answer is off by a few bits at the bottom.
         * Hence, the disabled check.
         */
        BOOST_CHECK_EQUAL((int32_t)0x40000000, gr::fxpt::float_to_fixed(GR_M_PI / 2));
        BOOST_CHECK_EQUAL((int32_t)0, gr::fxpt::float_to_fixed(0));
        BOOST_CHECK_EQUAL((int32_t)0x80000000, gr::fxpt::float_to_fixed(-GR_M_PI));
    }
}

BOOST_AUTO_TEST_CASE(t1)
{
    BOOST_CHECK(std::abs(0 - gr::fxpt::sin(0x00000000)) <= SIN_COS_TOLERANCE);
    BOOST_CHECK(std::abs(0.707106781 - gr::fxpt::sin(0x20000000)) <= SIN_COS_TOLERANCE);
    BOOST_CHECK(std::abs(1 - gr::fxpt::sin(0x40000000)) <= SIN_COS_TOLERANCE);
    BOOST_CHECK(std::abs(0.707106781 - gr::fxpt::sin(0x60000000)) <= SIN_COS_TOLERANCE);
    BOOST_CHECK(std::abs(0 - gr::fxpt::sin(0x7fffffff)) <= SIN_COS_TOLERANCE);
    BOOST_CHECK(std::abs(0 - gr::fxpt::sin(0x80000000)) <= SIN_COS_TOLERANCE);
    BOOST_CHECK(std::abs(0 - gr::fxpt::sin(0x80000001)) <= SIN_COS_TOLERANCE);
    BOOST_CHECK(std::abs(-1 - gr::fxpt::sin(-0x40000000)) <= SIN_COS_TOLERANCE);
    BOOST_CHECK(std::abs(-0.707106781 - gr::fxpt::sin(-0x20000000)) <= SIN_COS_TOLERANCE);

    for (float p = -GR_M_PI; p < GR_M_PI; p += 2 * GR_M_PI / 3600) {
        float expected = sin(p);
        float actual = gr::fxpt::sin(gr::fxpt::float_to_fixed(p));
        BOOST_CHECK(std::abs(expected - actual) <= SIN_COS_TOLERANCE);
    }
}

BOOST_AUTO_TEST_CASE(t2)
{
    for (float p = -GR_M_PI; p < GR_M_PI; p += 2 * GR_M_PI / 3600) {
        float expected = cos(p);
        float actual = gr::fxpt::cos(gr::fxpt::float_to_fixed(p));
        BOOST_CHECK(std::abs(expected - actual) <= SIN_COS_TOLERANCE);
    }
}

BOOST_AUTO_TEST_CASE(t3)
{
    for (float p = -GR_M_PI; p < GR_M_PI; p += 2 * GR_M_PI / 3600) {
        float expected_sin = sin(p);
        float expected_cos = cos(p);
        float actual_sin;
        float actual_cos;
        gr::fxpt::sincos(gr::fxpt::float_to_fixed(p), &actual_sin, &actual_cos);
        BOOST_CHECK(std::abs(expected_sin - actual_sin) <= SIN_COS_TOLERANCE);
        BOOST_CHECK(std::abs(expected_cos - actual_cos) <= SIN_COS_TOLERANCE);
    }
}

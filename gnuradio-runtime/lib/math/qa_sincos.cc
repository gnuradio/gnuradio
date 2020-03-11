/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/sincos.h>
#include <boost/test/unit_test.hpp>
#include <cmath>

BOOST_AUTO_TEST_CASE(t1)
{
    static const unsigned int N = 1000;
    double c_sin, c_cos;
    double gr_sin, gr_cos;

    for (unsigned i = 0; i < N; i++) {
        double x = i / 100.0;
        c_sin = sin(x);
        c_cos = cos(x);

        gr::sincos(x, &gr_sin, &gr_cos);

        BOOST_CHECK_CLOSE(c_sin, gr_sin, 0.0001);
        BOOST_CHECK_CLOSE(c_cos, gr_cos, 0.0001);
    }
}

BOOST_AUTO_TEST_CASE(t2)
{
    static const unsigned int N = 1000;
    float c_sin, c_cos;
    float gr_sin, gr_cos;

    for (unsigned i = 0; i < N; i++) {
        float x = i / 100.0;
        c_sin = sinf(x);
        c_cos = cosf(x);

        gr::sincosf(x, &gr_sin, &gr_cos);

        BOOST_CHECK_CLOSE(c_sin, gr_sin, 0.0001);
        BOOST_CHECK_CLOSE(c_cos, gr_cos, 0.0001);
    }
}

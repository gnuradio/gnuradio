/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/math.h>
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <limits>

#ifdef _MSC_VER
#define ISNAN _isnan
#else
#define ISNAN std::isnan
#endif

BOOST_AUTO_TEST_CASE(t1)
{
    static const int N = 100;
    float c_atan2;
    float gr_atan2f;

    for (float i = -N / 2; i < N / 2; i++) {
        for (float j = -N / 2; j < N / 2; j++) {
            float x = i / 10.0;
            float y = j / 10.0;
            c_atan2 = atan2(y, x);

            gr_atan2f = gr::fast_atan2f(y, x);

            BOOST_CHECK_CLOSE(c_atan2, gr_atan2f, 0.001);
        }
    }
}

BOOST_AUTO_TEST_CASE(t2)
{
    float c_atan2;
    float gr_atan2f;
    float x, y;

    float inf = std::numeric_limits<float>::infinity();
    float nan = std::numeric_limits<float>::quiet_NaN();

    /* Test x as INF */
    x = inf;
    y = 0;
    c_atan2 = atan2(y, x);
    gr_atan2f = gr::fast_atan2f(y, x);
    BOOST_CHECK_CLOSE(c_atan2, gr_atan2f, 0.0);

    x = -inf;
    y = 0;
    c_atan2 = atan2(y, x);
    gr_atan2f = gr::fast_atan2f(y, x);
    BOOST_CHECK_CLOSE(c_atan2, gr_atan2f, 0.0);


    /* Test y as INF */
    x = 0;
    y = inf;
    c_atan2 = atan2(y, x);
    gr_atan2f = gr::fast_atan2f(y, x);
    BOOST_CHECK_CLOSE(c_atan2, gr_atan2f, 0.0);

    x = 0;
    y = -inf;
    c_atan2 = atan2(y, x);
    gr_atan2f = gr::fast_atan2f(y, x);
    BOOST_CHECK_CLOSE(c_atan2, gr_atan2f, 0.0);


    /* Test x and y as INF */
    x = inf;
    y = inf;
    gr_atan2f = gr::fast_atan2f(y, x);
    BOOST_CHECK(ISNAN(gr_atan2f));


    /* Test x as NAN */
    x = nan;
    y = 0;
    gr_atan2f = gr::fast_atan2f(y, x);
    BOOST_CHECK_CLOSE(0.0f, gr_atan2f, 0.0001);

    x = -nan;
    y = 0;
    gr_atan2f = gr::fast_atan2f(y, x);
    BOOST_CHECK_CLOSE(0.0f, gr_atan2f, 0.0001);


    /* Test y as NAN */
    x = 0;
    y = nan;
    gr_atan2f = gr::fast_atan2f(y, x);
    BOOST_CHECK_CLOSE(0.0f, gr_atan2f, 0.0001);

    x = 0;
    y = -nan;
    gr_atan2f = gr::fast_atan2f(y, x);
    BOOST_CHECK_CLOSE(0.0f, gr_atan2f, 0.0001);

    /* Test mixed NAN and INF */
    x = inf;
    y = nan;
    gr_atan2f = gr::fast_atan2f(y, x);
    BOOST_CHECK(ISNAN(gr_atan2f));

    x = nan;
    y = inf;
    gr_atan2f = gr::fast_atan2f(y, x);
    BOOST_CHECK(ISNAN(gr_atan2f));
}

/*
 * Copyright 2008 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/math.h>
#include <boost/test/unit_test.hpp>
#include <cmath>

BOOST_AUTO_TEST_CASE(test_binary_slicer1)
{
    float x[5] = { -1, -0.5, 0, 0.5, 1.0 };
    unsigned int z[5] = { 0, 0, 1, 1, 1 };
    unsigned int y;

    for (unsigned int i = 0; i < 5; i++) {
        y = gr::binary_slicer(x[i]);
        BOOST_CHECK_EQUAL(y, z[i]);
    }

    for (unsigned int i = 0; i < 5; i++) {
        y = gr::branchless_binary_slicer(x[i]);
        BOOST_CHECK_EQUAL(y, z[i]);
    }
}

BOOST_AUTO_TEST_CASE(test_quad_0deg_slicer1)
{
    gr_complex x[4] = {
        gr_complex(1, 0), gr_complex(0, 1), gr_complex(-1, 0), gr_complex(0, -1)
    };

    unsigned int z[4] = { 0, 1, 2, 3 };
    unsigned int y;

    for (unsigned int i = 0; i < 4; i++) {
        y = gr::quad_0deg_slicer(x[i]);
        BOOST_CHECK_EQUAL(y, z[i]);
    }

    for (unsigned int i = 0; i < 4; i++) {
        y = gr::branchless_quad_0deg_slicer(x[i]);
        BOOST_CHECK_EQUAL(y, z[i]);
    }
}

BOOST_AUTO_TEST_CASE(test_quad_45deg_slicer1)
{
    gr_complex x[4] = { gr_complex(0.707, 0.707),
                        gr_complex(-0.707, 0.707),
                        gr_complex(-0.707, -0.707),
                        gr_complex(0.707, -0.707) };

    unsigned int z[4] = { 0, 1, 2, 3 };
    unsigned int y;

    for (unsigned int i = 0; i < 4; i++) {
        y = gr::quad_45deg_slicer(x[i]);
        BOOST_CHECK_EQUAL(y, z[i]);
    }

    for (unsigned int i = 0; i < 4; i++) {
        y = gr::branchless_quad_45deg_slicer(x[i]);
        BOOST_CHECK_EQUAL(y, z[i]);
    }
}

/* -*- c++ -*- */
/*
 * Copyright 2002,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/fft/fft.h>
#include <gnuradio/filter/mmse_fir_interpolator_ff.h>
#include <gnuradio/math.h>
#include <volk/volk_alloc.hh>
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <cstdio>

namespace gr {
namespace filter {

static float test_fcn(double index)
{
    return (2 * sin(index * 0.25 * 2 * GR_M_PI + 0.125 * GR_M_PI) +
            3 * sin(index * 0.077 * 2 * GR_M_PI + 0.3 * GR_M_PI));
}

BOOST_AUTO_TEST_CASE(t1)
{
    // use aligned malloc and make sure that everything in this
    // buffer is properly initialized.
    static const unsigned N = 100;
    volk::vector<float> input(N + 10);

    for (unsigned i = 0; i < N + 10; i++)
        input[i] = test_fcn((double)i);

    mmse_fir_interpolator_ff intr;
    float inv_nsteps = 1.0 / intr.nsteps();

    for (unsigned i = 0; i < N; i++) {
        for (unsigned imu = 0; imu <= intr.nsteps(); imu += 1) {
            float expected = test_fcn((i + 3) + imu * inv_nsteps);
            float actual = intr.interpolate(&input[i], imu * inv_nsteps);

            BOOST_CHECK(std::abs(expected - actual) <= 0.004);
            // printf ("%9.6f  %9.6f  %9.6f\n", expected, actual, expected - actual);
        }
    }
}

} /* namespace filter */
} /* namespace gr */

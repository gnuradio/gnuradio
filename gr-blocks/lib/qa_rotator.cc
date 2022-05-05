/* -*- c++ -*- */
/*
 * Copyright 2002,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/attributes.h>
#include <gnuradio/blocks/rotator.h>
#include <gnuradio/expj.h>
#include <gnuradio/math.h>
#include <boost/test/unit_test.hpp>

#include <cmath>

// error vector magnitude
[[maybe_unused]] static float error_vector_mag(gr_complex a, gr_complex b)
{
    return abs(a - b);
}

BOOST_AUTO_TEST_CASE(t1)
{
    static const unsigned int N = 100000;

    gr::blocks::rotator r;

    double phase_incr = 2 * GR_M_PI / 1003;
    double phase = 0;

    // Old code: We increment then return the rotated value, thus we
    // need to start one tick back r.set_phase(gr_complex(1,0) *
    // conj(gr_expj(phase_incr)));

    r.set_phase(gr_complex(1, 0));
    r.set_phase_incr(gr_expj(phase_incr));

    for (unsigned i = 0; i < N; i++) {
        gr_complex expected = gr_expj(phase);
        gr_complex actual = r.rotate(gr_complex(1, 0));

        BOOST_CHECK(std::abs(expected - actual) <= 0.0001);

        phase += phase_incr;
        if (phase >= 2 * GR_M_PI)
            phase -= 2 * GR_M_PI;
    }
}

BOOST_AUTO_TEST_CASE(t2)
{
    static const unsigned int N = 100000;

    gr::blocks::rotator r;
    std::vector<gr_complex> input(N);
    std::vector<gr_complex> output(N);

    double phase_incr = 2 * GR_M_PI / 1003;
    double phase = 0;

    r.set_phase(gr_complex(1, 0));
    r.set_phase_incr(gr_expj(phase_incr));

    // Generate a unity sequence
    for (unsigned i = 0; i < N; i++)
        input[i] = gr_complex(1.0f, 0.0f);

    // Rotate it
    r.rotateN(output.data(), input.data(), N);

    // Compare with expected result
    for (unsigned i = 0; i < N; i++) {
        gr_complex expected = gr_expj(phase);
        gr_complex actual = output[i];

        BOOST_CHECK(std::abs(expected - actual) <= 0.0001);

        phase += phase_incr;
        if (phase >= 2 * GR_M_PI)
            phase -= 2 * GR_M_PI;
    }
}

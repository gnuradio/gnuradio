/* -*- c++ -*- */
/*
 * Copyright 2004,2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vco.h"
#include <gnuradio/fxpt_vco.h>
#include <unistd.h>
#include <boost/test/unit_test.hpp>
#include <cmath>

static const float SIN_COS_TOLERANCE = 1e-5;

static const float SIN_COS_K = 0.42;
static const float SIN_COS_AMPL = 0.8;

static const int SIN_COS_BLOCK_SIZE = 100000;

static double max_d(double a, double b) { return fabs(a) > fabs(b) ? a : b; }

BOOST_AUTO_TEST_CASE(t0)
{
    gr::vco<float, float> ref_vco;
    gr::fxpt_vco new_vco;
    double max_error = 0, max_phase_error = 0;
    float input[SIN_COS_BLOCK_SIZE];

    for (int i = 0; i < SIN_COS_BLOCK_SIZE; i++) {
        input[i] = sin(double(i));
    }

    for (int i = 0; i < SIN_COS_BLOCK_SIZE; i++) {
        float ref_cos = ref_vco.cos();
        float new_cos = new_vco.cos();
        BOOST_CHECK(std::abs(ref_cos - new_cos) <= SIN_COS_TOLERANCE);

        max_error = max_d(max_error, ref_cos - new_cos);

        ref_vco.adjust_phase(input[i]);
        new_vco.adjust_phase(input[i]);

        BOOST_CHECK(std::abs(ref_vco.get_phase() - new_vco.get_phase()) <=
                    SIN_COS_TOLERANCE);

        max_phase_error =
            max_d(max_phase_error, ref_vco.get_phase() - new_vco.get_phase());
    }
}

BOOST_AUTO_TEST_CASE(t1)
{
    gr::vco<float, float> ref_vco;
    gr::fxpt_vco new_vco;
    std::vector<float> ref_block(SIN_COS_BLOCK_SIZE);
    std::vector<float> new_block(SIN_COS_BLOCK_SIZE);
    std::vector<float> input(SIN_COS_BLOCK_SIZE);
    double max_error = 0;

    for (int i = 0; i < SIN_COS_BLOCK_SIZE; i++) {
        input[i] = sin(double(i));
    }

    ref_vco.cos(
        ref_block.data(), input.data(), SIN_COS_BLOCK_SIZE, SIN_COS_K, SIN_COS_AMPL);
    new_vco.cos(
        new_block.data(), input.data(), SIN_COS_BLOCK_SIZE, SIN_COS_K, SIN_COS_AMPL);

    for (int i = 0; i < SIN_COS_BLOCK_SIZE; i++) {
        BOOST_CHECK(std::abs(ref_block[i] - new_block[i]) <= SIN_COS_TOLERANCE);
        max_error = max_d(max_error, ref_block[i] - new_block[i]);
    }
    BOOST_CHECK(std::abs(ref_vco.get_phase() - new_vco.get_phase()) <= SIN_COS_TOLERANCE);
}


BOOST_AUTO_TEST_CASE(t2)
{
    gr::vco<gr_complex, float> ref_vco;
    gr::fxpt_vco new_vco;
    std::vector<gr_complex> ref_block(SIN_COS_BLOCK_SIZE);
    std::vector<gr_complex> new_block(SIN_COS_BLOCK_SIZE);
    std::vector<float> input(SIN_COS_BLOCK_SIZE);
    double max_error = 0;

    for (int i = 0; i < SIN_COS_BLOCK_SIZE; i++) {
        input[i] = sin(double(i));
    }

    ref_vco.sincos(
        ref_block.data(), input.data(), SIN_COS_BLOCK_SIZE, SIN_COS_K, SIN_COS_AMPL);
    new_vco.sincos(
        new_block.data(), input.data(), SIN_COS_BLOCK_SIZE, SIN_COS_K, SIN_COS_AMPL);

    for (int i = 0; i < SIN_COS_BLOCK_SIZE; i++) {
        BOOST_CHECK(std::abs(ref_block[i] - new_block[i]) <= SIN_COS_TOLERANCE);
        max_error = max_d(max_error, abs(ref_block[i] - new_block[i]));
    }
    BOOST_CHECK(std::abs(ref_vco.get_phase() - new_vco.get_phase()) <= SIN_COS_TOLERANCE);
}

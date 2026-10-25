/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sig_source_nco.h"
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <limits>
#include <vector>

BOOST_AUTO_TEST_CASE(frequency_accuracy)
{
    constexpr double sample_rate = 1e8;
    constexpr int samples = 123456789;
    for (double frequency : { 0.0, 1.0, -1.0, 1e-6, -1e-6, 87923.234 }) {
        gr::analog::detail::sig_source_nco nco;
        const double rate = 2 * GR_M_PI * frequency / sample_rate;
        nco.set_freq(rate);
        const double tuning_tolerance =
            2 * std::numeric_limits<double>::epsilon() * std::abs(rate) +
            2 * GR_M_PI / 0x1p64;
        BOOST_CHECK_SMALL(nco.get_freq() - rate, tuning_tolerance);
        nco.step(samples);
        const double phase = std::remainder(rate * samples, 2 * GR_M_PI);
        BOOST_CHECK_SMALL(std::remainder(nco.get_phase() - phase, 2 * GR_M_PI), 3e-7);
        BOOST_CHECK_SMALL(nco.sin() - std::sin(phase), 6e-6);
        BOOST_CHECK_SMALL(nco.cos() - std::cos(phase), 6e-6);
    }
}

BOOST_AUTO_TEST_CASE(block_partition_and_step)
{
    constexpr int count = 1021;
    for (double rate : { 0.0, 0.123456789, -0.123456789, 1e-12 }) {
        gr::analog::detail::sig_source_nco whole, split, scalar;
        whole.set_phase(3.0f);
        split.set_phase(3.0f);
        scalar.set_phase(3.0f);
        whole.set_freq(rate);
        split.set_freq(rate);
        scalar.set_freq(rate);
        std::vector<gr_complex> a(count), b(count);
        whole.sincos(a.data(), count, 0.75);
        int offset = 0;
        for (int chunk : { 1, 17, 31, 233, 739 }) {
            split.sincos(b.data() + offset, chunk, 0.75);
            offset += chunk;
        }
        BOOST_CHECK(a == b);
        for (int i = 0; i < count; ++i) {
            BOOST_CHECK_SMALL(a[i].real() - 0.75 * scalar.cos(), 1e-7);
            BOOST_CHECK_SMALL(a[i].imag() - 0.75 * scalar.sin(), 1e-7);
            scalar.step();
        }
        BOOST_CHECK_EQUAL(whole.get_phase(), split.get_phase());
        BOOST_CHECK_EQUAL(whole.get_phase(), scalar.get_phase());
        scalar.step(-count);
        BOOST_CHECK_SMALL(scalar.get_phase() - 3.0, 3e-7);
    }
}

BOOST_AUTO_TEST_CASE(retune_and_wrap)
{
    gr::analog::detail::sig_source_nco nco;
    const double first_rate = 2 * GR_M_PI / 1e8;
    nco.set_phase(0.25f);
    nco.set_freq(first_rate);
    nco.step(10000000);
    const float before = nco.get_phase();
    nco.set_freq(-first_rate);
    BOOST_CHECK_EQUAL(nco.get_phase(), before);
    nco.step(10000000);
    BOOST_CHECK_SMALL(nco.get_phase() - 0.25, 3e-7);
    nco.set_freq(first_rate);
    nco.step(25000000);
    BOOST_CHECK_SMALL(nco.get_phase() - (0.25 + GR_M_PI / 2), 3e-7);
    nco.set_freq(0);
    const float stopped = nco.get_phase();
    nco.step(2000000000);
    BOOST_CHECK_EQUAL(nco.get_phase(), stopped);
}


BOOST_AUTO_TEST_CASE(phase_conversion_boundaries)
{
    gr::analog::detail::sig_source_nco nco;
    for (double rate : { GR_M_PI,
                         -GR_M_PI,
                         2 * GR_M_PI,
                         -2 * GR_M_PI,
                         std::nextafter(GR_M_PI, 0.0),
                         std::nextafter(-GR_M_PI, 0.0),
                         1e100,
                         -1e100 }) {
        nco.set_phase(0);
        nco.set_freq(rate);
        nco.step();
        const double phase = std::remainder(rate, 2 * GR_M_PI);
        BOOST_CHECK_SMALL(std::remainder(nco.get_phase() - phase, 2 * GR_M_PI), 3e-7);
        BOOST_CHECK_SMALL(nco.sin() - std::sin(phase), 6e-6);
        nco.step(-1);
        BOOST_CHECK_EQUAL(nco.get_phase(), 0);
    }
    for (double invalid : { std::numeric_limits<double>::infinity(),
                            -std::numeric_limits<double>::infinity(),
                            std::numeric_limits<double>::quiet_NaN() }) {
        BOOST_CHECK_THROW(nco.set_freq(invalid), std::invalid_argument);
        BOOST_CHECK_THROW(nco.set_phase(invalid), std::invalid_argument);
    }
}

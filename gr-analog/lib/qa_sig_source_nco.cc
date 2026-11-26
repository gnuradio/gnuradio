/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sig_source_nco.h"
#include <gnuradio/analog/sig_source.h>
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <limits>
#include <vector>

BOOST_AUTO_TEST_CASE(frequency_accuracy)
{
    constexpr double sample_rate = 1e8;
    constexpr int samples = 123456789;
    for (double frequency : { 0.0, 1.0, -1.0, 1e-6, -1e-6, 1e-3, -1e-3, 87923.234 }) {
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


BOOST_AUTO_TEST_CASE(phase_sign_and_precision)
{
    gr::analog::detail::sig_source_nco nco;
    // These values cover tiny phases and rounding boundaries near quarter turns.
    // A float-only scale can move a phase across a Source waveform comparison.
    for (double phase : { 0.0,
                          0x1p-50,
                          -0x1p-50,
                          1.0,
                          -1.0,
                          1.57079628,
                          -1.57079628,
                          3.14159253,
                          -3.14159253,
                          GR_M_PI,
                          -GR_M_PI }) {
        nco.set_phase(0);
        nco.set_freq(phase);
        nco.step();
        // One step from zero must expose the actual quantized increment, even
        // when the phase is too small to appear in the upper 32 phase bits.
        const float expected = nco.get_freq();
        BOOST_CHECK_EQUAL(nco.get_phase(), expected);
        BOOST_CHECK_EQUAL(nco.phase_is_negative(), expected < 0);
        nco.step(-1);
        BOOST_CHECK_EQUAL(nco.get_phase(), 0);
        BOOST_CHECK(!nco.phase_is_negative());
    }

    nco.set_freq(GR_M_PI / 32);
    for (int sample = 0; sample < 129; ++sample) {
        BOOST_CHECK_EQUAL(nco.phase_is_negative(), nco.get_phase() < 0);
        nco.step();
    }
}

BOOST_AUTO_TEST_CASE(normalized_phase_accuracy)
{
    gr::analog::detail::sig_source_nco nco;
    for (double rate :
         { 0.0, 1e-20, -1e-20, 1e-12, -1e-12, GR_M_PI / 2, -GR_M_PI / 2, GR_M_PI }) {
        nco.set_phase(0);
        nco.set_freq(rate);
        nco.step();
        const double expected = std::remainder(nco.get_freq(), 2 * GR_M_PI) / GR_M_PI;
        BOOST_CHECK_SMALL(nco.get_phase_normalized() - expected, 2e-16);
        nco.step(-1);
        BOOST_CHECK_EQUAL(nco.get_phase_normalized(), 0);
    }
}

BOOST_AUTO_TEST_CASE(sample_rate_change_preserves_signal)
{
    constexpr int count = 1021;
    constexpr double sample_rate = 1e8;
    constexpr float initial_phase = 0.23f;
    for (double frequency : { 0.0,
                              1.0,
                              -1.0,
                              1e-3,
                              -1e-3,
                              1234.567,
                              -1234.567,
                              sample_rate / 4,
                              -sample_rate / 4,
                              sample_rate / 2,
                              -sample_rate / 2 }) {
        auto source = gr::analog::sig_source_c::make(
            sample_rate, gr::analog::GR_COS_WAVE, frequency, 0.75, 0, initial_phase);
        std::vector<gr_complex> output(count);
        gr_vector_const_void_star inputs;
        gr_vector_void_star outputs{ output.data() };
        source->work(count, inputs, outputs);
        const float phase = source->phase();
        source->set_sampling_freq(2 * sample_rate);
        BOOST_CHECK_EQUAL(source->phase(), phase);
        BOOST_CHECK_EQUAL(source->frequency(), frequency);
        BOOST_CHECK_EQUAL(source->sampling_freq(), 2 * sample_rate);
        source->work(count, inputs, outputs);
        for (int i = 0; i < count; ++i) {
            const double expected =
                initial_phase + 2 * GR_M_PI * frequency / sample_rate * (count + i / 2.0);
            BOOST_CHECK_SMALL(output[i].real() - 0.75 * std::cos(expected), 6e-6);
            BOOST_CHECK_SMALL(output[i].imag() - 0.75 * std::sin(expected), 6e-6);
        }
    }
}

BOOST_AUTO_TEST_CASE(quadrant_matches_float_phase_comparison)
{
    // The complex waveforms used to compare get_phase() with +/-pi/2. Sweep
    // across each quarter turn boundary in steps much finer than the float
    // rounding of the phase and require the same selection.
    const auto previous = [](const gr::analog::detail::sig_source_nco& nco) {
        if (nco.get_phase() < -1 * GR_M_PI / 2)
            return 2u;
        if (nco.get_phase() < 0)
            return 3u;
        if (nco.get_phase() < GR_M_PI / 2)
            return 0u;
        return 1u;
    };
    constexpr double span = 1.5e-7; // well beyond the float rounding of +/-pi/2
    for (double boundary : { -GR_M_PI, -GR_M_PI / 2, 0.0, GR_M_PI / 2, GR_M_PI }) {
        for (double rate : { 1e-11, -1e-11, 3.7e-12 }) {
            const int count = static_cast<int>(span / std::abs(rate));
            gr::analog::detail::sig_source_nco nco;
            nco.set_phase(static_cast<float>(boundary));
            nco.set_freq(rate);
            nco.step(-count);
            for (int i = 0; i < 2 * count; ++i) {
                BOOST_REQUIRE_EQUAL(nco.quadrant(), previous(nco));
                nco.step();
            }
        }
    }
}

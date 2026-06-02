/* -*- c++ -*- */
/*
 * Copyright 2002,2015,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 *  Copyright 1997 Massachusetts Institute of Technology
 *
 *  Permission to use, copy, modify, distribute, and sell this software and its
 *  documentation for any purpose is hereby granted without fee, provided that
 *  the above copyright notice appear in all copies and that both that
 *  copyright notice and this permission notice appear in supporting
 *  documentation, and that the name of M.I.T. not be used in advertising or
 *  publicity pertaining to distribution of the software without specific,
 *  written prior permission.  M.I.T. makes no representations about the
 *  suitability of this software for any purpose.  It is provided "as is"
 *  without express or implied warranty.
 *
 */

#include <cstdint>
#include <limits>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/math.h>
#include <gnuradio/random.h>

#include <chrono>
#include <cmath>

namespace gr {

random::random(uint64_t seed, int64_t min_integer, int64_t max_integer)
    : d_rng(seed), d_integer_dis(0, 1)
{
    d_gauss_stored = false; // set gasdev (gauss distributed numbers) on calculation state

    // Setup random number generators
    set_integer_limits(min_integer, max_integer);
}

random::~random() {}

/*
 * Seed is initialized with time if the given seed is 0. Otherwise the seed is taken
 * directly. Sets the seed for the random number generator.
 */
void random::reseed(uint64_t seed)
{
    d_seed = seed;
    if (d_seed == 0) {
        auto now = std::chrono::system_clock::now().time_since_epoch();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
        d_rng.seed(ns);
    } else {
        d_rng.seed(d_seed);
    }
}

void random::set_integer_limits(int64_t minimum, int64_t maximum)
{
    // boost expects integer limits defined as [minimum, maximum] which is unintuitive.
    // use the expected half open interval behavior! [minimum, maximum)!
    d_integer_dis = std::uniform_int_distribution<int64_t>(minimum, maximum - 1);
}

/*!
 * Uniform random integers in the range set by 'set_integer_limits' [min, max).
 */
int64_t random::ran_int() { return d_integer_dis(d_rng); }

/*
 * Returns uniformly distributed numbers in [0,1) taken from boost.random using a Mersenne
 * twister
 */
float random::ran1() { return d_uniform(d_rng); }

/*
 * Returns a normally distributed deviate with zero mean and variance 1.
 * Used is the Marsaglia polar method.
 * Every second call a number is stored because the transformation works only in pairs.
 * Otherwise half calculation is thrown away.
 */
float random::gasdev()
{
    if (d_gauss_stored) { // just return the stored value if available
        d_gauss_stored = false;
        return d_gauss_value;
    } else { // generate a pair of gaussian distributed numbers
        float x, y, s;
        do {
            x = 2.0 * ran1() - 1.0;
            y = 2.0 * ran1() - 1.0;
            s = x * x + y * y;
        } while (s >= 1.0f || s == 0.0f);
        d_gauss_stored = true;
        d_gauss_value = x * sqrtf(-2.0 * logf(s) / s);
        return y * sqrtf(-2.0 * logf(s) / s);
    }
}

float random::laplacian()
{
    /* Original code, for reference:
     *
     * float z = ran1();
     * if (z > 0.5f) {
     *     return -logf(2.0f * (1.0f - z));
     * }
     * return logf(2 * z);
     *
     * So: Z ~ U[0;1], which means the (1.0f - z)
     * is utterly senseless, and then so is the "2*";
     * we transform the if-clause
     * float y = 2*ran1(); // =2*(1-z)
     * if (y <= 1.0f) {
     *      return -logf(y);
     * }
     * return logf(y);
     *
     * or, really, just
     *
     * return absf(logf(2*ran1()));
     * that's it. That should have been the function.
     *
     * BUT.
     *
     * Let's talk about Laplace(µ,2b) generation!
     *
     * Classically, let V ~ U[-0.5;+0.5];
     *
     * X = µ - b · sgn(V) ln(1 - 2·|V|)
     *
     * We're generating mean = 0, var = 2, so
     * µ = 0, b = 1
     *
     * therefore
     *
     * X = 0 - 1 · sgn(V) ln(1-2·|V|)
     *   = -sgn(V) ln(1-2·|V|)
     *
     * that ln argument is stupid – instead,
     * let W ~ U[0;1], S~U{-1,+1}
     * X = S · ln(W)
     *
     * That is a tiny bit less work, isn't it?
     * We need to form W from a 63 bit uniform integer P (we're using the
     * highest bit as S), so technically W = P / (2⁶³ - 1) but logarithm says
     * X = S · (ln(P) - ln(2⁶³-1))
     */
    // make sure we're on the right range
    static_assert(decltype(d_rng)::min() == 0 &&
                  decltype(d_rng)::max() == std::numeric_limits<uint64_t>::max());
    constexpr auto lower_bitmask = ((uint64_t)(1) << 63) - 1;
    // ln(2⁶³-1):
    constexpr float log_scaling = 0x1.5d589f2fe5107p+5;
    const uint64_t v = d_rng();
    const bool signbit = (v >> 63);
    const float w = (v & lower_bitmask);
    const float value = std::log(w) - log_scaling;
    return signbit ? -value : value;
}
/*
 * Copied from The KC7WW / OH2BNS Channel Simulator
 * FIXME Need to check how good this is at some point
 */
// 5 => scratchy, 8 => Geiger
float random::impulse(float factor = 5)
{
    float z = -GR_M_SQRT2 * logf(ran1());
    if (fabsf(z) <= factor)
        return 0.0;
    else
        return z;
}

gr_complex random::rayleigh_complex() { return gr_complex(gasdev(), gasdev()); }

float random::rayleigh() { return sqrtf(-2.0 * logf(ran1())); }

} /* namespace gr */

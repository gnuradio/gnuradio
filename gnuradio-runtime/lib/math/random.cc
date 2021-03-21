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
    float z = ran1();
    if (z > 0.5f) {
        return -logf(2.0f * (1.0f - z));
    }
    return logf(2 * z);
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

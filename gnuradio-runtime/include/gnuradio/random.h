/* -*- c++ -*- */
/*
 * Copyright 2002, 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RANDOM_H
#define INCLUDED_GR_RANDOM_H

#include <gnuradio/api.h>
#include <gnuradio/gr_complex.h>
#include <gnuradio/xoroshiro128p.h>

#include <limits>
#include <random>

namespace gr {

/*!
 * \brief wrapper for XOROSHIRO128+ PRNG for use in std::distributions
 * Fulfills C++ named requirements for UniformRandomBitGenerator
 * \ingroup math_blk
 */
class GR_RUNTIME_API xoroshiro128p_prng
{
public:
    using result_type = uint64_t; //! \brief value type is uint64

private:
    result_type state[2];

public:
    /*!
     * \brief minimum value
     */
    static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
    /*!
     * \brief maximum value
     */
    static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

    /*!
     * \brief constructor. Expects a seed.
     */
    xoroshiro128p_prng(uint64_t init) { seed(init); }


    /*!
     * \brief yield a random value and advance state
     */
    result_type operator()() { return xoroshiro128p_next(state); }

    /*!
     * \brief set new seed
     */
    void seed(uint64_t seed) { xoroshiro128p_seed(state, seed); }
};
/*!
 * \brief pseudo random number generator
 * \ingroup math_blk
 */
class GR_RUNTIME_API random
{
protected:
    long d_seed;
    bool d_gauss_stored;
    float d_gauss_value;

    xoroshiro128p_prng d_rng; // mersenne twister as random number generator
    std::uniform_real_distribution<float>
        d_uniform; // choose uniform distribution, default is [0,1)
    std::uniform_int_distribution<int64_t> d_integer_dis;

public:
    random(uint64_t seed = 0, int64_t min_integer = 0, int64_t max_integer = 2);
    ~random();

    /*!
     * \brief Change the seed for the initialized number generator. seed = 0 initializes
     * the random number generator with the system time.
     */
    void reseed(uint64_t seed);

    /*!
     * set minimum and maximum for integer random number generator.
     * Limits are [minimum, maximum)
     * Default: [0, std::numeric_limits< IntType >::max)]
     */
    void set_integer_limits(int64_t minimum, int64_t maximum);

    /*!
     * Uniform random integers in the range set by 'set_integer_limits' [min, max).
     */
    int64_t ran_int();

    /*!
     * \brief Uniform random numbers in the range [0.0, 1.0)
     */
    float ran1();

    /*!
     * \brief Normally distributed random numbers (Gaussian distribution with zero mean
     * and variance 1)
     */
    float gasdev();

    /*!
     * \brief Laplacian distributed random numbers with zero mean and variance 1
     */
    float laplacian();

    /*!
     * \brief Rayleigh distributed random numbers (zero mean and variance 1 for the
     * underlying Gaussian distributions)
     */
    float rayleigh();

    /*!
     * \brief Exponentially distributed random numbers with values less than or equal
     * to factor replaced with zero. The underlying exponential distribution has
     * mean sqrt(2) and variance 2.
     */
    float impulse(float factor);

    /*!
     * \brief Normally distributed random numbers with zero mean and variance 1 on real
     * and imaginary part. This results in a Rayleigh distribution for the amplitude and
     * an uniform distribution for the phase.
     */
    gr_complex rayleigh_complex();
};

} /* namespace gr */

#endif /* INCLUDED_GR_RANDOM_H */

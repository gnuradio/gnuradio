/* -*- c++ -*- */
/*
 * Copyright 2002, 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_RANDOM_H
#define INCLUDED_GR_RANDOM_H

#include <gnuradio/api.h>
#include <gnuradio/gr_complex.h>

#include <stdlib.h>
#include <boost/random.hpp>
#include <ctime>

namespace gr {

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

    boost::mt19937 *d_rng; // mersenne twister as random number generator
    boost::uniform_real<float> *d_uniform; // choose uniform distribution, default is [0,1)
    boost::uniform_int<> *d_integer_dis;
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > *d_generator;
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > *d_integer_generator;

  public:
    random(unsigned int seed=0, int min_integer = 0, int max_integer = 2);
    ~random();

    /*!
     * \brief Change the seed for the initialized number generator. seed = 0 initializes the random number generator with the system time. Note that a fast initialization of various instances can result in the same seed.
     */
    void reseed(unsigned int seed);

    /*!
     * set minimum and maximum for integer random number generator.
     * Limits are [minimum, maximum)
     * Default: [0, std::numeric_limits< IntType >::max)]
     */
    void set_integer_limits(const int minimum, const int maximum);

    /*!
     * Uniform random integers in the range set by 'set_integer_limits' [min, max).
     */
    int ran_int();

    /*!
     * \brief Uniform random numbers in the range [0.0, 1.0)
     */
    float ran1();

    /*!
     * \brief Normally distributed random numbers (Gaussian distribution with zero mean and variance 1)
     */
    float gasdev();

    /*!
     * \brief Laplacian distributed random numbers with zero mean and variance 1
     */
    float laplacian();

    /*!
     * \brief Rayleigh distributed random numbers (zero mean and variance 1 for the underlying Gaussian distributions)
     */
    float rayleigh();

    /*!
     * \brief FIXME: add description
     */
    float impulse(float factor);

    /*!
     * \brief Normally distributed random numbers with zero mean and variance 1 on real and imaginary part. This results in a Rayleigh distribution for the amplitude and an uniform distribution for the phase.
     */
    gr_complex rayleigh_complex();
  };

} /* namespace gr */

#endif /* INCLUDED_GR_RANDOM_H */


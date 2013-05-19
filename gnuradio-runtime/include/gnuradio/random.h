/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

// While rand(3) specifies RAND_MAX, random(3) says that the output
// ranges from 0 to 2^31-1 but does not specify a macro to denote
// this.  We define RANDOM_MAX for cleanliness.  We must omit the
// definition for systems that have made the same choice.  (Note that
// random(3) is from 4.2BSD, and not specified by POSIX.)

#ifndef RANDOM_MAX
static const int RANDOM_MAX = 2147483647; // 2^31-1
#endif /* RANDOM_MAX */

#include <stdlib.h>

namespace gr {

  /*!
   * \brief pseudo random number generator
   * \ingroup math_blk
   */
  class GR_RUNTIME_API random
  {
  protected:
    static const int NTAB = 32;
    long d_seed;
    long d_iy;
    long d_iv[NTAB];
    int	d_iset;
    float d_gset;

  public:
    random(long seed=3021);

    void reseed(long seed);

    /*!
     * \brief uniform random deviate in the range [0.0, 1.0)
     */
    float ran1();

    /*!
     * \brief normally distributed deviate with zero mean and variance 1
     */
    float gasdev();

    float laplacian();
    float impulse(float factor);
    float rayleigh();
    gr_complex rayleigh_complex();
  };

} /* namespace gr */

#endif /* INCLUDED_GR_RANDOM_H */


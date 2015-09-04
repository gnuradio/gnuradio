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

#include <math.h>
#include <gnuradio/random.h>

namespace gr {

  random::random(unsigned int seed)
  {
    d_gauss_stored = false; // set gasdev (gauss distributed numbers) on calculation state

    // Setup random number generator
    d_rng = new boost::mt19937;
    reseed(seed); // set seed for random number generator
    d_uniform = new boost::uniform_real<float>;
    d_generator = new boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > (*d_rng,*d_uniform); // create number generator in [0,1) from boost.random
  }

  random::~random()
  {
      delete d_rng;
      delete d_uniform;
      delete d_generator;
  }

  /*
   * Seed is initialized with time if the given seed is 0. Otherwise the seed is taken directly. Sets the seed for the random number generator.
   */
  void
  random::reseed(unsigned int seed)
  {
    if(seed==0) d_seed = static_cast<unsigned int>(std::time(0));
    else d_seed = seed;
    d_rng->seed(d_seed);
  }

  /*
   * Returns uniformly distributed numbers in [0,1) taken from boost.random using a Mersenne twister
   */
  float
  random::ran1()
  {
    return (*d_generator)();
  }

  /*
   * Returns a normally distributed deviate with zero mean and variance 1.
   * Used is the Marsaglia polar method.
   * Every second call a number is stored because the transformation works only in pairs. Otherwise half calculation is thrown away.
   */
  float
  random::gasdev()
  {
      if(d_gauss_stored){ // just return the stored value if available
          d_gauss_stored = false;
          return d_gauss_value;
      }
      else{ // generate a pair of gaussian distributed numbers
          float x,y,s;
          do{
              x = 2.0*ran1()-1.0;
              y = 2.0*ran1()-1.0;
              s = x*x+y*y;
          }while(not(s<1.0));
          d_gauss_stored = true;
          d_gauss_value = x*sqrt(-2.0*log(s)/s);
          return y*sqrt(-2.0*log(s)/s);
      }
  }

  float
  random::laplacian()
  {
    float z = ran1()-0.5;
    if(z>0) return -log(1-2*z);
    else return log(1+2*z);
  }

  /*
   * Copied from The KC7WW / OH2BNS Channel Simulator
   * FIXME Need to check how good this is at some point
   */
  // 5 => scratchy, 8 => Geiger
  float
  random::impulse(float factor = 5)
  {
    float z = -M_SQRT2 * log(ran1());
    if(fabsf(z) <= factor)
      return 0.0;
    else
      return z;
  }

  gr_complex
  random::rayleigh_complex()
  {
    return gr_complex(gasdev(),gasdev());
  }

  float
  random::rayleigh()
  {
    return sqrt(-2.0 * log(ran1()));
  }

} /* namespace gr */

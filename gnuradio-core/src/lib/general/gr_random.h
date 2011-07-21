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

#include <gr_core_api.h>
#include <gr_complex.h>

/*!
 * \brief pseudo random number generator
 * \ingroup math_blk
 */
class GR_CORE_API gr_random {
protected:
  static const int NTAB  = 32;
  long  d_seed;
  long  d_iy;
  long  d_iv[NTAB];
  int	d_iset;
  float	d_gset;

  
public: 
  gr_random (long seed=3021);

  void reseed (long seed);

  /*!
   * \brief uniform random deviate in the range [0.0, 1.0)
   */
  float ran1 ();

  /*!
   * \brief normally distributed deviate with zero mean and variance 1
   */
  float gasdev ();

  float laplacian ();
  float impulse (float factor);
  float rayleigh ();
  gr_complex rayleigh_complex ();
};

#endif /* INCLUDED_GR_RANDOM_H */


/* -*- c++ -*- */
/*
 * Copyright 2003,2005 Free Software Foundation, Inc.
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
 * mathematical odds and ends.
 */

#ifndef _GR_MATH_H_
#define _GR_MATH_H_

#include <gr_complex.h>

long gr_gcd (long m, long n);

// returns a non-zero value if value is "not-a-number" (NaN), and 0 otherwise
int gr_isnan (double value);

// returns a non-zero value if the value of x has its sign bit set.
//
// This  is  not  the  same  as `x < 0.0', because IEEE 754 floating point
// allows zero to be signed.  The comparison `-0.0 < 0.0'  is  false,  but
// `gr_signbit (-0.0)' will return a nonzero value.

int gr_signbit (double x);
  
/*!
 * \brief Fast arc tangent using table lookup and linear interpolation
 *
 * \param y component of input vector
 * \param x component of input vector
 * \returns float angle angle of vector (x, y) in radians
 *
 * This function calculates the angle of the vector (x,y) based on a
 * table lookup and linear interpolation. The table uses a 256 point
 * table covering -45 to +45 degrees and uses symetry to determine the
 * final angle value in the range of -180 to 180 degrees. Note that
 * this function uses the small angle approximation for values close
 * to zero. This routine calculates the arc tangent with an average
 * error of +/- 0.045 degrees.
 */
float gr_fast_atan2f(float y, float x);

static inline float gr_fast_atan2f(gr_complex z) 
{ 
  return gr_fast_atan2f(z.imag(), z.real()); 
}

#endif /* _GR_MATH_H_ */

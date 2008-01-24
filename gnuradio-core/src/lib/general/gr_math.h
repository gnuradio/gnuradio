/* -*- c++ -*- */
/*
 * Copyright 2003,2005,2008 Free Software Foundation, Inc.
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
 * \ingroup math
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

/* This bounds x by +/- clip without a branch */
static inline float gr_branchless_clip(float x, float clip)
{
  float x1 = fabsf(x+clip);
  float x2 = fabsf(x-clip);
  x1 -= x2;
  return 0.5*x1;
}

static inline float gr_clip(float x, float clip)
{
  float y;
  if(x > clip)
    y = clip;
  else if(x < clip)
    y = -clip;
  return y;
}

// Slicer Functions
static inline unsigned int gr_binary_slicer(float x)
{
  if(x >= 0)
    return 1;
  else
    return 0;
}

static inline unsigned int gr_quad_45deg_slicer(float r, float i)
{
  unsigned int ret = 0;
  if((r >= 0) && (i >= 0))
    ret = 0;
  else if((r < 0) && (i >= 0))
    ret = 1;
  else if((r < 0) && (i < 0))
    ret = 2;
  else 
    ret = 3;
  return ret;
}

static inline unsigned int gr_quad_0deg_slicer(float r, float i)
{
  unsigned int ret = 0;
  if(fabsf(r) > fabsf(i)) {
    if(r > 0)
      ret = 0;
    else
      ret = 2;
  }
  else {
    if(i > 0)
      ret = 1;
    else
      ret = 3;
  }

  return ret;
}

static inline unsigned int gr_quad_45deg_slicer(gr_complex x)
{
  return gr_quad_45deg_slicer(x.real(), x.imag());
}

static inline unsigned int gr_quad_0deg_slicer(gr_complex x)
{
  return gr_quad_0deg_slicer(x.real(), x.imag());
}

// Branchless Slicer Functions
static inline unsigned int gr_branchless_binary_slicer(float x)
{
  return (x >= 0);
}

static inline unsigned int gr_branchless_quad_0deg_slicer(float r, float i)
{
  unsigned int ret = 0;
  ret =  (fabsf(r) > fabsf(i)) * (((r < 0) << 0x1));       // either 0 (00) or 2 (10)
  ret |= (fabsf(i) > fabsf(r)) * (((i < 0) << 0x1) | 0x1); // either 1 (01) or 3 (11)

  return ret;
}

static inline unsigned int gr_branchless_quad_0deg_slicer(gr_complex x)
{
  return gr_branchless_quad_0deg_slicer(x.real(), x.imag());
}

static inline unsigned int gr_branchless_quad_45deg_slicer(float r, float i)
{
  char ret = (r <= 0);
  ret |= ((i <= 0) << 1);
  return (ret ^ ((ret & 0x2) >> 0x1));
}

static inline unsigned int gr_branchless_quad_45deg_slicer(gr_complex x)
{
  return gr_branchless_quad_45deg_slicer(x.real(), x.imag());
}

#endif /* _GR_MATH_H_ */

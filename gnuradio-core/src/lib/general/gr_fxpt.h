/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_FXPT_H
#define INCLUDED_GR_FXPT_H

#include <gr_types.h>

/*!
 * \brief fixed point sine and cosine and friends.
 * \ingroup math
 *
 *   fixed pt	radians
 *  ---------	--------
 *   -2**31       -pi
 *        0         0
 *  2**31-1        pi - epsilon
 *
 */
class gr_fxpt
{
  static const int WORDBITS = 32;
  static const int NBITS = 10;
  static const float s_sine_table[1 << NBITS][2];
  static const float PI = 3.14159265358979323846;
  static const float TWO_TO_THE_31 = 2147483648.0;
public:

  static gr_int32
  float_to_fixed (float x)
  {
    return (gr_int32) ((float) x * TWO_TO_THE_31 / PI);
  }

  static float
  fixed_to_float (gr_int32 x)
  {
    return x * (PI / TWO_TO_THE_31);
  }

  /*!
   * \brief Given a fixed point angle x, return float sine (x)
   */
  static float
  sin (gr_int32 x)
  {
    gr_uint32 ux = x;
    int index = ux >> (WORDBITS - NBITS);
    return s_sine_table[index][0] * (ux >> 1) + s_sine_table[index][1];
  }

  /*
   * \brief Given a fixed point angle x, return float cosine (x)
   */
  static float
  cos (gr_int32 x)
  {
    gr_uint32 ux = x + 0x40000000;
    int index = ux >> (WORDBITS - NBITS);
    return s_sine_table[index][0] * (ux >> 1) + s_sine_table[index][1];
  }

};

#endif /* INCLUDED_GR_FXPT_H */

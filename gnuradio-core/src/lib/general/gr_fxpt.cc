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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_fxpt.h>

const float gr_fxpt::s_sine_table[1 << NBITS][2] = {
#include "sine_table.h"
};

// gcc 4.x fix
const float gr_fxpt::TWO_TO_THE_31;
const float gr_fxpt::PI;

#if 0
/*
 * Compute sine using table lookup with linear interpolation.
 * Each table entry contains slope and intercept.
 */
float
gr_fxpt::sin (gr_int32 x)
{
  gr_uint32 ux = x;
  int index = ux >> (WORDBITS - NBITS);
  return s_sine_table[index][0] * (ux >> 1) + s_sine_table[index][1];
}

float
gr_fxpt::cos (gr_int32 x)
{
  gr_uint32 ux = x + 0x40000000;
  int index = ux >> (WORDBITS - NBITS);
  return s_sine_table[index][0] * (ux >> 1) + s_sine_table[index][1];
}
#endif

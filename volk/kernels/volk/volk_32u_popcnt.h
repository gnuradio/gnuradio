/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VOLK_32u_POPCNT_A16_H
#define INCLUDED_VOLK_32u_POPCNT_A16_H

#include <stdio.h>
#include <inttypes.h>


#ifdef LV_HAVE_GENERIC

static inline void volk_32u_popcnt_generic(uint32_t* ret, const uint32_t value) {

  // This is faster than a lookup table
  uint32_t retVal = value;

  retVal = (retVal & 0x55555555) + (retVal >> 1 & 0x55555555);
  retVal = (retVal & 0x33333333) + (retVal >> 2 & 0x33333333);
  retVal = (retVal + (retVal >> 4)) & 0x0F0F0F0F;
  retVal = (retVal + (retVal >> 8));
  retVal = (retVal + (retVal >> 16)) & 0x0000003F;

  *ret = retVal;
}

#endif /*LV_HAVE_GENERIC*/

#ifdef LV_HAVE_SSE4_2

#include <nmmintrin.h>

static inline void volk_32u_popcnt_a_sse4_2(uint32_t* ret, const uint32_t value) {
  *ret = _mm_popcnt_u32(value);
}

#endif /*LV_HAVE_SSE4_2*/

#endif /*INCLUDED_VOLK_32u_POPCNT_A16_H*/

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

#ifndef INCLUDED_volk_64u_popcnt_a_H
#define INCLUDED_volk_64u_popcnt_a_H

#include <stdio.h>
#include <inttypes.h>


#ifdef LV_HAVE_GENERIC


static inline void volk_64u_popcnt_generic(uint64_t* ret, const uint64_t value) {

  //const uint32_t* valueVector = (const uint32_t*)&value;

  // This is faster than a lookup table
  //uint32_t retVal = valueVector[0];
  uint32_t retVal = (uint32_t)(value & 0x00000000FFFFFFFF);

  retVal = (retVal & 0x55555555) + (retVal >> 1 & 0x55555555);
  retVal = (retVal & 0x33333333) + (retVal >> 2 & 0x33333333);
  retVal = (retVal + (retVal >> 4)) & 0x0F0F0F0F;
  retVal = (retVal + (retVal >> 8));
  retVal = (retVal + (retVal >> 16)) & 0x0000003F;
  uint64_t retVal64  = retVal;

  //retVal = valueVector[1];
  retVal = (uint32_t)((value & 0xFFFFFFFF00000000) >> 31);
  retVal = (retVal & 0x55555555) + (retVal >> 1 & 0x55555555);
  retVal = (retVal & 0x33333333) + (retVal >> 2 & 0x33333333);
  retVal = (retVal + (retVal >> 4)) & 0x0F0F0F0F;
  retVal = (retVal + (retVal >> 8));
  retVal = (retVal + (retVal >> 16)) & 0x0000003F;
  retVal64 += retVal;

  *ret = retVal64;

}

#endif /*LV_HAVE_GENERIC*/

#if LV_HAVE_SSE4_2 && LV_HAVE_64

#include <nmmintrin.h>

static inline void volk_64u_popcnt_a_sse4_2(uint64_t* ret, const uint64_t value) {
  *ret = _mm_popcnt_u64(value);

}

#endif /*LV_HAVE_SSE4_2*/

#if LV_HAVE_NEON
#include <arm_neon.h>
static inline void volk_64u_popcnt_neon(uint64_t* ret, const uint64_t value) {
    uint8x8_t input_val, count8x8_val;
    uint16x4_t count16x4_val;
    uint32x2_t count32x2_val;
    uint64x1_t count64x1_val;

    input_val = vld1_u8((unsigned char *) &value);
    count8x8_val = vcnt_u8(input_val);
    count16x4_val = vpaddl_u8(count8x8_val);
    count32x2_val = vpaddl_u16(count16x4_val);
    count64x1_val = vpaddl_u32(count32x2_val);
    vst1_u64(ret, count64x1_val);

    //*ret = _mm_popcnt_u64(value);

}
#endif /*LV_HAVE_NEON*/

#endif /*INCLUDED_volk_64u_popcnt_a_H*/

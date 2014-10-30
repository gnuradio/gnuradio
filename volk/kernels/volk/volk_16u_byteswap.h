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

#ifndef INCLUDED_volk_16u_byteswap_u_H
#define INCLUDED_volk_16u_byteswap_u_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>

/*!
  \brief Byteswaps (in-place) an unaligned vector of int16_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_16u_byteswap_u_sse2(uint16_t* intsToSwap, unsigned int num_points){
  unsigned int number = 0;
  uint16_t* inputPtr = intsToSwap;
  __m128i input, left, right, output;

  const unsigned int eighthPoints = num_points / 8;
  for(;number < eighthPoints; number++){
    // Load the 16t values, increment inputPtr later since we're doing it in-place.
    input = _mm_loadu_si128((__m128i*)inputPtr);
    // Do the two shifts
    left = _mm_slli_epi16(input, 8);
    right = _mm_srli_epi16(input, 8);
    // Or the left and right halves together
    output = _mm_or_si128(left, right);
    // Store the results
    _mm_storeu_si128((__m128i*)inputPtr, output);
    inputPtr += 8;
  }

  // Byteswap any remaining points:
  number = eighthPoints*8;
  for(; number < num_points; number++){
    uint16_t outputVal = *inputPtr;
    outputVal = (((outputVal >> 8) & 0xff) | ((outputVal << 8) & 0xff00));
    *inputPtr = outputVal;
    inputPtr++;
  }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Byteswaps (in-place) an unaligned vector of int16_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_16u_byteswap_generic(uint16_t* intsToSwap, unsigned int num_points){
  unsigned int point;
  uint16_t* inputPtr = intsToSwap;
  for(point = 0; point < num_points; point++){
    uint16_t output = *inputPtr;
    output = (((output >> 8) & 0xff) | ((output << 8) & 0xff00));
    *inputPtr = output;
    inputPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */

#endif /* INCLUDED_volk_16u_byteswap_u_H */
#ifndef INCLUDED_volk_16u_byteswap_a_H
#define INCLUDED_volk_16u_byteswap_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>

/*!
  \brief Byteswaps (in-place) an aligned vector of int16_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_16u_byteswap_a_sse2(uint16_t* intsToSwap, unsigned int num_points){
  unsigned int number = 0;
  uint16_t* inputPtr = intsToSwap;
  __m128i input, left, right, output;

  const unsigned int eighthPoints = num_points / 8;
  for(;number < eighthPoints; number++){
    // Load the 16t values, increment inputPtr later since we're doing it in-place.
    input = _mm_load_si128((__m128i*)inputPtr);
    // Do the two shifts
    left = _mm_slli_epi16(input, 8);
    right = _mm_srli_epi16(input, 8);
    // Or the left and right halves together
    output = _mm_or_si128(left, right);
    // Store the results
    _mm_store_si128((__m128i*)inputPtr, output);
    inputPtr += 8;
  }


  // Byteswap any remaining points:
  number = eighthPoints*8;
  for(; number < num_points; number++){
    uint16_t outputVal = *inputPtr;
    outputVal = (((outputVal >> 8) & 0xff) | ((outputVal << 8) & 0xff00));
    *inputPtr = outputVal;
    inputPtr++;
  }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
/*!
  \brief Byteswaps (in-place) an unaligned vector of int16_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_16u_byteswap_neon(uint16_t* intsToSwap, unsigned int num_points){
  unsigned int number;
  unsigned int eighth_points = num_points / 8;
  uint16x8_t input, output;
  uint16_t* inputPtr = intsToSwap;

  for(number = 0; number < eighth_points; number++) {
    input = vld1q_u16(inputPtr);
    output = vsriq_n_u16(output, input, 8);
    output = vsliq_n_u16(output, input, 8);
    vst1q_u16(inputPtr, output);
    inputPtr += 8;
  }

  for(number = eighth_points * 8; number < num_points; number++){
    uint16_t output = *inputPtr;
    output = (((output >> 8) & 0xff) | ((output << 8) & 0xff00));
    *inputPtr = output;
    inputPtr++;
  }
}
#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
/*!
  \brief Byteswaps (in-place) an aligned vector of int32_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_16u_byteswap_neon_table(uint16_t* intsToSwap, unsigned int num_points){
  uint16_t* inputPtr = intsToSwap;
  unsigned int number = 0;
  unsigned int n16points = num_points / 16;

  uint8x8x4_t input_table;
  uint8x8_t int_lookup01, int_lookup23, int_lookup45, int_lookup67;
  uint8x8_t swapped_int01, swapped_int23, swapped_int45, swapped_int67;

  /* these magic numbers are used as byte-indeces in the LUT.
     they are pre-computed to save time. A simple C program
     can calculate them; for example for lookup01:
    uint8_t chars[8] = {24, 16, 8, 0, 25, 17, 9, 1};
    for(ii=0; ii < 8; ++ii) {
        index += ((uint64_t)(*(chars+ii))) << (ii*8);
    }
  */
  int_lookup01 = vcreate_u8(1232017111498883080);
  int_lookup23 = vcreate_u8(1376697457175036426);
  int_lookup45 = vcreate_u8(1521377802851189772);
  int_lookup67 = vcreate_u8(1666058148527343118);

  for(number = 0; number < n16points; ++number){
    input_table = vld4_u8((uint8_t*) inputPtr);
    swapped_int01 = vtbl4_u8(input_table, int_lookup01);
    swapped_int23 = vtbl4_u8(input_table, int_lookup23);
    swapped_int45 = vtbl4_u8(input_table, int_lookup45);
    swapped_int67 = vtbl4_u8(input_table, int_lookup67);
    vst1_u8((uint8_t*)inputPtr, swapped_int01);
    vst1_u8((uint8_t*)(inputPtr+4), swapped_int23);
    vst1_u8((uint8_t*)(inputPtr+8), swapped_int45);
    vst1_u8((uint8_t*)(inputPtr+12), swapped_int67);

    inputPtr += 16;
  }

  for(number = n16points * 16; number < num_points; ++number){
    uint16_t output = *inputPtr;
    output = (((output >> 8) & 0xff) | ((output << 8) & 0xff00));
    *inputPtr = output;
    inputPtr++;
  }
}
#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Byteswaps (in-place) an aligned vector of int16_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_16u_byteswap_a_generic(uint16_t* intsToSwap, unsigned int num_points){
  unsigned int point;
  uint16_t* inputPtr = intsToSwap;
  for(point = 0; point < num_points; point++){
    uint16_t output = *inputPtr;
    output = (((output >> 8) & 0xff) | ((output << 8) & 0xff00));
    *inputPtr = output;
    inputPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_ORC
/*!
  \brief Byteswaps (in-place) an aligned vector of int16_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
extern void volk_16u_byteswap_a_orc_impl(uint16_t* intsToSwap, unsigned int num_points);
static inline void volk_16u_byteswap_u_orc(uint16_t* intsToSwap, unsigned int num_points){
    volk_16u_byteswap_a_orc_impl(intsToSwap, num_points);
}
#endif /* LV_HAVE_ORC */


#endif /* INCLUDED_volk_16u_byteswap_a_H */

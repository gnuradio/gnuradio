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

#ifndef INCLUDED_volk_32u_byteswap_u_H
#define INCLUDED_volk_32u_byteswap_u_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>

/*!
  \brief Byteswaps (in-place) an aligned vector of int32_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_32u_byteswap_u_sse2(uint32_t* intsToSwap, unsigned int num_points){
  unsigned int number = 0;

  uint32_t* inputPtr = intsToSwap;
  __m128i input, byte1, byte2, byte3, byte4, output;
  __m128i byte2mask = _mm_set1_epi32(0x00FF0000);
  __m128i byte3mask = _mm_set1_epi32(0x0000FF00);

  const uint64_t quarterPoints = num_points / 4;
  for(;number < quarterPoints; number++){
    // Load the 32t values, increment inputPtr later since we're doing it in-place.
    input = _mm_loadu_si128((__m128i*)inputPtr);
    // Do the four shifts
    byte1 = _mm_slli_epi32(input, 24);
    byte2 = _mm_slli_epi32(input, 8);
    byte3 = _mm_srli_epi32(input, 8);
    byte4 = _mm_srli_epi32(input, 24);
    // Or bytes together
    output = _mm_or_si128(byte1, byte4);
    byte2 = _mm_and_si128(byte2, byte2mask);
    output = _mm_or_si128(output, byte2);
    byte3 = _mm_and_si128(byte3, byte3mask);
    output = _mm_or_si128(output, byte3);
    // Store the results
    _mm_storeu_si128((__m128i*)inputPtr, output);
    inputPtr += 4;
  }

  // Byteswap any remaining points:
  number = quarterPoints*4;
  for(; number < num_points; number++){
    uint32_t outputVal = *inputPtr;
    outputVal = (((outputVal >> 24) & 0xff) | ((outputVal >> 8) & 0x0000ff00) | ((outputVal << 8) & 0x00ff0000) | ((outputVal << 24) & 0xff000000));
    *inputPtr = outputVal;
    inputPtr++;
  }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
/*!
  \brief Byteswaps (in-place) an aligned vector of int32_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_32u_byteswap_neon(uint32_t* intsToSwap, unsigned int num_points){
  uint32_t* inputPtr = intsToSwap;
  unsigned int number = 0;
  unsigned int n8points = num_points / 8;

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
  int_lookup01 = vcreate_u8(74609667900706840);
  int_lookup23 = vcreate_u8(219290013576860186);
  int_lookup45 = vcreate_u8(363970359253013532);
  int_lookup67 = vcreate_u8(508650704929166878);
  
  for(number = 0; number < n8points; ++number){
    input_table = vld4_u8((uint8_t*) inputPtr);
    swapped_int01 = vtbl4_u8(input_table, int_lookup01);
    swapped_int23 = vtbl4_u8(input_table, int_lookup23);
    swapped_int45 = vtbl4_u8(input_table, int_lookup45);
    swapped_int67 = vtbl4_u8(input_table, int_lookup67);
    vst1_u8((uint8_t*) inputPtr, swapped_int01);
    vst1_u8((uint8_t*) (inputPtr+2), swapped_int23);
    vst1_u8((uint8_t*) (inputPtr+4), swapped_int45);
    vst1_u8((uint8_t*) (inputPtr+6), swapped_int67);

    inputPtr += 8;
  }

  for(number = n8points * 8; number < num_points; ++number){
    uint32_t output = *inputPtr;
    output = (((output >> 24) & 0xff) | ((output >> 8) & 0x0000ff00) | ((output << 8) & 0x00ff0000) | ((output << 24) & 0xff000000));

    *inputPtr = output;
    inputPtr++;
  }
}
#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Byteswaps (in-place) an aligned vector of int32_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_32u_byteswap_generic(uint32_t* intsToSwap, unsigned int num_points){
  uint32_t* inputPtr = intsToSwap;

  unsigned int point;
  for(point = 0; point < num_points; point++){
    uint32_t output = *inputPtr;
    output = (((output >> 24) & 0xff) | ((output >> 8) & 0x0000ff00) | ((output << 8) & 0x00ff0000) | ((output << 24) & 0xff000000));

    *inputPtr = output;
    inputPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */


#endif /* INCLUDED_volk_32u_byteswap_u_H */
#ifndef INCLUDED_volk_32u_byteswap_a_H
#define INCLUDED_volk_32u_byteswap_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>

/*!
  \brief Byteswaps (in-place) an aligned vector of int32_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_32u_byteswap_a_sse2(uint32_t* intsToSwap, unsigned int num_points){
  unsigned int number = 0;

  uint32_t* inputPtr = intsToSwap;
  __m128i input, byte1, byte2, byte3, byte4, output;
  __m128i byte2mask = _mm_set1_epi32(0x00FF0000);
  __m128i byte3mask = _mm_set1_epi32(0x0000FF00);

  const uint64_t quarterPoints = num_points / 4;
  for(;number < quarterPoints; number++){
    // Load the 32t values, increment inputPtr later since we're doing it in-place.
    input = _mm_load_si128((__m128i*)inputPtr);
    // Do the four shifts
    byte1 = _mm_slli_epi32(input, 24);
    byte2 = _mm_slli_epi32(input, 8);
    byte3 = _mm_srli_epi32(input, 8);
    byte4 = _mm_srli_epi32(input, 24);
    // Or bytes together
    output = _mm_or_si128(byte1, byte4);
    byte2 = _mm_and_si128(byte2, byte2mask);
    output = _mm_or_si128(output, byte2);
    byte3 = _mm_and_si128(byte3, byte3mask);
    output = _mm_or_si128(output, byte3);
    // Store the results
    _mm_store_si128((__m128i*)inputPtr, output);
    inputPtr += 4;
  }

  // Byteswap any remaining points:
  number = quarterPoints*4;
  for(; number < num_points; number++){
    uint32_t outputVal = *inputPtr;
    outputVal = (((outputVal >> 24) & 0xff) | ((outputVal >> 8) & 0x0000ff00) | ((outputVal << 8) & 0x00ff0000) | ((outputVal << 24) & 0xff000000));
    *inputPtr = outputVal;
    inputPtr++;
  }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Byteswaps (in-place) an aligned vector of int32_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_32u_byteswap_a_generic(uint32_t* intsToSwap, unsigned int num_points){
  uint32_t* inputPtr = intsToSwap;

  unsigned int point;
  for(point = 0; point < num_points; point++){
    uint32_t output = *inputPtr;
    output = (((output >> 24) & 0xff) | ((output >> 8) & 0x0000ff00) | ((output << 8) & 0x00ff0000) | ((output << 24) & 0xff000000));

    *inputPtr = output;
    inputPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32u_byteswap_a_H */

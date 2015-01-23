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

#ifndef INCLUDED_volk_32f_binary_slicer_8i_H
#define INCLUDED_volk_32f_binary_slicer_8i_H


#ifdef LV_HAVE_GENERIC
/*!
  \brief Returns integer 1 if float input is greater than or equal to 0, 1 otherwise
  \param cVector The char (int8_t) output (either 0 or 1)
  \param aVector The float input
  \param num_points The number of values in aVector and stored into cVector
*/
static inline void
volk_32f_binary_slicer_8i_generic(int8_t* cVector, const float* aVector,
                                  unsigned int num_points)
{
  int8_t* cPtr = cVector;
  const float* aPtr = aVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++) {
    if(*aPtr++ >= 0) {
      *cPtr++ = 1;
    }
    else {
      *cPtr++ = 0;
    }
  }
}
#endif /* LV_HAVE_GENERIC */


#ifdef LV_HAVE_GENERIC
/*!
  \brief Returns integer 1 if float input is greater than or equal to 0, 1 otherwise
  \param cVector The char (int8_t) output (either 0 or 1)
  \param aVector The float input
  \param num_points The number of values in aVector and stored into cVector
*/
static inline void
volk_32f_binary_slicer_8i_generic_branchless(int8_t* cVector, const float* aVector,
                                             unsigned int num_points)
{
  int8_t* cPtr = cVector;
  const float* aPtr = aVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *cPtr++ = (*aPtr++ >= 0);
  }
}
#endif /* LV_HAVE_GENERIC */


#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
/*!
  \brief Returns integer 1 if float input is greater than or equal to 0, 1 otherwise
  \param cVector The char (int8_t) output (either 0 or 1)
  \param aVector The float input
  \param num_points The number of values in aVector and stored into cVector
*/
static inline void
volk_32f_binary_slicer_8i_a_sse2(int8_t* cVector, const float* aVector,
                                 unsigned int num_points)
{
  int8_t* cPtr = cVector;
  const float* aPtr = aVector;
  unsigned int number = 0;

  unsigned int n16points = num_points / 16;
  __m128 a0_val, a1_val, a2_val, a3_val;
  __m128 res0_f, res1_f, res2_f, res3_f;
  __m128i res0_i, res1_i, res2_i, res3_i;
  __m128 zero_val;
  zero_val = _mm_set1_ps(0.0f);

  for(number = 0; number < n16points; number++) {
    a0_val = _mm_load_ps(aPtr);
    a1_val = _mm_load_ps(aPtr+4);
    a2_val = _mm_load_ps(aPtr+8);
    a3_val = _mm_load_ps(aPtr+12);

    // compare >= 0; return float
    res0_f = _mm_cmpge_ps(a0_val, zero_val);
    res1_f = _mm_cmpge_ps(a1_val, zero_val);
    res2_f = _mm_cmpge_ps(a2_val, zero_val);
    res3_f = _mm_cmpge_ps(a3_val, zero_val);

    // convert to 32i and >> 31
    res0_i = _mm_srli_epi32(_mm_cvtps_epi32(res0_f), 31);
    res1_i = _mm_srli_epi32(_mm_cvtps_epi32(res1_f), 31);
    res2_i = _mm_srli_epi32(_mm_cvtps_epi32(res2_f), 31);
    res3_i = _mm_srli_epi32(_mm_cvtps_epi32(res3_f), 31);

    // pack into 16-bit results
    res0_i = _mm_packs_epi32(res0_i, res1_i);
    res2_i = _mm_packs_epi32(res2_i, res3_i);

    // pack into 8-bit results
    res0_i = _mm_packs_epi16(res0_i, res2_i);

    _mm_store_si128((__m128i*)cPtr, res0_i);

    cPtr += 16;
    aPtr += 16;
  }

  for(number = n16points * 16; number < num_points; number++) {
    if( *aPtr++ >= 0) {
      *cPtr++ = 1;
    }
    else {
      *cPtr++ = 0;
    }
  }
}
#endif /* LV_HAVE_SSE2 */



#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
/*!
  \brief Returns integer 1 if float input is greater than or equal to 0, 1 otherwise
  \param cVector The char (int8_t) output (either 0 or 1)
  \param aVector The float input
  \param num_points The number of values in aVector and stored into cVector
*/
static inline void
volk_32f_binary_slicer_8i_u_sse2(int8_t* cVector, const float* aVector,
                                  unsigned int num_points)
{
  int8_t* cPtr = cVector;
  const float* aPtr = aVector;
  unsigned int number = 0;

  unsigned int n16points = num_points / 16;
  __m128 a0_val, a1_val, a2_val, a3_val;
  __m128 res0_f, res1_f, res2_f, res3_f;
  __m128i res0_i, res1_i, res2_i, res3_i;
  __m128 zero_val;
  zero_val = _mm_set1_ps (0.0f);

  for(number = 0; number < n16points; number++) {
    a0_val = _mm_loadu_ps(aPtr);
    a1_val = _mm_loadu_ps(aPtr+4);
    a2_val = _mm_loadu_ps(aPtr+8);
    a3_val = _mm_loadu_ps(aPtr+12);

    // compare >= 0; return float
    res0_f = _mm_cmpge_ps(a0_val, zero_val);
    res1_f = _mm_cmpge_ps(a1_val, zero_val);
    res2_f = _mm_cmpge_ps(a2_val, zero_val);
    res3_f = _mm_cmpge_ps(a3_val, zero_val);

    // convert to 32i and >> 31
    res0_i = _mm_srli_epi32(_mm_cvtps_epi32(res0_f), 31);
    res1_i = _mm_srli_epi32(_mm_cvtps_epi32(res1_f), 31);
    res2_i = _mm_srli_epi32(_mm_cvtps_epi32(res2_f), 31);
    res3_i = _mm_srli_epi32(_mm_cvtps_epi32(res3_f), 31);

    // pack into 16-bit results
    res0_i = _mm_packs_epi32(res0_i, res1_i);
    res2_i = _mm_packs_epi32(res2_i, res3_i);

    // pack into 8-bit results
    res0_i = _mm_packs_epi16(res0_i, res2_i);

    _mm_storeu_si128((__m128i*)cPtr, res0_i);

    cPtr += 16;
    aPtr += 16;
  }

  for(number = n16points * 16; number < num_points; number++) {
    if( *aPtr++ >= 0) {
      *cPtr++ = 1;
    }
    else {
      *cPtr++ = 0;
    }
  }
}
#endif /* LV_HAVE_SSE2 */


#ifdef LV_HAVE_NEON
#include <arm_neon.h>
/*!
  \brief Returns integer 1 if float input is greater than or equal to 0, 1 otherwise
  \param cVector The char (int8_t) output (either 0 or 1)
  \param aVector The float input
  \param num_points The number of values in aVector and stored into cVector
*/
static inline void
volk_32f_binary_slicer_8i_neon(int8_t* cVector, const float* aVector,
                                  unsigned int num_points)
{
  int8_t* cPtr = cVector;
  const float* aPtr = aVector;
  unsigned int number = 0;
  unsigned int n16points = num_points / 16;

  float32x4x2_t input_val0, input_val1;
  float32x4_t zero_val;
  uint32x4x2_t res0_u32, res1_u32;
  uint16x4x2_t res0_u16x4, res1_u16x4;
  uint16x8x2_t res_u16x8;
  uint8x8x2_t res_u8;
  uint8x8_t one;

  zero_val = vdupq_n_f32(0.0);
  one = vdup_n_u8(0x01);
  
  // TODO: this is a good candidate for asm because the vcombines
  // can be eliminated simply by picking dst registers that are
  // adjacent.
  for(number = 0; number < n16points; number++) {
    input_val0 = vld2q_f32(aPtr);
    input_val1 = vld2q_f32(aPtr+8);

    // test against 0; return uint32
    res0_u32.val[0] = vcgeq_f32(input_val0.val[0], zero_val);
    res0_u32.val[1] = vcgeq_f32(input_val0.val[1], zero_val);
    res1_u32.val[0] = vcgeq_f32(input_val1.val[0], zero_val);
    res1_u32.val[1] = vcgeq_f32(input_val1.val[1], zero_val);

    // narrow uint32 -> uint16 followed by combine to 8-element vectors
    res0_u16x4.val[0] = vmovn_u32(res0_u32.val[0]);
    res0_u16x4.val[1] = vmovn_u32(res0_u32.val[1]);
    res1_u16x4.val[0] = vmovn_u32(res1_u32.val[0]);
    res1_u16x4.val[1] = vmovn_u32(res1_u32.val[1]);

    res_u16x8.val[0] = vcombine_u16(res0_u16x4.val[0], res1_u16x4.val[0]);
    res_u16x8.val[1] = vcombine_u16(res0_u16x4.val[1], res1_u16x4.val[1]);
    
    // narrow uint16x8 -> uint8x8
    res_u8.val[0] = vmovn_u16(res_u16x8.val[0]);
    res_u8.val[1] = vmovn_u16(res_u16x8.val[1]);
    // we *could* load twice as much data and do another vcombine here
    // to get a uint8x16x2 vector, still only do 2 vandqs and a single store
    // but that turns out to be ~16% slower than this version on zc702
    // it's possible register contention in GCC scheduler slows it down
    // and a hand-written asm with quad-word u8 registers is much faster.

    res_u8.val[0] = vand_u8(one, res_u8.val[0]);
    res_u8.val[1] = vand_u8(one, res_u8.val[1]);

    vst2_u8((unsigned char*)cPtr, res_u8);
    cPtr += 16;
    aPtr += 16;

  }

  for(number = n16points * 16; number < num_points; number++) {
    if(*aPtr++ >= 0) {
      *cPtr++ = 1;
    }
    else {
      *cPtr++ = 0;
    }
  }
}
#endif /* LV_HAVE_NEON */


#endif /* INCLUDED_volk_32f_binary_slicer_8i_H */

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

#ifndef INCLUDED_VOLK_8sc_DEINTERLEAVE_REAL_8s_ALIGNED8_H
#define INCLUDED_VOLK_8sc_DEINTERLEAVE_REAL_8s_ALIGNED8_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSSE3
#include <tmmintrin.h>
/*!
  \brief Deinterleaves the complex 8 bit vector into I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_deinterleave_real_8i_a_ssse3(int8_t* iBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int8_t* complexVectorPtr = (int8_t*)complexVector;
  int8_t* iBufferPtr = iBuffer;
  __m128i moveMask1 = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0);
  __m128i moveMask2 = _mm_set_epi8(14, 12, 10, 8, 6, 4, 2, 0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
  __m128i complexVal1, complexVal2, outputVal;

  unsigned int sixteenthPoints = num_points / 16;

  for(number = 0; number < sixteenthPoints; number++){
    complexVal1 = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 16;
    complexVal2 = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 16;

    complexVal1 = _mm_shuffle_epi8(complexVal1, moveMask1);
    complexVal2 = _mm_shuffle_epi8(complexVal2, moveMask2);

    outputVal = _mm_or_si128(complexVal1, complexVal2);

    _mm_store_si128((__m128i*)iBufferPtr, outputVal);
    iBufferPtr += 16;
  }

  number = sixteenthPoints * 16;
  for(; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_SSSE3 */

#ifdef LV_HAVE_AVX
#include <immintrin.h>
/*!
  \brief Deinterleaves the complex 8 bit vector into I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/

static inline void volk_8ic_deinterleave_real_8i_a_avx(int8_t* iBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int8_t* complexVectorPtr = (int8_t*)complexVector;
  int8_t* iBufferPtr = iBuffer;
  __m128i moveMaskL = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0);
  __m128i moveMaskH = _mm_set_epi8(14, 12, 10, 8, 6, 4, 2, 0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
  __m256i complexVal1, complexVal2, outputVal;
  __m128i complexVal1H, complexVal1L, complexVal2H, complexVal2L, outputVal1, outputVal2;

  unsigned int thirtysecondPoints = num_points / 32;

  for(number = 0; number < thirtysecondPoints; number++){

    complexVal1 = _mm256_load_si256((__m256i*)complexVectorPtr);
    complexVectorPtr += 32;
    complexVal2 = _mm256_load_si256((__m256i*)complexVectorPtr);
    complexVectorPtr += 32;

    complexVal1H = _mm256_extractf128_si256(complexVal1, 1);
    complexVal1L = _mm256_extractf128_si256(complexVal1, 0);
    complexVal2H = _mm256_extractf128_si256(complexVal2, 1);
    complexVal2L = _mm256_extractf128_si256(complexVal2, 0);

    complexVal1H = _mm_shuffle_epi8(complexVal1H, moveMaskH);
    complexVal1L = _mm_shuffle_epi8(complexVal1L, moveMaskL);
    outputVal1 = _mm_or_si128(complexVal1H, complexVal1L);


    complexVal2H = _mm_shuffle_epi8(complexVal2H, moveMaskH);
    complexVal2L = _mm_shuffle_epi8(complexVal2L, moveMaskL);
    outputVal2 = _mm_or_si128(complexVal2H, complexVal2L);

    __m256i dummy = _mm256_setzero_si256();
    outputVal = _mm256_insertf128_si256(dummy, outputVal1, 0);
    outputVal = _mm256_insertf128_si256(outputVal, outputVal2, 1);


    _mm256_store_si256((__m256i*)iBufferPtr, outputVal);
    iBufferPtr += 32;
  }

  number = thirtysecondPoints * 32;
  for(; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Deinterleaves the complex 8 bit vector into I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_deinterleave_real_8i_generic(int8_t* iBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int8_t* complexVectorPtr = (int8_t*)complexVector;
  int8_t* iBufferPtr = iBuffer;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
/*!
  \brief Deinterleaves the complex 8 bit vector into I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_deinterleave_real_8i_neon(int8_t* iBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
    unsigned int number;
    unsigned int sixteenth_points = num_points / 16;

    int8x16x2_t input_vector;
    for(number=0; number < sixteenth_points; ++number) {
        input_vector = vld2q_s8((int8_t*) complexVector );
        vst1q_s8(iBuffer, input_vector.val[0]);
        iBuffer += 16;
        complexVector += 16;
    }

    const int8_t* complexVectorPtr = (int8_t*)complexVector;
    int8_t* iBufferPtr = iBuffer;
    for(number = sixteenth_points*16; number < num_points; number++){
      *iBufferPtr++ = *complexVectorPtr++;
      complexVectorPtr++;
    }
}
#endif /* LV_HAVE_NEON */



#endif /* INCLUDED_VOLK_8sc_DEINTERLEAVE_REAL_8s_ALIGNED8_H */

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

#ifndef INCLUDED_volk_8ic_deinterleave_real_16i_a_H
#define INCLUDED_volk_8ic_deinterleave_real_16i_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Deinterleaves the complex 8 bit vector into I 16 bit vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_deinterleave_real_16i_a_sse4_1(int16_t* iBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int8_t* complexVectorPtr = (int8_t*)complexVector;
  int16_t* iBufferPtr = iBuffer;
  __m128i moveMask = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0);
  __m128i complexVal, outputVal;

  unsigned int eighthPoints = num_points / 8;

  for(number = 0; number < eighthPoints; number++){
    complexVal = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 16;

    complexVal = _mm_shuffle_epi8(complexVal, moveMask);

    outputVal = _mm_cvtepi8_epi16(complexVal);
    outputVal = _mm_slli_epi16(outputVal, 7);

    _mm_store_si128((__m128i*)iBufferPtr, outputVal);
    iBufferPtr += 8;
  }

  number = eighthPoints * 8;
  for(; number < num_points; number++){
    *iBufferPtr++ = ((int16_t)*complexVectorPtr++) * 128;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_SSE4_1 */

#ifdef LV_HAVE_AVX
#include <immintrin.h>
/*!
  \brief Deinterleaves the complex 8 bit vector into I 16 bit vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_deinterleave_real_16i_a_avx(int16_t* iBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int8_t* complexVectorPtr = (int8_t*)complexVector;
  int16_t* iBufferPtr = iBuffer;
  __m128i moveMask = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0);
  __m256i complexVal, outputVal;
  __m128i complexVal1, complexVal0, outputVal1, outputVal0;

  unsigned int sixteenthPoints = num_points / 16;

  for(number = 0; number < sixteenthPoints; number++){
    complexVal = _mm256_load_si256((__m256i*)complexVectorPtr);  complexVectorPtr += 32;

    complexVal1 = _mm256_extractf128_si256(complexVal, 1);
    complexVal0 = _mm256_extractf128_si256(complexVal, 0);

    outputVal1 = _mm_shuffle_epi8(complexVal1, moveMask);
    outputVal0 = _mm_shuffle_epi8(complexVal0, moveMask);

    outputVal1 = _mm_cvtepi8_epi16(outputVal1);
    outputVal1 = _mm_slli_epi16(outputVal1, 7);
    outputVal0 = _mm_cvtepi8_epi16(outputVal0);
    outputVal0 = _mm_slli_epi16(outputVal0, 7);

    __m256i dummy = _mm256_setzero_si256();
    outputVal = _mm256_insertf128_si256(dummy, outputVal0, 0);
    outputVal = _mm256_insertf128_si256(outputVal, outputVal1, 1);
    _mm256_store_si256((__m256i*)iBufferPtr, outputVal);

    iBufferPtr += 16;
  }

  number = sixteenthPoints * 16;
  for(; number < num_points; number++){
    *iBufferPtr++ = ((int16_t)*complexVectorPtr++) * 128;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_AVX */


#ifdef LV_HAVE_GENERIC
/*!
  \brief Deinterleaves the complex 8 bit vector into I 16 bit vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_deinterleave_real_16i_generic(int16_t* iBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int8_t* complexVectorPtr = (const int8_t*)complexVector;
  int16_t* iBufferPtr = iBuffer;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = ((int16_t)(*complexVectorPtr++)) * 128;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_8ic_deinterleave_real_16i_a_H */

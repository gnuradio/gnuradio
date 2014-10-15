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

#ifndef INCLUDED_volk_8ic_deinterleave_16i_x2_a_H
#define INCLUDED_volk_8ic_deinterleave_16i_x2_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Deinterleaves the complex 8 bit vector into I & Q 16 bit vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param qBuffer The Q buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/

static inline void volk_8ic_deinterleave_16i_x2_a_sse4_1(int16_t* iBuffer, int16_t* qBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int8_t* complexVectorPtr = (int8_t*)complexVector;
  int16_t* iBufferPtr = iBuffer;
  int16_t* qBufferPtr = qBuffer;
  __m128i iMoveMask = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0);  // set 16 byte values
  __m128i qMoveMask = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 15, 13, 11, 9, 7, 5, 3, 1);
  __m128i complexVal, iOutputVal, qOutputVal;

  unsigned int eighthPoints = num_points / 8;

  for(number = 0; number < eighthPoints; number++){
    complexVal = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 16;   // aligned load

    iOutputVal = _mm_shuffle_epi8(complexVal, iMoveMask);   // shuffle 16 bytes of 128bit complexVal
    qOutputVal = _mm_shuffle_epi8(complexVal, qMoveMask);

    iOutputVal = _mm_cvtepi8_epi16(iOutputVal);     // fills 2-byte sign extended versions of lower 8 bytes of input to output
    iOutputVal = _mm_slli_epi16(iOutputVal, 8);     // shift in left by 8 bits, each of the 8 16-bit integers, shift in with zeros

    qOutputVal = _mm_cvtepi8_epi16(qOutputVal);
    qOutputVal = _mm_slli_epi16(qOutputVal, 8);

    _mm_store_si128((__m128i*)iBufferPtr, iOutputVal);  // aligned store
    _mm_store_si128((__m128i*)qBufferPtr, qOutputVal);

    iBufferPtr += 8;
    qBufferPtr += 8;
  }

  number = eighthPoints * 8;
  for(; number < num_points; number++){
    *iBufferPtr++ = ((int16_t)*complexVectorPtr++) * 256;   // load 8 bit Complexvector into 16 bit, shift left by 8 bits and store
    *qBufferPtr++ = ((int16_t)*complexVectorPtr++) * 256;
  }
}
#endif /* LV_HAVE_SSE4_1 */

#ifdef LV_HAVE_AVX
#include <immintrin.h>
/*!
  \brief Deinterleaves the complex 8 bit vector into I & Q 16 bit vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param qBuffer The Q buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_deinterleave_16i_x2_a_avx(int16_t* iBuffer, int16_t* qBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int8_t* complexVectorPtr = (int8_t*)complexVector;
  int16_t* iBufferPtr = iBuffer;
  int16_t* qBufferPtr = qBuffer;
  __m128i iMoveMask = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0);  // set 16 byte values
  __m128i qMoveMask = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 15, 13, 11, 9, 7, 5, 3, 1);
  __m256i complexVal, iOutputVal, qOutputVal;
  __m128i complexVal1, complexVal0;
  __m128i iOutputVal1, iOutputVal0, qOutputVal1, qOutputVal0;

  unsigned int sixteenthPoints = num_points / 16;

  for(number = 0; number < sixteenthPoints; number++){
    complexVal = _mm256_load_si256((__m256i*)complexVectorPtr);  complexVectorPtr += 32;    // aligned load

    // Extract from complexVal to iOutputVal and qOutputVal
    complexVal1 = _mm256_extractf128_si256(complexVal, 1);
    complexVal0 = _mm256_extractf128_si256(complexVal, 0);

    iOutputVal1 = _mm_shuffle_epi8(complexVal1, iMoveMask);     // shuffle 16 bytes of 128bit complexVal
    iOutputVal0 = _mm_shuffle_epi8(complexVal0, iMoveMask);
    qOutputVal1 = _mm_shuffle_epi8(complexVal1, qMoveMask);
    qOutputVal0 = _mm_shuffle_epi8(complexVal0, qMoveMask);

    iOutputVal1 = _mm_cvtepi8_epi16(iOutputVal1);   // fills 2-byte sign extended versions of lower 8 bytes of input to output
    iOutputVal1 = _mm_slli_epi16(iOutputVal1, 8);   // shift in left by 8 bits, each of the 8 16-bit integers, shift in with zeros
    iOutputVal0 = _mm_cvtepi8_epi16(iOutputVal0);
    iOutputVal0 = _mm_slli_epi16(iOutputVal0, 8);

    qOutputVal1 = _mm_cvtepi8_epi16(qOutputVal1);
    qOutputVal1 = _mm_slli_epi16(qOutputVal1, 8);
    qOutputVal0 = _mm_cvtepi8_epi16(qOutputVal0);
    qOutputVal0 = _mm_slli_epi16(qOutputVal0, 8);

    // Pack iOutputVal0,1 to iOutputVal
    __m256i dummy = _mm256_setzero_si256();
    iOutputVal = _mm256_insertf128_si256(dummy, iOutputVal0, 0);
    iOutputVal = _mm256_insertf128_si256(iOutputVal, iOutputVal1, 1);
    qOutputVal = _mm256_insertf128_si256(dummy, qOutputVal0, 0);
    qOutputVal = _mm256_insertf128_si256(qOutputVal, qOutputVal1, 1);

    _mm256_store_si256((__m256i*)iBufferPtr, iOutputVal);   // aligned store
    _mm256_store_si256((__m256i*)qBufferPtr, qOutputVal);

    iBufferPtr += 16;
    qBufferPtr += 16;
  }

  number = sixteenthPoints * 16;
  for(; number < num_points; number++){
    *iBufferPtr++ = ((int16_t)*complexVectorPtr++) * 256;   // load 8 bit Complexvector into 16 bit, shift left by 8 bits and store
    *qBufferPtr++ = ((int16_t)*complexVectorPtr++) * 256;
  }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Deinterleaves the complex 8 bit vector into I & Q 16 bit vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param qBuffer The Q buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_deinterleave_16i_x2_generic(int16_t* iBuffer, int16_t* qBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
  const int8_t* complexVectorPtr = (const int8_t*)complexVector;
  int16_t* iBufferPtr = iBuffer;
  int16_t* qBufferPtr = qBuffer;
  unsigned int number;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = (int16_t)(*complexVectorPtr++)*256;
    *qBufferPtr++ = (int16_t)(*complexVectorPtr++)*256;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_8ic_deinterleave_16i_x2_a_H */

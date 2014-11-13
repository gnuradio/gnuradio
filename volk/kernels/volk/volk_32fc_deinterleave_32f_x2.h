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

#ifndef INCLUDED_volk_32fc_deinterleave_32f_x2_a_H
#define INCLUDED_volk_32fc_deinterleave_32f_x2_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_AVX
#include <immintrin.h>
/*!
  \brief Deinterleaves the complex vector into I & Q vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param qBuffer The Q buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_32fc_deinterleave_32f_x2_a_avx(float* iBuffer, float* qBuffer, const lv_32fc_t* complexVector, unsigned int num_points){
  const float* complexVectorPtr = (float*)complexVector;
  float* iBufferPtr = iBuffer;
  float* qBufferPtr = qBuffer;

  unsigned int number = 0;
  // Mask for real and imaginary parts
  const unsigned int eighthPoints = num_points / 8;
  __m256 cplxValue1, cplxValue2, complex1, complex2, iValue, qValue;
  for(;number < eighthPoints; number++){

    cplxValue1 = _mm256_load_ps(complexVectorPtr);
    complexVectorPtr += 8;

    cplxValue2 = _mm256_load_ps(complexVectorPtr);
    complexVectorPtr += 8;

    complex1 = _mm256_permute2f128_ps(cplxValue1, cplxValue2, 0x20);
    complex2 = _mm256_permute2f128_ps(cplxValue1, cplxValue2, 0x31);

    // Arrange in i1i2i3i4 format
    iValue = _mm256_shuffle_ps(complex1, complex2, 0x88);
    // Arrange in q1q2q3q4 format
    qValue = _mm256_shuffle_ps(complex1, complex2, 0xdd);

    _mm256_store_ps(iBufferPtr, iValue);
    _mm256_store_ps(qBufferPtr, qValue);

    iBufferPtr += 8;
    qBufferPtr += 8;
  }

  number = eighthPoints * 8;
  for(; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
    *qBufferPtr++ = *complexVectorPtr++;
  }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Deinterleaves the complex vector into I & Q vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param qBuffer The Q buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_32fc_deinterleave_32f_x2_a_sse(float* iBuffer, float* qBuffer, const lv_32fc_t* complexVector, unsigned int num_points){
  const float* complexVectorPtr = (float*)complexVector;
  float* iBufferPtr = iBuffer;
  float* qBufferPtr = qBuffer;

  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;
  __m128 cplxValue1, cplxValue2, iValue, qValue;
  for(;number < quarterPoints; number++){

    cplxValue1 = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;

    cplxValue2 = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;

    // Arrange in i1i2i3i4 format
    iValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(2,0,2,0));
    // Arrange in q1q2q3q4 format
    qValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(3,1,3,1));

    _mm_store_ps(iBufferPtr, iValue);
    _mm_store_ps(qBufferPtr, qValue);

    iBufferPtr += 4;
    qBufferPtr += 4;
  }

  number = quarterPoints * 4;
  for(; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
    *qBufferPtr++ = *complexVectorPtr++;
  }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
/*!
  \brief Deinterleaves the complex vector into I & Q vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param qBuffer The Q buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_32fc_deinterleave_32f_x2_neon(float* iBuffer, float* qBuffer, const lv_32fc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  unsigned int quarter_points = num_points / 4;
  const float* complexVectorPtr = (float*)complexVector;
  float* iBufferPtr = iBuffer;
  float* qBufferPtr = qBuffer;
  float32x4x2_t complexInput;

  for(number = 0; number < quarter_points; number++){
    complexInput = vld2q_f32(complexVectorPtr);
    vst1q_f32( iBufferPtr, complexInput.val[0] );
    vst1q_f32( qBufferPtr, complexInput.val[1] );
    complexVectorPtr += 8;
    iBufferPtr += 4;
    qBufferPtr += 4;
  }

  for(number = quarter_points*4; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
    *qBufferPtr++ = *complexVectorPtr++;
  }
}
#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Deinterleaves the complex vector into I & Q vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param qBuffer The Q buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_32fc_deinterleave_32f_x2_generic(float* iBuffer, float* qBuffer, const lv_32fc_t* complexVector, unsigned int num_points){
  const float* complexVectorPtr = (float*)complexVector;
  float* iBufferPtr = iBuffer;
  float* qBufferPtr = qBuffer;
  unsigned int number;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
    *qBufferPtr++ = *complexVectorPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32fc_deinterleave_32f_x2_a_H */

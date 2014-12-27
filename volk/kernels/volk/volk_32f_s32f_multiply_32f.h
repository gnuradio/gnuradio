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

#ifndef INCLUDED_volk_32f_s32f_multiply_32f_u_H
#define INCLUDED_volk_32f_s32f_multiply_32f_u_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Scalar float multiply
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param scalar the scalar value
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_s32f_multiply_32f_u_sse(float* cVector, const float* aVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    float* cPtr = cVector;
    const float* aPtr = aVector;

    __m128 aVal, bVal, cVal;
    bVal = _mm_set_ps1(scalar);
    for(;number < quarterPoints; number++){

      aVal = _mm_loadu_ps(aPtr);

      cVal = _mm_mul_ps(aVal, bVal);

      _mm_storeu_ps(cPtr,cVal); // Store the results back into the C container

      aPtr += 4;
      cPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
      *cPtr++ = (*aPtr++) * scalar;
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_AVX
#include <immintrin.h>
/*!
  \brief Scalar float multiply
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param scalar the scalar value
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_s32f_multiply_32f_u_avx(float* cVector, const float* aVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int eighthPoints = num_points / 8;

    float* cPtr = cVector;
    const float* aPtr = aVector;

    __m256 aVal, bVal, cVal;
    bVal = _mm256_set1_ps(scalar);
    for(;number < eighthPoints; number++){

      aVal = _mm256_loadu_ps(aPtr);

      cVal = _mm256_mul_ps(aVal, bVal);

      _mm256_storeu_ps(cPtr,cVal); // Store the results back into the C container

      aPtr += 8;
      cPtr += 8;
    }

    number = eighthPoints * 8;
    for(;number < num_points; number++){
      *cPtr++ = (*aPtr++) * scalar;
    }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Scalar float multiply
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param scalar the scalar value
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_s32f_multiply_32f_generic(float* cVector, const float* aVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;
  const float* inputPtr = aVector;
  float* outputPtr = cVector;
  for(number = 0; number < num_points; number++){
    *outputPtr = (*inputPtr) * scalar;
    inputPtr++;
    outputPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */


#endif /* INCLUDED_volk_32f_s32f_multiply_32f_u_H */
#ifndef INCLUDED_volk_32f_s32f_multiply_32f_a_H
#define INCLUDED_volk_32f_s32f_multiply_32f_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Scalar float multiply
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param scalar the scalar value
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_s32f_multiply_32f_a_sse(float* cVector, const float* aVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    float* cPtr = cVector;
    const float* aPtr = aVector;

    __m128 aVal, bVal, cVal;
    bVal = _mm_set_ps1(scalar);
    for(;number < quarterPoints; number++){

      aVal = _mm_load_ps(aPtr);

      cVal = _mm_mul_ps(aVal, bVal);

      _mm_store_ps(cPtr,cVal); // Store the results back into the C container

      aPtr += 4;
      cPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
      *cPtr++ = (*aPtr++) * scalar;
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_AVX
#include <immintrin.h>
/*!
  \brief Scalar float multiply
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param scalar the scalar value
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_s32f_multiply_32f_a_avx(float* cVector, const float* aVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int eighthPoints = num_points / 8;

    float* cPtr = cVector;
    const float* aPtr = aVector;

    __m256 aVal, bVal, cVal;
    bVal = _mm256_set1_ps(scalar);
    for(;number < eighthPoints; number++){

      aVal = _mm256_load_ps(aPtr);

      cVal = _mm256_mul_ps(aVal, bVal);

      _mm256_store_ps(cPtr,cVal); // Store the results back into the C container

      aPtr += 8;
      cPtr += 8;
    }

    number = eighthPoints * 8;
    for(;number < num_points; number++){
      *cPtr++ = (*aPtr++) * scalar;
    }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
/*!
  \brief Scalar float multiply
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param scalar the scalar value
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_s32f_multiply_32f_u_neon(float* cVector, const float* aVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;
  const float* inputPtr = aVector;
  float* outputPtr = cVector;
  const unsigned int quarterPoints = num_points / 4;

  float32x4_t aVal, cVal;

  for(number = 0; number < quarterPoints; number++){
    aVal = vld1q_f32(inputPtr); // Load into NEON regs
    cVal = vmulq_n_f32 (aVal, scalar); // Do the multiply
    vst1q_f32(outputPtr, cVal); // Store results back to output
    inputPtr += 4;
    outputPtr += 4;
  }
  for(number = quarterPoints * 4; number < num_points; number++){
      *outputPtr++ = (*inputPtr++) * scalar;
  }
}
#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Scalar float multiply
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param scalar the scalar value
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_s32f_multiply_32f_a_generic(float* cVector, const float* aVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;
  const float* inputPtr = aVector;
  float* outputPtr = cVector;
  for(number = 0; number < num_points; number++){
    *outputPtr = (*inputPtr) * scalar;
    inputPtr++;
    outputPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_ORC
/*!
  \brief Scalar float multiply
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param scalar the scalar value
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
extern void volk_32f_s32f_multiply_32f_a_orc_impl(float* dst, const float* src, const float scalar, unsigned int num_points);
static inline void volk_32f_s32f_multiply_32f_u_orc(float* cVector, const float* aVector, const float scalar, unsigned int num_points){
    volk_32f_s32f_multiply_32f_a_orc_impl(cVector, aVector, scalar, num_points);
}
#endif /* LV_HAVE_GENERIC */


#endif /* INCLUDED_volk_32f_s32f_multiply_32f_a_H */

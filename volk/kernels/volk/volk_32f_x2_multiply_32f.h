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

#ifndef INCLUDED_volk_32f_x2_multiply_32f_u_H
#define INCLUDED_volk_32f_x2_multiply_32f_u_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Multiplys the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param bVector One of the vectors to be multiplied
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_x2_multiply_32f_u_sse(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    float* cPtr = cVector;
    const float* aPtr = aVector;
    const float* bPtr=  bVector;

    __m128 aVal, bVal, cVal;
    for(;number < quarterPoints; number++){

      aVal = _mm_loadu_ps(aPtr);
      bVal = _mm_loadu_ps(bPtr);

      cVal = _mm_mul_ps(aVal, bVal);

      _mm_storeu_ps(cPtr,cVal); // Store the results back into the C container

      aPtr += 4;
      bPtr += 4;
      cPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
      *cPtr++ = (*aPtr++) * (*bPtr++);
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_AVX
#include <immintrin.h>
/*!
  \brief Multiplies the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param bVector One of the vectors to be multiplied
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_x2_multiply_32f_u_avx(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int eighthPoints = num_points / 8;

    float* cPtr = cVector;
    const float* aPtr = aVector;
    const float* bPtr=  bVector;

    __m256 aVal, bVal, cVal;
    for(;number < eighthPoints; number++){

      aVal = _mm256_loadu_ps(aPtr);
      bVal = _mm256_loadu_ps(bPtr);

      cVal = _mm256_mul_ps(aVal, bVal);

      _mm256_storeu_ps(cPtr,cVal); // Store the results back into the C container

      aPtr += 8;
      bPtr += 8;
      cPtr += 8;
    }

    number = eighthPoints * 8;
    for(;number < num_points; number++){
      *cPtr++ = (*aPtr++) * (*bPtr++);
    }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Multiplys the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param bVector One of the vectors to be multiplied
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_x2_multiply_32f_generic(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    float* cPtr = cVector;
    const float* aPtr = aVector;
    const float* bPtr=  bVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
      *cPtr++ = (*aPtr++) * (*bPtr++);
    }
}
#endif /* LV_HAVE_GENERIC */


#endif /* INCLUDED_volk_32f_x2_multiply_32f_u_H */
#ifndef INCLUDED_volk_32f_x2_multiply_32f_a_H
#define INCLUDED_volk_32f_x2_multiply_32f_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Multiplys the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param bVector One of the vectors to be multiplied
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_x2_multiply_32f_a_sse(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    float* cPtr = cVector;
    const float* aPtr = aVector;
    const float* bPtr=  bVector;

    __m128 aVal, bVal, cVal;
    for(;number < quarterPoints; number++){

      aVal = _mm_load_ps(aPtr);
      bVal = _mm_load_ps(bPtr);

      cVal = _mm_mul_ps(aVal, bVal);

      _mm_store_ps(cPtr,cVal); // Store the results back into the C container

      aPtr += 4;
      bPtr += 4;
      cPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
      *cPtr++ = (*aPtr++) * (*bPtr++);
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_AVX
#include <immintrin.h>
/*!
  \brief Multiplies the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param bVector One of the vectors to be multiplied
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_x2_multiply_32f_a_avx(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int eighthPoints = num_points / 8;

    float* cPtr = cVector;
    const float* aPtr = aVector;
    const float* bPtr=  bVector;

    __m256 aVal, bVal, cVal;
    for(;number < eighthPoints; number++){

      aVal = _mm256_load_ps(aPtr);
      bVal = _mm256_load_ps(bPtr);

      cVal = _mm256_mul_ps(aVal, bVal);

      _mm256_store_ps(cPtr,cVal); // Store the results back into the C container

      aPtr += 8;
      bPtr += 8;
      cPtr += 8;
    }

    number = eighthPoints * 8;
    for(;number < num_points; number++){
      *cPtr++ = (*aPtr++) * (*bPtr++);
    }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_NEON
#include <arm_neon.h>

/*!
  \brief Multiplys the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param bVector One of the vectors to be multiplied
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_x2_multiply_32f_neon(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    const unsigned int quarter_points = num_points / 4;
    unsigned int number;
    float32x4_t avec, bvec, cvec;
    for(number=0; number < quarter_points; ++number) {
        avec = vld1q_f32(aVector);
        bvec = vld1q_f32(bVector);
        cvec = vmulq_f32(avec, bvec);
        vst1q_f32(cVector, cvec);
        aVector += 4;
        bVector += 4;
        cVector += 4;
    }
    for(number=quarter_points*4; number < num_points; ++number) {
        *cVector++ = *aVector++ * *bVector++;
    }
}
#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Multiplys the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param bVector One of the vectors to be multiplied
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_x2_multiply_32f_a_generic(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    float* cPtr = cVector;
    const float* aPtr = aVector;
    const float* bPtr=  bVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
      *cPtr++ = (*aPtr++) * (*bPtr++);
    }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_ORC
/*!
  \brief Multiplys the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param bVector One of the vectors to be multiplied
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
extern void volk_32f_x2_multiply_32f_a_orc_impl(float* cVector, const float* aVector, const float* bVector, unsigned int num_points);
static inline void volk_32f_x2_multiply_32f_u_orc(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    volk_32f_x2_multiply_32f_a_orc_impl(cVector, aVector, bVector, num_points);
}
#endif /* LV_HAVE_ORC */


#endif /* INCLUDED_volk_32f_x2_multiply_32f_a_H */

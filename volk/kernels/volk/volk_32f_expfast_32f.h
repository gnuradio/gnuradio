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

#include <stdio.h>
#include <math.h>
#include <inttypes.h>

#define Mln2 0.6931471805f
#define A 8388608.0f
#define B 1065353216.0f
#define C 60801.0f


#ifndef INCLUDED_volk_32f_expfast_32f_a_H
#define INCLUDED_volk_32f_expfast_32f_a_H

#ifdef LV_HAVE_AVX
#include <immintrin.h>
/*!
  \brief Computes fast exp (max 7% error) of input vector and stores results in output vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which exp is to be computed
*/
static inline void volk_32f_expfast_32f_a_avx(float* bVector, const float* aVector, unsigned int num_points){

    float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    const unsigned int eighthPoints = num_points / 8;

    __m256 aVal, bVal, a, b;
    __m256i exp;
    a = _mm256_set1_ps(A/Mln2);
    b = _mm256_set1_ps(B-C);

    for(;number < eighthPoints; number++){
        aVal = _mm256_load_ps(aPtr);
        exp = _mm256_cvtps_epi32(_mm256_add_ps(_mm256_mul_ps(a,aVal), b));
        bVal = _mm256_castsi256_ps(exp);

        _mm256_store_ps(bPtr, bVal);
        aPtr += 8;
        bPtr += 8;
    }

    number = eighthPoints * 8;
    for(;number < num_points; number++){
        *bPtr++ = expf(*aPtr++);
    }
}

#endif /* LV_HAVE_AVX for aligned */

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Computes fast exp (max 7% error) of input vector and stores results in output vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which exp is to be computed
*/
static inline void volk_32f_expfast_32f_a_sse4_1(float* bVector, const float* aVector, unsigned int num_points){

    float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    __m128 aVal, bVal, a, b;
    __m128i exp;
    a = _mm_set1_ps(A/Mln2);
    b = _mm_set1_ps(B-C);

    for(;number < quarterPoints; number++){
        aVal = _mm_load_ps(aPtr);
        exp = _mm_cvtps_epi32(_mm_add_ps(_mm_mul_ps(a,aVal), b));
        bVal = _mm_castsi128_ps(exp);

        _mm_store_ps(bPtr, bVal);
        aPtr += 4;
        bPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
        *bPtr++ = expf(*aPtr++);
    }
}

#endif /* LV_HAVE_SSE4_1 for aligned */

#endif /* INCLUDED_volk_32f_expfast_32f_a_H */

#ifndef INCLUDED_volk_32f_expfast_32f_u_H
#define INCLUDED_volk_32f_expfast_32f_u_H

#ifdef LV_HAVE_AVX
#include <immintrin.h>
/*!
  \brief Computes fast exp (max 7% error) of input vector and stores results in output vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which exp is to be computed
*/
static inline void volk_32f_expfast_32f_u_avx(float* bVector, const float* aVector, unsigned int num_points){

    float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    const unsigned int eighthPoints = num_points / 8;

    __m256 aVal, bVal, a, b;
    __m256i exp;
    a = _mm256_set1_ps(A/Mln2);
    b = _mm256_set1_ps(B-C);

    for(;number < eighthPoints; number++){
        aVal = _mm256_loadu_ps(aPtr);
        exp = _mm256_cvtps_epi32(_mm256_add_ps(_mm256_mul_ps(a,aVal), b));
        bVal = _mm256_castsi256_ps(exp);

        _mm256_storeu_ps(bPtr, bVal);
        aPtr += 8;
        bPtr += 8;
    }

    number = eighthPoints * 8;
    for(;number < num_points; number++){
        *bPtr++ = expf(*aPtr++);
    }
}

#endif /* LV_HAVE_AVX for aligned */

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Computes fast exp (max 7% error) of input vector and stores results in output vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which log is to be computed
*/
static inline void volk_32f_expfast_32f_u_sse4_1(float* bVector, const float* aVector, unsigned int num_points){

    float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    __m128 aVal, bVal, a, b;
    __m128i exp;
    a = _mm_set1_ps(A/Mln2);
    b = _mm_set1_ps(B-C);

    for(;number < quarterPoints; number++){
        aVal = _mm_loadu_ps(aPtr);
        exp = _mm_cvtps_epi32(_mm_add_ps(_mm_mul_ps(a,aVal), b));
        bVal = _mm_castsi128_ps(exp);

        _mm_storeu_ps(bPtr, bVal);
        aPtr += 4;
        bPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
        *bPtr++ = expf(*aPtr++);
    }
}

#endif /* LV_HAVE_SSE4_1 for unaligned */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Computes fast exp (max 7% error) of input vector and stores results in output vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which log is to be computed
*/
static inline void volk_32f_expfast_32f_generic(float* bVector, const float* aVector, unsigned int num_points){
    float* bPtr = bVector;
    const float* aPtr = aVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
        *bPtr++ = expf(*aPtr++);
    }

}
#endif /* LV_HAVE_GENERIC */

#endif /* INCLUDED_volk_32f_expfast_32f_u_H */

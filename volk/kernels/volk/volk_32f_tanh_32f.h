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

#ifndef INCLUDED_volk_32f_tanh_32f_a_H
#define INCLUDED_volk_32f_tanh_32f_a_H

#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#ifdef LV_HAVE_GENERIC
/*!
\brief Calculates tanh(x)
\param cVector The vector where the results will be stored
\param aVector Input vector
\param num_points The number of values to calulate
*/
static inline void volk_32f_tanh_32f_generic(float* cVector, const float* aVector,
                                             unsigned int num_points)
{
  unsigned int number = 0;
  float* cPtr = cVector;
  const float* aPtr = aVector;
  for(; number < num_points; number++) {
    *cPtr++ = tanh(*aPtr++);
  }
}

#endif /* LV_HAVE_GENERIC */


#ifdef LV_HAVE_GENERIC
/*!
\brief Calculates tanh(x) using a series approximation, good to within 1e-6 of the actual tanh.
\param cVector The vector where the results will be stored
\param aVector Input vector
\param num_points The number of values to calulate
*/
static inline void volk_32f_tanh_32f_series(float* cVector, const float* aVector,
                                            unsigned int num_points)
{
  unsigned int number = 0;
  float* cPtr = cVector;
  const float* aPtr = aVector;
  for(; number < num_points; number++) {
    if(*aPtr > 4.97)
      *cPtr++ = 1;
    else if(*aPtr <= -4.97)
      *cPtr++ = -1;
    else {
      float x2 = (*aPtr) * (*aPtr);
      float a = (*aPtr) * (135135.0f + x2 * (17325.0f + x2 * (378.0f + x2)));
      float b = 135135.0f + x2 * (62370.0f + x2 * (3150.0f + x2 * 28.0f));
      *cPtr++ = a / b;
      aPtr++;
    }
  }
}

#endif /* LV_HAVE_GENERIC */



#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
\brief Calculates tanh(x) using a series approximation, good to within 1e-6 of the actual tanh.
\param cVector The vector where the results will be stored
\param aVector Input vector
\param num_points The number of values to calulate
*/
static inline void volk_32f_tanh_32f_a_sse(float* cVector, const float* aVector,
                                           unsigned int num_points)
{
  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;

  float* cPtr = cVector;
  const float* aPtr = aVector;

  __m128 aVal, cVal, x2, a, b;
  __m128 const1, const2, const3, const4, const5, const6;
  const1 = _mm_set_ps1(135135.0f);
  const2 = _mm_set_ps1(17325.0f);
  const3 = _mm_set_ps1(378.0f);
  const4 = _mm_set_ps1(62370.0f);
  const5 = _mm_set_ps1(3150.0f);
  const6 = _mm_set_ps1(28.0f);
  for(;number < quarterPoints; number++){

    aVal = _mm_load_ps(aPtr);
    x2 = _mm_mul_ps(aVal, aVal);
    a  = _mm_mul_ps(aVal, _mm_add_ps(const1, _mm_mul_ps(x2, _mm_add_ps(const2, _mm_mul_ps(x2, _mm_add_ps(const3, x2))))));
    b  = _mm_add_ps(const1, _mm_mul_ps(x2, _mm_add_ps(const4, _mm_mul_ps(x2, _mm_add_ps(const5, _mm_mul_ps(x2, const6))))));

    cVal = _mm_div_ps(a, b);

    _mm_store_ps(cPtr, cVal); // Store the results back into the C container

    aPtr += 4;
    cPtr += 4;
  }

  number = quarterPoints * 4;
  for(;number < num_points; number++) {
    if(*aPtr > 4.97)
      *cPtr++ = 1;
    else if(*aPtr <= -4.97)
      *cPtr++ = -1;
    else {
      float x2 = (*aPtr) * (*aPtr);
      float a = (*aPtr) * (135135.0f + x2 * (17325.0f + x2 * (378.0f + x2)));
      float b = 135135.0f + x2 * (62370.0f + x2 * (3150.0f + x2 * 28.0f));
      *cPtr++ = a / b;
      aPtr++;
    }
  }
}
#endif /* LV_HAVE_SSE */


#ifdef LV_HAVE_AVX
#include <immintrin.h>
/*!
\brief Calculates tanh(x) using a series approximation, good to within 1e-6 of the actual tanh.
\param cVector The vector where the results will be stored
\param aVector Input vector
\param num_points The number of values to calulate
*/
static inline void volk_32f_tanh_32f_a_avx(float* cVector, const float* aVector,
                                           unsigned int num_points)
{
  unsigned int number = 0;
  const unsigned int eighthPoints = num_points / 8;

  float* cPtr = cVector;
  const float* aPtr = aVector;

  __m256 aVal, cVal, x2, a, b;
  __m256 const1, const2, const3, const4, const5, const6;
  const1 = _mm256_set1_ps(135135.0f);
  const2 = _mm256_set1_ps(17325.0f);
  const3 = _mm256_set1_ps(378.0f);
  const4 = _mm256_set1_ps(62370.0f);
  const5 = _mm256_set1_ps(3150.0f);
  const6 = _mm256_set1_ps(28.0f);
  for(;number < eighthPoints; number++){

    aVal = _mm256_load_ps(aPtr);
    x2 = _mm256_mul_ps(aVal, aVal);
    a  = _mm256_mul_ps(aVal, _mm256_add_ps(const1, _mm256_mul_ps(x2, _mm256_add_ps(const2, _mm256_mul_ps(x2, _mm256_add_ps(const3, x2))))));
    b  = _mm256_add_ps(const1, _mm256_mul_ps(x2, _mm256_add_ps(const4, _mm256_mul_ps(x2, _mm256_add_ps(const5, _mm256_mul_ps(x2, const6))))));

    cVal = _mm256_div_ps(a, b);

    _mm256_store_ps(cPtr, cVal); // Store the results back into the C container

    aPtr += 8;
    cPtr += 8;
  }

  number = eighthPoints * 8;
  for(;number < num_points; number++) {
    if(*aPtr > 4.97)
      *cPtr++ = 1;
    else if(*aPtr <= -4.97)
      *cPtr++ = -1;
    else {
      float x2 = (*aPtr) * (*aPtr);
      float a = (*aPtr) * (135135.0f + x2 * (17325.0f + x2 * (378.0f + x2)));
      float b = 135135.0f + x2 * (62370.0f + x2 * (3150.0f + x2 * 28.0f));
      *cPtr++ = a / b;
      aPtr++;
    }
  }
}
#endif /* LV_HAVE_AVX */




#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
\brief Calculates tanh(x) using a series approximation, good to within 1e-6 of the actual tanh.
\param cVector The vector where the results will be stored
\param aVector Input vector
\param num_points The number of values to calulate
*/
static inline void volk_32f_tanh_32f_u_sse(float* cVector, const float* aVector,
                                           unsigned int num_points)
{
  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;

  float* cPtr = cVector;
  const float* aPtr = aVector;

  __m128 aVal, cVal, x2, a, b;
  __m128 const1, const2, const3, const4, const5, const6;
  const1 = _mm_set_ps1(135135.0f);
  const2 = _mm_set_ps1(17325.0f);
  const3 = _mm_set_ps1(378.0f);
  const4 = _mm_set_ps1(62370.0f);
  const5 = _mm_set_ps1(3150.0f);
  const6 = _mm_set_ps1(28.0f);
  for(;number < quarterPoints; number++){

    aVal = _mm_loadu_ps(aPtr);
    x2 = _mm_mul_ps(aVal, aVal);
    a  = _mm_mul_ps(aVal, _mm_add_ps(const1, _mm_mul_ps(x2, _mm_add_ps(const2, _mm_mul_ps(x2, _mm_add_ps(const3, x2))))));
    b  = _mm_add_ps(const1, _mm_mul_ps(x2, _mm_add_ps(const4, _mm_mul_ps(x2, _mm_add_ps(const5, _mm_mul_ps(x2, const6))))));

    cVal = _mm_div_ps(a, b);

    _mm_storeu_ps(cPtr, cVal); // Store the results back into the C container

    aPtr += 4;
    cPtr += 4;
  }

  number = quarterPoints * 4;
  for(;number < num_points; number++) {
    if(*aPtr > 4.97)
      *cPtr++ = 1;
    else if(*aPtr <= -4.97)
      *cPtr++ = -1;
    else {
      float x2 = (*aPtr) * (*aPtr);
      float a = (*aPtr) * (135135.0f + x2 * (17325.0f + x2 * (378.0f + x2)));
      float b = 135135.0f + x2 * (62370.0f + x2 * (3150.0f + x2 * 28.0f));
      *cPtr++ = a / b;
      aPtr++;
    }
  }
}
#endif /* LV_HAVE_SSE */



#ifdef LV_HAVE_AVX
#include <immintrin.h>
/*!
\brief Calculates tanh(x) using a series approximation, good to within 1e-6 of the actual tanh.
\param cVector The vector where the results will be stored
\param aVector Input vector
\param num_points The number of values to calulate
*/
static inline void volk_32f_tanh_32f_u_avx(float* cVector, const float* aVector,
                                           unsigned int num_points)
{
  unsigned int number = 0;
  const unsigned int eighthPoints = num_points / 8;

  float* cPtr = cVector;
  const float* aPtr = aVector;

  __m256 aVal, cVal, x2, a, b;
  __m256 const1, const2, const3, const4, const5, const6;
  const1 = _mm256_set1_ps(135135.0f);
  const2 = _mm256_set1_ps(17325.0f);
  const3 = _mm256_set1_ps(378.0f);
  const4 = _mm256_set1_ps(62370.0f);
  const5 = _mm256_set1_ps(3150.0f);
  const6 = _mm256_set1_ps(28.0f);
  for(;number < eighthPoints; number++){

    aVal = _mm256_loadu_ps(aPtr);
    x2 = _mm256_mul_ps(aVal, aVal);
    a  = _mm256_mul_ps(aVal, _mm256_add_ps(const1, _mm256_mul_ps(x2, _mm256_add_ps(const2, _mm256_mul_ps(x2, _mm256_add_ps(const3, x2))))));
    b  = _mm256_add_ps(const1, _mm256_mul_ps(x2, _mm256_add_ps(const4, _mm256_mul_ps(x2, _mm256_add_ps(const5, _mm256_mul_ps(x2, const6))))));

    cVal = _mm256_div_ps(a, b);

    _mm256_storeu_ps(cPtr, cVal); // Store the results back into the C container

    aPtr += 8;
    cPtr += 8;
  }

  number = eighthPoints * 8;
  for(;number < num_points; number++) {
    if(*aPtr > 4.97)
      *cPtr++ = 1;
    else if(*aPtr <= -4.97)
      *cPtr++ = -1;
    else {
      float x2 = (*aPtr) * (*aPtr);
      float a = (*aPtr) * (135135.0f + x2 * (17325.0f + x2 * (378.0f + x2)));
      float b = 135135.0f + x2 * (62370.0f + x2 * (3150.0f + x2 * 28.0f));
      *cPtr++ = a / b;
      aPtr++;
    }
  }
}
#endif /* LV_HAVE_AVX */

#endif /* INCLUDED_volk_32f_tanh_32f_a_H */

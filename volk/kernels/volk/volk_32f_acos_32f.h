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

/* This is the number of terms of Taylor series to evaluate, increase this for more accuracy*/
#define ACOS_TERMS 2

#ifndef INCLUDED_volk_32f_acos_32f_a_H
#define INCLUDED_volk_32f_acos_32f_a_H

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Computes arccosine of input vector and stores results in output vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which arccosine is to be computed
*/
static inline void volk_32f_acos_32f_a_sse4_1(float* bVector, const float* aVector, unsigned int num_points){

  float* bPtr = bVector;
  const float* aPtr = aVector;

  unsigned int number = 0;
  unsigned int quarterPoints = num_points / 4;
  int i, j;

  __m128 aVal, d, pi, pio2, x, y, z, arccosine;
  __m128 fzeroes, fones, ftwos, ffours, condition;

  pi = _mm_set1_ps(3.14159265358979323846);
  pio2 = _mm_set1_ps(3.14159265358979323846/2);
  fzeroes = _mm_setzero_ps();
  fones = _mm_set1_ps(1.0);
  ftwos = _mm_set1_ps(2.0);
  ffours = _mm_set1_ps(4.0);

  for(;number < quarterPoints; number++){
    aVal = _mm_load_ps(aPtr);
    d = aVal;
    aVal = _mm_div_ps(_mm_sqrt_ps(_mm_mul_ps(_mm_add_ps(fones, aVal), _mm_sub_ps(fones, aVal))), aVal);
    z = aVal;
    condition = _mm_cmplt_ps(z, fzeroes);
    z = _mm_sub_ps(z, _mm_and_ps(_mm_mul_ps(z, ftwos), condition));
    x = z;
    condition = _mm_cmplt_ps(z, fones);
    x = _mm_add_ps(x, _mm_and_ps(_mm_sub_ps(_mm_div_ps(fones, z), z), condition));

    for(i = 0; i < 2; i++)
      x = _mm_add_ps(x, _mm_sqrt_ps(_mm_add_ps(fones, _mm_mul_ps(x, x))));
    x = _mm_div_ps(fones, x);
    y = fzeroes;
    for(j = ACOS_TERMS - 1; j >=0 ; j--)
      y = _mm_add_ps(_mm_mul_ps(y, _mm_mul_ps(x, x)), _mm_set1_ps(pow(-1,j)/(2*j+1)));

    y = _mm_mul_ps(y, _mm_mul_ps(x, ffours));
    condition = _mm_cmpgt_ps(z, fones);

    y = _mm_add_ps(y, _mm_and_ps(_mm_sub_ps(pio2, _mm_mul_ps(y, ftwos)), condition));
    arccosine = y;
    condition = _mm_cmplt_ps(aVal, fzeroes);
    arccosine = _mm_sub_ps(arccosine, _mm_and_ps(_mm_mul_ps(arccosine, ftwos), condition));
    condition = _mm_cmplt_ps(d, fzeroes);
    arccosine = _mm_add_ps(arccosine, _mm_and_ps(pi, condition));

    _mm_store_ps(bPtr, arccosine);
    aPtr += 4;
    bPtr += 4;
  }

  number = quarterPoints * 4;
  for(;number < num_points; number++){
    *bPtr++ = acos(*aPtr++);
  }
}

#endif /* LV_HAVE_SSE4_1 for aligned */

#endif /* INCLUDED_volk_32f_acos_32f_a_H */


#ifndef INCLUDED_volk_32f_acos_32f_u_H
#define INCLUDED_volk_32f_acos_32f_u_H

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Computes arccosine of input vector and stores results in output vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which arccosine is to be computed
*/
static inline void volk_32f_acos_32f_u_sse4_1(float* bVector, const float* aVector, unsigned int num_points){
  float* bPtr = bVector;
  const float* aPtr = aVector;

  unsigned int number = 0;
  unsigned int quarterPoints = num_points / 4;
  int i, j;

  __m128 aVal, d, pi, pio2, x, y, z, arccosine;
  __m128 fzeroes, fones, ftwos, ffours, condition;

  pi = _mm_set1_ps(3.14159265358979323846);
  pio2 = _mm_set1_ps(3.14159265358979323846/2);
  fzeroes = _mm_setzero_ps();
  fones = _mm_set1_ps(1.0);
  ftwos = _mm_set1_ps(2.0);
  ffours = _mm_set1_ps(4.0);

  for(;number < quarterPoints; number++){
    aVal = _mm_loadu_ps(aPtr);
    d = aVal;
    aVal = _mm_div_ps(_mm_sqrt_ps(_mm_mul_ps(_mm_add_ps(fones, aVal), _mm_sub_ps(fones, aVal))), aVal);
    z = aVal;
    condition = _mm_cmplt_ps(z, fzeroes);
    z = _mm_sub_ps(z, _mm_and_ps(_mm_mul_ps(z, ftwos), condition));
    x = z;
    condition = _mm_cmplt_ps(z, fones);
    x = _mm_add_ps(x, _mm_and_ps(_mm_sub_ps(_mm_div_ps(fones, z), z), condition));

    for(i = 0; i < 2; i++)
      x = _mm_add_ps(x, _mm_sqrt_ps(_mm_add_ps(fones, _mm_mul_ps(x, x))));
    x = _mm_div_ps(fones, x);
    y = fzeroes;

    for(j = ACOS_TERMS - 1; j >=0 ; j--)
      y = _mm_add_ps(_mm_mul_ps(y, _mm_mul_ps(x, x)), _mm_set1_ps(pow(-1,j)/(2*j+1)));

    y = _mm_mul_ps(y, _mm_mul_ps(x, ffours));
    condition = _mm_cmpgt_ps(z, fones);

    y = _mm_add_ps(y, _mm_and_ps(_mm_sub_ps(pio2, _mm_mul_ps(y, ftwos)), condition));
    arccosine = y;
    condition = _mm_cmplt_ps(aVal, fzeroes);
    arccosine = _mm_sub_ps(arccosine, _mm_and_ps(_mm_mul_ps(arccosine, ftwos), condition));
    condition = _mm_cmplt_ps(d, fzeroes);
    arccosine = _mm_add_ps(arccosine, _mm_and_ps(pi, condition));

    _mm_storeu_ps(bPtr, arccosine);
    aPtr += 4;
    bPtr += 4;
  }

  number = quarterPoints * 4;
  for(;number < num_points; number++){
    *bPtr++ = acos(*aPtr++);
  }
}

#endif /* LV_HAVE_SSE4_1 for aligned */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Computes arccosine of input vector and stores results in output vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which arccosine is to be computed
*/
static inline void volk_32f_acos_32f_generic(float* bVector, const float* aVector, unsigned int num_points){
  float* bPtr = bVector;
  const float* aPtr = aVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *bPtr++ = acos(*aPtr++);
  }

}
#endif /* LV_HAVE_GENERIC */

#endif /* INCLUDED_volk_32f_acos_32f_u_H */

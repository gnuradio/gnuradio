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

#ifndef INCLUDED_volk_32f_tan_32f_a_H
#define INCLUDED_volk_32f_tan_32f_a_H

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Computes tangent of input vector and stores results in output vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which tangent is to be computed
*/
static inline void volk_32f_tan_32f_a_sse4_1(float* bVector, const float* aVector, unsigned int num_points){

    float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    unsigned int quarterPoints = num_points / 4;
    unsigned int i = 0;

    __m128 aVal, s, m4pi, pio4A, pio4B, cp1, cp2, cp3, cp4, cp5, ffours, ftwos, fones, fzeroes;
    __m128 sine, cosine, tangent, condition1, condition2, condition3;
    __m128i q, r, ones, twos, fours;

    m4pi = _mm_set1_ps(1.273239545);
    pio4A = _mm_set1_ps(0.78515625);
    pio4B = _mm_set1_ps(0.241876e-3);
    ffours = _mm_set1_ps(4.0);
    ftwos = _mm_set1_ps(2.0);
    fones = _mm_set1_ps(1.0);
    fzeroes = _mm_setzero_ps();
    ones = _mm_set1_epi32(1);
    twos = _mm_set1_epi32(2);
    fours = _mm_set1_epi32(4);

    cp1 = _mm_set1_ps(1.0);
    cp2 = _mm_set1_ps(0.83333333e-1);
    cp3 = _mm_set1_ps(0.2777778e-2);
    cp4 = _mm_set1_ps(0.49603e-4);
    cp5 = _mm_set1_ps(0.551e-6);

    for(;number < quarterPoints; number++){
        aVal = _mm_load_ps(aPtr);
        s = _mm_sub_ps(aVal, _mm_and_ps(_mm_mul_ps(aVal, ftwos), _mm_cmplt_ps(aVal, fzeroes)));
        q = _mm_cvtps_epi32(_mm_mul_ps(s, m4pi));
        r = _mm_add_epi32(q, _mm_and_si128(q, ones));

        s = _mm_sub_ps(s, _mm_mul_ps(_mm_cvtepi32_ps(r), pio4A));
        s = _mm_sub_ps(s, _mm_mul_ps(_mm_cvtepi32_ps(r), pio4B));

        s = _mm_div_ps(s, _mm_set1_ps(8.0));    // The constant is 2^N, for 3 times argument reduction
        s = _mm_mul_ps(s, s);
        // Evaluate Taylor series
        s = _mm_mul_ps(_mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_mul_ps(_mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_mul_ps(s, cp5), cp4), s), cp3), s), cp2), s), cp1), s);

        for(i = 0; i < 3; i++){
            s = _mm_mul_ps(s, _mm_sub_ps(ffours, s));
        }
        s = _mm_div_ps(s, ftwos);

        sine = _mm_sqrt_ps(_mm_mul_ps(_mm_sub_ps(ftwos, s), s));
        cosine = _mm_sub_ps(fones, s);

        condition1 = _mm_cmpneq_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_add_epi32(q, ones), twos)), fzeroes);
        condition2 = _mm_cmpneq_ps(_mm_cmpneq_ps(_mm_cvtepi32_ps(_mm_and_si128(q, fours)), fzeroes), _mm_cmplt_ps(aVal, fzeroes));
        condition3 = _mm_cmpneq_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_add_epi32(q, twos), fours)), fzeroes);

        __m128 temp = cosine;
        cosine = _mm_add_ps(cosine, _mm_and_ps(_mm_sub_ps(sine, cosine), condition1));
        sine = _mm_add_ps(sine, _mm_and_ps(_mm_sub_ps(temp, sine), condition1));
        sine = _mm_sub_ps(sine, _mm_and_ps(_mm_mul_ps(sine, _mm_set1_ps(2.0f)), condition2));
        cosine = _mm_sub_ps(cosine, _mm_and_ps(_mm_mul_ps(cosine, _mm_set1_ps(2.0f)), condition3));
        tangent = _mm_div_ps(sine, cosine);
        _mm_store_ps(bPtr, tangent);
        aPtr += 4;
        bPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
       *bPtr++ = tan(*aPtr++);
    }
}

#endif /* LV_HAVE_SSE4_1 for aligned */

#endif /* INCLUDED_volk_32f_tan_32f_a_H */

#ifndef INCLUDED_volk_32f_tan_32f_u_H
#define INCLUDED_volk_32f_tan_32f_u_H

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Computes tangent of input vector and stores results in output vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which tangent is to be computed
*/
static inline void volk_32f_tan_32f_u_sse4_1(float* bVector, const float* aVector, unsigned int num_points){

    float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    unsigned int quarterPoints = num_points / 4;
    unsigned int i = 0;

    __m128 aVal, s, m4pi, pio4A, pio4B, cp1, cp2, cp3, cp4, cp5, ffours, ftwos, fones, fzeroes;
    __m128 sine, cosine, tangent, condition1, condition2, condition3;
    __m128i q, r, ones, twos, fours;

    m4pi = _mm_set1_ps(1.273239545);
    pio4A = _mm_set1_ps(0.78515625);
    pio4B = _mm_set1_ps(0.241876e-3);
    ffours = _mm_set1_ps(4.0);
    ftwos = _mm_set1_ps(2.0);
    fones = _mm_set1_ps(1.0);
    fzeroes = _mm_setzero_ps();
    ones = _mm_set1_epi32(1);
    twos = _mm_set1_epi32(2);
    fours = _mm_set1_epi32(4);

    cp1 = _mm_set1_ps(1.0);
    cp2 = _mm_set1_ps(0.83333333e-1);
    cp3 = _mm_set1_ps(0.2777778e-2);
    cp4 = _mm_set1_ps(0.49603e-4);
    cp5 = _mm_set1_ps(0.551e-6);

    for(;number < quarterPoints; number++){
        aVal = _mm_loadu_ps(aPtr);
        s = _mm_sub_ps(aVal, _mm_and_ps(_mm_mul_ps(aVal, ftwos), _mm_cmplt_ps(aVal, fzeroes)));
        q = _mm_cvtps_epi32(_mm_mul_ps(s, m4pi));
        r = _mm_add_epi32(q, _mm_and_si128(q, ones));

        s = _mm_sub_ps(s, _mm_mul_ps(_mm_cvtepi32_ps(r), pio4A));
        s = _mm_sub_ps(s, _mm_mul_ps(_mm_cvtepi32_ps(r), pio4B));

        s = _mm_div_ps(s, _mm_set1_ps(8.0));    // The constant is 2^N, for 3 times argument reduction
        s = _mm_mul_ps(s, s);
        // Evaluate Taylor series
        s = _mm_mul_ps(_mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_mul_ps(_mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_mul_ps(s, cp5), cp4), s), cp3), s), cp2), s), cp1), s);

        for(i = 0; i < 3; i++){
            s = _mm_mul_ps(s, _mm_sub_ps(ffours, s));
        }
        s = _mm_div_ps(s, ftwos);

        sine = _mm_sqrt_ps(_mm_mul_ps(_mm_sub_ps(ftwos, s), s));
        cosine = _mm_sub_ps(fones, s);

        condition1 = _mm_cmpneq_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_add_epi32(q, ones), twos)), fzeroes);
        condition2 = _mm_cmpneq_ps(_mm_cmpneq_ps(_mm_cvtepi32_ps(_mm_and_si128(q, fours)), fzeroes), _mm_cmplt_ps(aVal, fzeroes));
        condition3 = _mm_cmpneq_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_add_epi32(q, twos), fours)), fzeroes);

        __m128 temp = cosine;
        cosine = _mm_add_ps(cosine, _mm_and_ps(_mm_sub_ps(sine, cosine), condition1));
        sine = _mm_add_ps(sine, _mm_and_ps(_mm_sub_ps(temp, sine), condition1));
        sine = _mm_sub_ps(sine, _mm_and_ps(_mm_mul_ps(sine, _mm_set1_ps(2.0f)), condition2));
        cosine = _mm_sub_ps(cosine, _mm_and_ps(_mm_mul_ps(cosine, _mm_set1_ps(2.0f)), condition3));
        tangent = _mm_div_ps(sine, cosine);
        _mm_storeu_ps(bPtr, tangent);
        aPtr += 4;
        bPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
        *bPtr++ = tan(*aPtr++);
    }
}

#endif /* LV_HAVE_SSE4_1 for unaligned */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Computes tangent of input vector and stores results in output vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which tangent is to be computed
*/
static inline void volk_32f_tan_32f_generic(float* bVector, const float* aVector, unsigned int num_points){
    float* bPtr = bVector;
    const float* aPtr = aVector;
    unsigned int number = 0;

    for(; number < num_points; number++){
        *bPtr++ = tan(*aPtr++);
    }

}
#endif /* LV_HAVE_GENERIC */

#endif /* INCLUDED_volk_32f_tan_32f_u_H */

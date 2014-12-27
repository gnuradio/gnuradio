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
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#define POLY0(x, c0) _mm_set1_ps(c0)
#define POLY1(x, c0, c1) _mm_add_ps(_mm_mul_ps(POLY0(x, c1), x), _mm_set1_ps(c0))
#define POLY2(x, c0, c1, c2) _mm_add_ps(_mm_mul_ps(POLY1(x, c1, c2), x), _mm_set1_ps(c0))
#define POLY3(x, c0, c1, c2, c3) _mm_add_ps(_mm_mul_ps(POLY2(x, c1, c2, c3), x), _mm_set1_ps(c0))
#define POLY4(x, c0, c1, c2, c3, c4) _mm_add_ps(_mm_mul_ps(POLY3(x, c1, c2, c3, c4), x), _mm_set1_ps(c0))
#define POLY5(x, c0, c1, c2, c3, c4, c5) _mm_add_ps(_mm_mul_ps(POLY4(x, c1, c2, c3, c4, c5), x), _mm_set1_ps(c0))

#define LOG_POLY_DEGREE 3

#ifndef INCLUDED_volk_32f_x2_pow_32f_a_H
#define INCLUDED_volk_32f_x2_pow_32f_a_H

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Computes pow(x,y) by using exp and log
  \param cVector The vector where results will be stored
  \param aVector The input vector of bases
  \param bVector The input vector of indices
  \param num_points Number of points for which pow is to be computed
*/
static inline void volk_32f_x2_pow_32f_a_sse4_1(float* cVector, const float* bVector, const float* aVector, unsigned int num_points){

    float* cPtr = cVector;
    const float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    __m128 aVal, bVal, cVal, logarithm, mantissa, frac, leadingOne;
    __m128 tmp, fx, mask, pow2n, z, y;
    __m128 one, exp_hi, exp_lo, ln2, log2EF, half, exp_C1, exp_C2;
    __m128 exp_p0, exp_p1, exp_p2, exp_p3, exp_p4, exp_p5;
    __m128i bias, exp, emm0, pi32_0x7f;

    one = _mm_set1_ps(1.0);
    exp_hi = _mm_set1_ps(88.3762626647949);
    exp_lo = _mm_set1_ps(-88.3762626647949);
    ln2 = _mm_set1_ps(0.6931471805);
    log2EF = _mm_set1_ps(1.44269504088896341);
    half = _mm_set1_ps(0.5);
    exp_C1 = _mm_set1_ps(0.693359375);
    exp_C2 = _mm_set1_ps(-2.12194440e-4);
    pi32_0x7f = _mm_set1_epi32(0x7f);

    exp_p0 = _mm_set1_ps(1.9875691500e-4);
    exp_p1 = _mm_set1_ps(1.3981999507e-3);
    exp_p2 = _mm_set1_ps(8.3334519073e-3);
    exp_p3 = _mm_set1_ps(4.1665795894e-2);
    exp_p4 = _mm_set1_ps(1.6666665459e-1);
    exp_p5 = _mm_set1_ps(5.0000001201e-1);

    for(;number < quarterPoints; number++){
        // First compute the logarithm
        aVal = _mm_load_ps(aPtr);
        bias = _mm_set1_epi32(127);
        leadingOne = _mm_set1_ps(1.0f);
        exp = _mm_sub_epi32(_mm_srli_epi32(_mm_and_si128(_mm_castps_si128(aVal), _mm_set1_epi32(0x7f800000)), 23), bias);
        logarithm = _mm_cvtepi32_ps(exp);

        frac = _mm_or_ps(leadingOne, _mm_and_ps(aVal, _mm_castsi128_ps(_mm_set1_epi32(0x7fffff))));

        #if LOG_POLY_DEGREE == 6
          mantissa = POLY5( frac, 3.1157899f, -3.3241990f, 2.5988452f, -1.2315303f,  3.1821337e-1f, -3.4436006e-2f);
        #elif LOG_POLY_DEGREE == 5
          mantissa = POLY4( frac, 2.8882704548164776201f, -2.52074962577807006663f, 1.48116647521213171641f, -0.465725644288844778798f, 0.0596515482674574969533f);
        #elif LOG_POLY_DEGREE == 4
          mantissa = POLY3( frac, 2.61761038894603480148f, -1.75647175389045657003f, 0.688243882994381274313f, -0.107254423828329604454f);
        #elif LOG_POLY_DEGREE == 3
          mantissa = POLY2( frac, 2.28330284476918490682f, -1.04913055217340124191f, 0.204446009836232697516f);
        #else
        #error
        #endif

        logarithm = _mm_add_ps(logarithm, _mm_mul_ps(mantissa, _mm_sub_ps(frac, leadingOne)));
        logarithm = _mm_mul_ps(logarithm, ln2);


        // Now calculate b*lna
        bVal = _mm_load_ps(bPtr);
        bVal = _mm_mul_ps(bVal, logarithm);

        // Now compute exp(b*lna)
        tmp = _mm_setzero_ps();

        bVal = _mm_max_ps(_mm_min_ps(bVal, exp_hi), exp_lo);

        fx = _mm_add_ps(_mm_mul_ps(bVal, log2EF), half);

        emm0 = _mm_cvttps_epi32(fx);
        tmp = _mm_cvtepi32_ps(emm0);

        mask = _mm_and_ps(_mm_cmpgt_ps(tmp, fx), one);
        fx = _mm_sub_ps(tmp, mask);

        tmp = _mm_mul_ps(fx, exp_C1);
        z = _mm_mul_ps(fx, exp_C2);
        bVal = _mm_sub_ps(_mm_sub_ps(bVal, tmp), z);
        z = _mm_mul_ps(bVal, bVal);

        y = _mm_mul_ps(_mm_add_ps(_mm_mul_ps(exp_p0, bVal), exp_p1), bVal);
        y = _mm_add_ps(_mm_mul_ps(_mm_add_ps(y, exp_p2), bVal), exp_p3);
        y = _mm_mul_ps(_mm_add_ps(_mm_mul_ps(y, bVal), exp_p4), bVal);
        y = _mm_add_ps(_mm_mul_ps(_mm_add_ps(y, exp_p5), z), bVal);
        y = _mm_add_ps(y, one);

        emm0 = _mm_slli_epi32(_mm_add_epi32(_mm_cvttps_epi32(fx), pi32_0x7f), 23);

        pow2n = _mm_castsi128_ps(emm0);
        cVal = _mm_mul_ps(y, pow2n);

        _mm_store_ps(cPtr, cVal);

        aPtr += 4;
        bPtr += 4;
        cPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
        *cPtr++ = pow(*aPtr++, *bPtr++);
    }
}

#endif /* LV_HAVE_SSE4_1 for aligned */

#endif /* INCLUDED_volk_32f_x2_pow_32f_a_H */

#ifndef INCLUDED_volk_32f_x2_pow_32f_u_H
#define INCLUDED_volk_32f_x2_pow_32f_u_H

#ifdef LV_HAVE_GENERIC
/*!
  \brief Computes pow(x,y) by using exp and log
  \param cVector The vector where results will be stored
  \param aVector The input vector of bases
  \param bVector The input vector of indices
  \param num_points Number of points for which pow is to be computed
*/
static inline void volk_32f_x2_pow_32f_generic(float* cVector, const float* bVector, const float* aVector, unsigned int num_points){
    float* cPtr = cVector;
    const float* bPtr = bVector;
    const float* aPtr = aVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
        *cPtr++ = pow(*aPtr++, *bPtr++);
    }

}
#endif /* LV_HAVE_GENERIC */


#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Computes pow(x,y) by using exp and log
  \param cVector The vector where results will be stored
  \param aVector The input vector of bases
  \param bVector The input vector of indices
  \param num_points Number of points for which pow is to be computed
*/
static inline void volk_32f_x2_pow_32f_u_sse4_1(float* cVector, const float* bVector, const float* aVector, unsigned int num_points){

    float* cPtr = cVector;
    const float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    __m128 aVal, bVal, cVal, logarithm, mantissa, frac, leadingOne;
    __m128 tmp, fx, mask, pow2n, z, y;
    __m128 one, exp_hi, exp_lo, ln2, log2EF, half, exp_C1, exp_C2;
    __m128 exp_p0, exp_p1, exp_p2, exp_p3, exp_p4, exp_p5;
    __m128i bias, exp, emm0, pi32_0x7f;

    one = _mm_set1_ps(1.0);
    exp_hi = _mm_set1_ps(88.3762626647949);
    exp_lo = _mm_set1_ps(-88.3762626647949);
    ln2 = _mm_set1_ps(0.6931471805);
    log2EF = _mm_set1_ps(1.44269504088896341);
    half = _mm_set1_ps(0.5);
    exp_C1 = _mm_set1_ps(0.693359375);
    exp_C2 = _mm_set1_ps(-2.12194440e-4);
    pi32_0x7f = _mm_set1_epi32(0x7f);

    exp_p0 = _mm_set1_ps(1.9875691500e-4);
    exp_p1 = _mm_set1_ps(1.3981999507e-3);
    exp_p2 = _mm_set1_ps(8.3334519073e-3);
    exp_p3 = _mm_set1_ps(4.1665795894e-2);
    exp_p4 = _mm_set1_ps(1.6666665459e-1);
    exp_p5 = _mm_set1_ps(5.0000001201e-1);

    for(;number < quarterPoints; number++){

    // First compute the logarithm
    aVal = _mm_loadu_ps(aPtr);
    bias = _mm_set1_epi32(127);
    leadingOne = _mm_set1_ps(1.0f);
    exp = _mm_sub_epi32(_mm_srli_epi32(_mm_and_si128(_mm_castps_si128(aVal), _mm_set1_epi32(0x7f800000)), 23), bias);
    logarithm = _mm_cvtepi32_ps(exp);

    frac = _mm_or_ps(leadingOne, _mm_and_ps(aVal, _mm_castsi128_ps(_mm_set1_epi32(0x7fffff))));

    #if LOG_POLY_DEGREE == 6
      mantissa = POLY5( frac, 3.1157899f, -3.3241990f, 2.5988452f, -1.2315303f,  3.1821337e-1f, -3.4436006e-2f);
    #elif LOG_POLY_DEGREE == 5
      mantissa = POLY4( frac, 2.8882704548164776201f, -2.52074962577807006663f, 1.48116647521213171641f, -0.465725644288844778798f, 0.0596515482674574969533f);
    #elif LOG_POLY_DEGREE == 4
      mantissa = POLY3( frac, 2.61761038894603480148f, -1.75647175389045657003f, 0.688243882994381274313f, -0.107254423828329604454f);
    #elif LOG_POLY_DEGREE == 3
      mantissa = POLY2( frac, 2.28330284476918490682f, -1.04913055217340124191f, 0.204446009836232697516f);
    #else
    #error
    #endif

    logarithm = _mm_add_ps(logarithm, _mm_mul_ps(mantissa, _mm_sub_ps(frac, leadingOne)));
    logarithm = _mm_mul_ps(logarithm, ln2);


    // Now calculate b*lna
    bVal = _mm_loadu_ps(bPtr);
    bVal = _mm_mul_ps(bVal, logarithm);

    // Now compute exp(b*lna)
    tmp = _mm_setzero_ps();

    bVal = _mm_max_ps(_mm_min_ps(bVal, exp_hi), exp_lo);

    fx = _mm_add_ps(_mm_mul_ps(bVal, log2EF), half);

    emm0 = _mm_cvttps_epi32(fx);
    tmp = _mm_cvtepi32_ps(emm0);

    mask = _mm_and_ps(_mm_cmpgt_ps(tmp, fx), one);
    fx = _mm_sub_ps(tmp, mask);

    tmp = _mm_mul_ps(fx, exp_C1);
    z = _mm_mul_ps(fx, exp_C2);
    bVal = _mm_sub_ps(_mm_sub_ps(bVal, tmp), z);
    z = _mm_mul_ps(bVal, bVal);

    y = _mm_mul_ps(_mm_add_ps(_mm_mul_ps(exp_p0, bVal), exp_p1), bVal);
    y = _mm_add_ps(_mm_mul_ps(_mm_add_ps(y, exp_p2), bVal), exp_p3);
    y = _mm_mul_ps(_mm_add_ps(_mm_mul_ps(y, bVal), exp_p4), bVal);
    y = _mm_add_ps(_mm_mul_ps(_mm_add_ps(y, exp_p5), z), bVal);
    y = _mm_add_ps(y, one);

    emm0 = _mm_slli_epi32(_mm_add_epi32(_mm_cvttps_epi32(fx), pi32_0x7f), 23);

    pow2n = _mm_castsi128_ps(emm0);
    cVal = _mm_mul_ps(y, pow2n);

    _mm_storeu_ps(cPtr, cVal);

    aPtr += 4;
    bPtr += 4;
    cPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
        *cPtr++ = pow(*aPtr++, *bPtr++);
    }
}

#endif /* LV_HAVE_SSE4_1 for unaligned */

#endif /* INCLUDED_volk_32f_x2_log2_32f_u_H */

/* SIMD (SSE4) implementation of exp
   Inspired by Intel Approximate Math library, and based on the
   corresponding algorithms of the cephes math library
*/

/* Copyright (C) 2007  Julien Pommier

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  (this is the zlib license)
*/


#include <stdio.h>
#include <math.h>
#include <inttypes.h>

#ifndef INCLUDED_volk_32f_exp_32f_a_H
#define INCLUDED_volk_32f_exp_32f_a_H

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Computes exp (based on cephes math lib) of input vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which exp is to be computed
*/
static inline void volk_32f_exp_32f_a_sse4_1(float* bVector, const float* aVector, unsigned int num_points){

    float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    unsigned int quarterPoints = num_points / 4;

    // Declare variables and constants
    __m128 aVal, bVal, tmp, fx, mask, pow2n, z, y;
    __m128 one, exp_hi, exp_lo, log2EF, half, exp_C1, exp_C2;
    __m128 exp_p0, exp_p1, exp_p2, exp_p3, exp_p4, exp_p5;
    __m128i emm0, pi32_0x7f;

    one = _mm_set1_ps(1.0);
    exp_hi = _mm_set1_ps(88.3762626647949);
    exp_lo = _mm_set1_ps(-88.3762626647949);
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

        aVal = _mm_load_ps(aPtr);
        tmp = _mm_setzero_ps();

        aVal = _mm_max_ps(_mm_min_ps(aVal, exp_hi), exp_lo);

        /* express exp(x) as exp(g + n*log(2)) */
        fx = _mm_add_ps(_mm_mul_ps(aVal, log2EF), half);

        emm0 = _mm_cvttps_epi32(fx);
        tmp = _mm_cvtepi32_ps(emm0);

        mask = _mm_and_ps(_mm_cmpgt_ps(tmp, fx), one);
        fx = _mm_sub_ps(tmp, mask);

        tmp = _mm_mul_ps(fx, exp_C1);
        z = _mm_mul_ps(fx, exp_C2);
        aVal = _mm_sub_ps(_mm_sub_ps(aVal, tmp), z);
        z = _mm_mul_ps(aVal, aVal);

        y = _mm_mul_ps(_mm_add_ps(_mm_mul_ps(exp_p0, aVal), exp_p1), aVal);
        y = _mm_add_ps(_mm_mul_ps(_mm_add_ps(y, exp_p2), aVal), exp_p3);
        y = _mm_mul_ps(_mm_add_ps(_mm_mul_ps(y, aVal), exp_p4), aVal);
        y = _mm_add_ps(_mm_mul_ps(_mm_add_ps(y, exp_p5), z), aVal);
        y = _mm_add_ps(y, one);

        emm0 = _mm_slli_epi32(_mm_add_epi32(_mm_cvttps_epi32(fx), pi32_0x7f), 23);

        pow2n = _mm_castsi128_ps(emm0);
        bVal = _mm_mul_ps(y, pow2n);

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

#endif /* INCLUDED_volk_32f_exp_32f_a_H */

#ifndef INCLUDED_volk_32f_exp_32f_u_H
#define INCLUDED_volk_32f_exp_32f_u_H

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Computes exp (based on cephes math lib) of input vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which exp is to be computed
*/
static inline void volk_32f_exp_32f_u_sse4_1(float* bVector, const float* aVector, unsigned int num_points){

    float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    unsigned int quarterPoints = num_points / 4;

    // Declare variables and constants
    __m128 aVal, bVal, tmp, fx, mask, pow2n, z, y;
    __m128 one, exp_hi, exp_lo, log2EF, half, exp_C1, exp_C2;
    __m128 exp_p0, exp_p1, exp_p2, exp_p3, exp_p4, exp_p5;
    __m128i emm0, pi32_0x7f;

    one = _mm_set1_ps(1.0);
    exp_hi = _mm_set1_ps(88.3762626647949);
    exp_lo = _mm_set1_ps(-88.3762626647949);
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

        aVal = _mm_loadu_ps(aPtr);
        tmp = _mm_setzero_ps();

        aVal = _mm_max_ps(_mm_min_ps(aVal, exp_hi), exp_lo);

        /* express exp(x) as exp(g + n*log(2)) */
        fx = _mm_add_ps(_mm_mul_ps(aVal, log2EF), half);

        emm0 = _mm_cvttps_epi32(fx);
        tmp = _mm_cvtepi32_ps(emm0);

        mask = _mm_and_ps(_mm_cmpgt_ps(tmp, fx), one);
        fx = _mm_sub_ps(tmp, mask);

        tmp = _mm_mul_ps(fx, exp_C1);
        z = _mm_mul_ps(fx, exp_C2);
        aVal = _mm_sub_ps(_mm_sub_ps(aVal, tmp), z);
        z = _mm_mul_ps(aVal, aVal);

        y = _mm_mul_ps(_mm_add_ps(_mm_mul_ps(exp_p0, aVal), exp_p1), aVal);
        y = _mm_add_ps(_mm_mul_ps(_mm_add_ps(y, exp_p2), aVal), exp_p3);
        y = _mm_mul_ps(_mm_add_ps(_mm_mul_ps(y, aVal), exp_p4), aVal);
        y = _mm_add_ps(_mm_mul_ps(_mm_add_ps(y, exp_p5), z), aVal);
        y = _mm_add_ps(y, one);

        emm0 = _mm_slli_epi32(_mm_add_epi32(_mm_cvttps_epi32(fx), pi32_0x7f), 23);

        pow2n = _mm_castsi128_ps(emm0);
        bVal = _mm_mul_ps(y, pow2n);

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
  \brief Computes accurate exp (based on cephes math lib) of input vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which exp is to be computed
*/

static inline void volk_32f_exp_32f_u_generic(float* bVector, const float* aVector, unsigned int num_points){
    float* bPtr = bVector;
    const float* aPtr = aVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
        *bPtr++ = expf(*aPtr++);
    }

}

#endif /* LV_HAVE_GENERIC */

#endif /* INCLUDED_volk_32f_exp_32f_u_H */

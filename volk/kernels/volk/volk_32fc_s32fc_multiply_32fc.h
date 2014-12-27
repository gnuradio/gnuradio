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

#ifndef INCLUDED_volk_32fc_s32fc_multiply_32fc_u_H
#define INCLUDED_volk_32fc_s32fc_multiply_32fc_u_H

#include <inttypes.h>
#include <stdio.h>
#include <volk/volk_complex.h>
#include <float.h>

#ifdef LV_HAVE_AVX
#include <immintrin.h>
  /*!
    \brief Multiplies the two input complex vectors and stores their results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector One of the vectors to be multiplied
    \param bVector One of the vectors to be multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_s32fc_multiply_32fc_u_avx(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t scalar, unsigned int num_points){
    unsigned int number = 0;
    unsigned int i = 0;
    const unsigned int quarterPoints = num_points / 4;
    unsigned int isodd = num_points & 3;
    __m256 x, yl, yh, z, tmp1, tmp2;
    lv_32fc_t* c = cVector;
    const lv_32fc_t* a = aVector;

    // Set up constant scalar vector
    yl = _mm256_set1_ps(lv_creal(scalar));
    yh = _mm256_set1_ps(lv_cimag(scalar));

    for(;number < quarterPoints; number++){
      x = _mm256_loadu_ps((float*)a); // Load the ar + ai, br + bi as ar,ai,br,bi

      tmp1 = _mm256_mul_ps(x,yl); // tmp1 = ar*cr,ai*cr,br*dr,bi*dr

      x = _mm256_shuffle_ps(x,x,0xB1); // Re-arrange x to be ai,ar,bi,br

      tmp2 = _mm256_mul_ps(x,yh); // tmp2 = ai*ci,ar*ci,bi*di,br*di

      z = _mm256_addsub_ps(tmp1,tmp2); // ar*cr-ai*ci, ai*cr+ar*ci, br*dr-bi*di, bi*dr+br*di

      _mm256_storeu_ps((float*)c,z); // Store the results back into the C container

      a += 4;
      c += 4;
    }

    for(i = num_points-isodd; i < num_points; i++) {
        *c++ = (*a++) * scalar;
    }

}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE3
#include <pmmintrin.h>
/*!
  \brief Multiplies the input vector by a scalar and stores the results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector The vector to be multiplied
  \param scalar The complex scalar to multiply aVector
  \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32fc_s32fc_multiply_32fc_u_sse3(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t scalar, unsigned int num_points){
  unsigned int number = 0;
    const unsigned int halfPoints = num_points / 2;

    __m128 x, yl, yh, z, tmp1, tmp2;
    lv_32fc_t* c = cVector;
    const lv_32fc_t* a = aVector;

    // Set up constant scalar vector
    yl = _mm_set_ps1(lv_creal(scalar));
    yh = _mm_set_ps1(lv_cimag(scalar));

    for(;number < halfPoints; number++){

      x = _mm_loadu_ps((float*)a); // Load the ar + ai, br + bi as ar,ai,br,bi

      tmp1 = _mm_mul_ps(x,yl); // tmp1 = ar*cr,ai*cr,br*dr,bi*dr

      x = _mm_shuffle_ps(x,x,0xB1); // Re-arrange x to be ai,ar,bi,br

      tmp2 = _mm_mul_ps(x,yh); // tmp2 = ai*ci,ar*ci,bi*di,br*di

      z = _mm_addsub_ps(tmp1,tmp2); // ar*cr-ai*ci, ai*cr+ar*ci, br*dr-bi*di, bi*dr+br*di

      _mm_storeu_ps((float*)c,z); // Store the results back into the C container

      a += 2;
      c += 2;
    }

    if((num_points % 2) != 0) {
      *c = (*a) * scalar;
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Multiplies the input vector by a scalar and stores the results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector The vector to be multiplied
  \param scalar The complex scalar to multiply aVector
  \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32fc_s32fc_multiply_32fc_generic(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t scalar, unsigned int num_points){
    lv_32fc_t* cPtr = cVector;
    const lv_32fc_t* aPtr = aVector;
    unsigned int number = num_points;

    // unwrap loop
    while (number >= 8){
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      number -= 8;
    }

    // clean up any remaining
    while (number-- > 0)
      *cPtr++ = *aPtr++ * scalar;
}
#endif /* LV_HAVE_GENERIC */


#endif /* INCLUDED_volk_32fc_x2_multiply_32fc_u_H */
#ifndef INCLUDED_volk_32fc_s32fc_multiply_32fc_a_H
#define INCLUDED_volk_32fc_s32fc_multiply_32fc_a_H

#include <inttypes.h>
#include <stdio.h>
#include <volk/volk_complex.h>
#include <float.h>

#ifdef LV_HAVE_AVX
#include <immintrin.h>
  /*!
    \brief Multiplies the two input complex vectors and stores their results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector One of the vectors to be multiplied
    \param bVector One of the vectors to be multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_s32fc_multiply_32fc_a_avx(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t scalar, unsigned int num_points){
    unsigned int number = 0;
    unsigned int i = 0;
    const unsigned int quarterPoints = num_points / 4;
    unsigned int isodd = num_points & 3;
    __m256 x, yl, yh, z, tmp1, tmp2;
    lv_32fc_t* c = cVector;
    const lv_32fc_t* a = aVector;

    // Set up constant scalar vector
    yl = _mm256_set1_ps(lv_creal(scalar));
    yh = _mm256_set1_ps(lv_cimag(scalar));

    for(;number < quarterPoints; number++){
      x = _mm256_load_ps((float*)a); // Load the ar + ai, br + bi as ar,ai,br,bi

      tmp1 = _mm256_mul_ps(x,yl); // tmp1 = ar*cr,ai*cr,br*dr,bi*dr

      x = _mm256_shuffle_ps(x,x,0xB1); // Re-arrange x to be ai,ar,bi,br

      tmp2 = _mm256_mul_ps(x,yh); // tmp2 = ai*ci,ar*ci,bi*di,br*di

      z = _mm256_addsub_ps(tmp1,tmp2); // ar*cr-ai*ci, ai*cr+ar*ci, br*dr-bi*di, bi*dr+br*di

      _mm256_store_ps((float*)c,z); // Store the results back into the C container

      a += 4;
      c += 4;
    }

    for(i = num_points-isodd; i < num_points; i++) {
        *c++ = (*a++) * scalar;
    }

}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE3
#include <pmmintrin.h>
  /*!
    \brief Multiplies the two input complex vectors and stores their results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector One of the vectors to be multiplied
    \param bVector One of the vectors to be multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_s32fc_multiply_32fc_a_sse3(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t scalar, unsigned int num_points){
  unsigned int number = 0;
    const unsigned int halfPoints = num_points / 2;

    __m128 x, yl, yh, z, tmp1, tmp2;
    lv_32fc_t* c = cVector;
    const lv_32fc_t* a = aVector;

    // Set up constant scalar vector
    yl = _mm_set_ps1(lv_creal(scalar));
    yh = _mm_set_ps1(lv_cimag(scalar));

    for(;number < halfPoints; number++){

      x = _mm_load_ps((float*)a); // Load the ar + ai, br + bi as ar,ai,br,bi

      tmp1 = _mm_mul_ps(x,yl); // tmp1 = ar*cr,ai*cr,br*dr,bi*dr

      x = _mm_shuffle_ps(x,x,0xB1); // Re-arrange x to be ai,ar,bi,br

      tmp2 = _mm_mul_ps(x,yh); // tmp2 = ai*ci,ar*ci,bi*di,br*di

      z = _mm_addsub_ps(tmp1,tmp2); // ar*cr-ai*ci, ai*cr+ar*ci, br*dr-bi*di, bi*dr+br*di

      _mm_store_ps((float*)c,z); // Store the results back into the C container

      a += 2;
      c += 2;
    }

    if((num_points % 2) != 0) {
      *c = (*a) * scalar;
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
  /*!
    \brief Multiplies the two input complex vectors and stores their results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector One of the vectors to be multiplied
    \param bVector One of the vectors to be multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_s32fc_multiply_32fc_neon(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t scalar, unsigned int num_points){
    lv_32fc_t* cPtr = cVector;
    const lv_32fc_t* aPtr = aVector;
    unsigned int number = num_points;
    unsigned int quarter_points = num_points / 4;

    float32x4x2_t a_val, scalar_val;
    float32x4x2_t tmp_imag;

    scalar_val = vld2q_f32((const float*)&scalar);
    for(number = 0; number < quarter_points; ++number) {
        a_val = vld2q_f32((float*)aPtr);
        tmp_imag.val[1] = vmulq_f32(a_val.val[1], scalar_val.val[0]);
        tmp_imag.val[0] = vmulq_f32(a_val.val[0], scalar_val.val[0]);

        tmp_imag.val[1] = vmlaq_f32(tmp_imag.val[1], a_val.val[0], scalar_val.val[1]);
        tmp_imag.val[0] = vmlaq_f32(tmp_imag.val[0], a_val.val[1], scalar_val.val[1]);

        vst2q_f32((float*)cVector, tmp_imag);
        aPtr += 4;
        cVector += 4;
    }

    for(number = quarter_points*4; number < num_points; number++){
      *cPtr++ = *aPtr++ * scalar;
    }
}
#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Multiplies the two input complex vectors and stores their results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector One of the vectors to be multiplied
    \param bVector One of the vectors to be multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_s32fc_multiply_32fc_a_generic(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t scalar, unsigned int num_points){
    lv_32fc_t* cPtr = cVector;
    const lv_32fc_t* aPtr = aVector;
    unsigned int number = num_points;

    // unwrap loop
    while (number >= 8){
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      number -= 8;
    }

    // clean up any remaining
    while (number-- > 0)
      *cPtr++ = *aPtr++ * scalar;
}
#endif /* LV_HAVE_GENERIC */

#endif /* INCLUDED_volk_32fc_x2_multiply_32fc_a_H */

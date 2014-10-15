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

#ifndef INCLUDED_volk_32fc_x2_multiply_conjugate_32fc_u_H
#define INCLUDED_volk_32fc_x2_multiply_conjugate_32fc_u_H

#include <inttypes.h>
#include <stdio.h>
#include <volk/volk_complex.h>
#include <float.h>

#ifdef LV_HAVE_AVX
#include <immintrin.h>
  /*!
    \brief Multiplies vector a by the conjugate of vector b and stores the results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector First vector to be multiplied
    \param bVector Second vector that is conjugated before being multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_x2_multiply_conjugate_32fc_u_avx(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t* bVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    __m256 x, y, yl, yh, z, tmp1, tmp2;
    lv_32fc_t* c = cVector;
    const lv_32fc_t* a = aVector;
    const lv_32fc_t* b = bVector;

    __m256 conjugator = _mm256_setr_ps(0, -0.f, 0, -0.f, 0, -0.f, 0, -0.f);

    for(;number < quarterPoints; number++){

      x = _mm256_loadu_ps((float*)a); // Load the ar + ai, br + bi ... as ar,ai,br,bi ...
      y = _mm256_loadu_ps((float*)b); // Load the cr + ci, dr + di ... as cr,ci,dr,di ...

      y = _mm256_xor_ps(y, conjugator); // conjugate y

      yl = _mm256_moveldup_ps(y); // Load yl with cr,cr,dr,dr ...
      yh = _mm256_movehdup_ps(y); // Load yh with ci,ci,di,di ...

      tmp1 = _mm256_mul_ps(x,yl); // tmp1 = ar*cr,ai*cr,br*dr,bi*dr ...

      x = _mm256_shuffle_ps(x,x,0xB1); // Re-arrange x to be ai,ar,bi,br ...

      tmp2 = _mm256_mul_ps(x,yh); // tmp2 = ai*ci,ar*ci,bi*di,br*di ...

      z = _mm256_addsub_ps(tmp1,tmp2); // ar*cr-ai*ci, ai*cr+ar*ci, br*dr-bi*di, bi*dr+br*di ...

      _mm256_storeu_ps((float*)c,z); // Store the results back into the C container

      a += 4;
      b += 4;
      c += 4;
    }

    number = quarterPoints * 4;

    for(; number < num_points; number++) {
      *c++ = (*a++) * lv_conj(*b++);
    }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE3
#include <pmmintrin.h>
  /*!
    \brief Multiplies vector a by the conjugate of vector b and stores the results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector First vector to be multiplied
    \param bVector Second vector that is conjugated before being multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_x2_multiply_conjugate_32fc_u_sse3(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t* bVector, unsigned int num_points){
  unsigned int number = 0;
    const unsigned int halfPoints = num_points / 2;

    __m128 x, y, yl, yh, z, tmp1, tmp2;
    lv_32fc_t* c = cVector;
    const lv_32fc_t* a = aVector;
    const lv_32fc_t* b = bVector;

    __m128 conjugator = _mm_setr_ps(0, -0.f, 0, -0.f);

    for(;number < halfPoints; number++){

      x = _mm_loadu_ps((float*)a); // Load the ar + ai, br + bi as ar,ai,br,bi
      y = _mm_loadu_ps((float*)b); // Load the cr + ci, dr + di as cr,ci,dr,di

      y = _mm_xor_ps(y, conjugator); // conjugate y

      yl = _mm_moveldup_ps(y); // Load yl with cr,cr,dr,dr
      yh = _mm_movehdup_ps(y); // Load yh with ci,ci,di,di

      tmp1 = _mm_mul_ps(x,yl); // tmp1 = ar*cr,ai*cr,br*dr,bi*dr

      x = _mm_shuffle_ps(x,x,0xB1); // Re-arrange x to be ai,ar,bi,br

      tmp2 = _mm_mul_ps(x,yh); // tmp2 = ai*ci,ar*ci,bi*di,br*di

      z = _mm_addsub_ps(tmp1,tmp2); // ar*cr-ai*ci, ai*cr+ar*ci, br*dr-bi*di, bi*dr+br*di

      _mm_storeu_ps((float*)c,z); // Store the results back into the C container

      a += 2;
      b += 2;
      c += 2;
    }

    if((num_points % 2) != 0) {
      *c = (*a) * lv_conj(*b);
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Multiplies vector a by the conjugate of vector b and stores the results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector First vector to be multiplied
    \param bVector Second vector that is conjugated before being multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_x2_multiply_conjugate_32fc_generic(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t* bVector, unsigned int num_points){
    lv_32fc_t* cPtr = cVector;
    const lv_32fc_t* aPtr = aVector;
    const lv_32fc_t* bPtr=  bVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
      *cPtr++ = (*aPtr++) * lv_conj(*bPtr++);
    }
}
#endif /* LV_HAVE_GENERIC */


#endif /* INCLUDED_volk_32fc_x2_multiply_conjugate_32fc_u_H */
#ifndef INCLUDED_volk_32fc_x2_multiply_conjugate_32fc_a_H
#define INCLUDED_volk_32fc_x2_multiply_conjugate_32fc_a_H

#include <inttypes.h>
#include <stdio.h>
#include <volk/volk_complex.h>
#include <float.h>

#ifdef LV_HAVE_AVX
#include <immintrin.h>
  /*!
    \brief Multiplies vector a by the conjugate of vector b and stores the results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector First vector to be multiplied
    \param bVector Second vector that is conjugated before being multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_x2_multiply_conjugate_32fc_a_avx(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t* bVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    __m256 x, y, yl, yh, z, tmp1, tmp2;
    lv_32fc_t* c = cVector;
    const lv_32fc_t* a = aVector;
    const lv_32fc_t* b = bVector;

    __m256 conjugator = _mm256_setr_ps(0, -0.f, 0, -0.f, 0, -0.f, 0, -0.f);

    for(;number < quarterPoints; number++){

      x = _mm256_load_ps((float*)a); // Load the ar + ai, br + bi ... as ar,ai,br,bi ...
      y = _mm256_load_ps((float*)b); // Load the cr + ci, dr + di ... as cr,ci,dr,di ...

      y = _mm256_xor_ps(y, conjugator); // conjugate y

      yl = _mm256_moveldup_ps(y); // Load yl with cr,cr,dr,dr ...
      yh = _mm256_movehdup_ps(y); // Load yh with ci,ci,di,di ...

      tmp1 = _mm256_mul_ps(x,yl); // tmp1 = ar*cr,ai*cr,br*dr,bi*dr ...

      x = _mm256_shuffle_ps(x,x,0xB1); // Re-arrange x to be ai,ar,bi,br ...

      tmp2 = _mm256_mul_ps(x,yh); // tmp2 = ai*ci,ar*ci,bi*di,br*di ...

      z = _mm256_addsub_ps(tmp1,tmp2); // ar*cr-ai*ci, ai*cr+ar*ci, br*dr-bi*di, bi*dr+br*di ...

      _mm256_store_ps((float*)c,z); // Store the results back into the C container

      a += 4;
      b += 4;
      c += 4;
    }

    number = quarterPoints * 4;

    for(; number < num_points; number++) {
      *c++ = (*a++) * lv_conj(*b++);
    }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE3
#include <pmmintrin.h>
  /*!
    \brief Multiplies vector a by the conjugate of vector b and stores the results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector First vector to be multiplied
    \param bVector Second vector that is conjugated before being multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_x2_multiply_conjugate_32fc_a_sse3(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t* bVector, unsigned int num_points){
  unsigned int number = 0;
    const unsigned int halfPoints = num_points / 2;

    __m128 x, y, yl, yh, z, tmp1, tmp2;
    lv_32fc_t* c = cVector;
    const lv_32fc_t* a = aVector;
    const lv_32fc_t* b = bVector;

    __m128 conjugator = _mm_setr_ps(0, -0.f, 0, -0.f);

    for(;number < halfPoints; number++){

      x = _mm_load_ps((float*)a); // Load the ar + ai, br + bi as ar,ai,br,bi
      y = _mm_load_ps((float*)b); // Load the cr + ci, dr + di as cr,ci,dr,di

      y = _mm_xor_ps(y, conjugator); // conjugate y

      yl = _mm_moveldup_ps(y); // Load yl with cr,cr,dr,dr
      yh = _mm_movehdup_ps(y); // Load yh with ci,ci,di,di

      tmp1 = _mm_mul_ps(x,yl); // tmp1 = ar*cr,ai*cr,br*dr,bi*dr

      x = _mm_shuffle_ps(x,x,0xB1); // Re-arrange x to be ai,ar,bi,br

      tmp2 = _mm_mul_ps(x,yh); // tmp2 = ai*ci,ar*ci,bi*di,br*di

      z = _mm_addsub_ps(tmp1,tmp2); // ar*cr-ai*ci, ai*cr+ar*ci, br*dr-bi*di, bi*dr+br*di

      _mm_store_ps((float*)c,z); // Store the results back into the C container

      a += 2;
      b += 2;
      c += 2;
    }

    if((num_points % 2) != 0) {
      *c = (*a) * lv_conj(*b);
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
  /*!
    \brief Multiplies vector a by the conjugate of vector b and stores the results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector First vector to be multiplied
    \param bVector Second vector that is conjugated before being multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_x2_multiply_conjugate_32fc_neon(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t* bVector, unsigned int num_points){

    lv_32fc_t *a_ptr = (lv_32fc_t*) aVector;
    lv_32fc_t *b_ptr = (lv_32fc_t*) bVector;
    unsigned int quarter_points = num_points / 4;
    float32x4x2_t a_val, b_val, c_val;
    float32x4x2_t tmp_real, tmp_imag;
    unsigned int number = 0;

    for(number = 0; number < quarter_points; ++number) {
        a_val = vld2q_f32((float*)a_ptr); // a0r|a1r|a2r|a3r || a0i|a1i|a2i|a3i
        b_val = vld2q_f32((float*)b_ptr); // b0r|b1r|b2r|b3r || b0i|b1i|b2i|b3i
        b_val.val[1] = vnegq_f32(b_val.val[1]);
        __builtin_prefetch(a_ptr+4);
        __builtin_prefetch(b_ptr+4);

        // multiply the real*real and imag*imag to get real result
        // a0r*b0r|a1r*b1r|a2r*b2r|a3r*b3r
        tmp_real.val[0] = vmulq_f32(a_val.val[0], b_val.val[0]);
        // a0i*b0i|a1i*b1i|a2i*b2i|a3i*b3i
        tmp_real.val[1] = vmulq_f32(a_val.val[1], b_val.val[1]);

        // Multiply cross terms to get the imaginary result
        // a0r*b0i|a1r*b1i|a2r*b2i|a3r*b3i
        tmp_imag.val[0] = vmulq_f32(a_val.val[0], b_val.val[1]);
        // a0i*b0r|a1i*b1r|a2i*b2r|a3i*b3r
        tmp_imag.val[1] = vmulq_f32(a_val.val[1], b_val.val[0]);

        // store the results
        c_val.val[0] = vsubq_f32(tmp_real.val[0], tmp_real.val[1]);
        c_val.val[1] = vaddq_f32(tmp_imag.val[0], tmp_imag.val[1]);
        vst2q_f32((float*)cVector, c_val);

        a_ptr += 4;
        b_ptr += 4;
        cVector += 4;
    }

    for(number = quarter_points*4; number < num_points; number++){
      *cVector++ = (*a_ptr++) * conj(*b_ptr++);
    }
}
#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Multiplies vector a by the conjugate of vector b and stores the results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector First vector to be multiplied
    \param bVector Second vector that is conjugated before being multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_x2_multiply_conjugate_32fc_a_generic(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t* bVector, unsigned int num_points){
    lv_32fc_t* cPtr = cVector;
    const lv_32fc_t* aPtr = aVector;
    const lv_32fc_t* bPtr=  bVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
      *cPtr++ = (*aPtr++) * lv_conj(*bPtr++);
    }
}
#endif /* LV_HAVE_GENERIC */


#endif /* INCLUDED_volk_32fc_x2_multiply_conjugate_32fc_a_H */

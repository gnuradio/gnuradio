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

#ifndef INCLUDED_volk_16i_32fc_dot_prod_32fc_H
#define INCLUDED_volk_16i_32fc_dot_prod_32fc_H

#include <volk/volk_common.h>
#include<stdio.h>


#ifdef LV_HAVE_GENERIC

static inline void volk_16i_32fc_dot_prod_32fc_generic(lv_32fc_t* result, const short* input, const lv_32fc_t * taps, unsigned int num_points) {

  static const int N_UNROLL = 4;

  lv_32fc_t acc0 = 0;
  lv_32fc_t acc1 = 0;
  lv_32fc_t acc2 = 0;
  lv_32fc_t acc3 = 0;

  unsigned i = 0;
  unsigned n = (num_points / N_UNROLL) * N_UNROLL;

  for(i = 0; i < n; i += N_UNROLL) {
    acc0 += taps[i + 0] * (float)input[i + 0];
    acc1 += taps[i + 1] * (float)input[i + 1];
    acc2 += taps[i + 2] * (float)input[i + 2];
    acc3 += taps[i + 3] * (float)input[i + 3];
  }

  for(; i < num_points; i++) {
    acc0 += taps[i] * (float)input[i];
  }

  *result = acc0 + acc1 + acc2 + acc3;
}

#endif /*LV_HAVE_GENERIC*/

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
static inline void volk_16i_32fc_dot_prod_32fc_neon(lv_32fc_t* result, const short* input, const lv_32fc_t * taps, unsigned int num_points) {

  unsigned ii;
  unsigned quarter_points = num_points / 4;
  lv_32fc_t* tapsPtr = (lv_32fc_t*) taps;
  short* inputPtr = (short*) input;
  lv_32fc_t accumulator_vec[4];

  float32x4x2_t tapsVal, accumulator_val;
  int16x4_t input16;
  int32x4_t input32;
  float32x4_t input_float, prod_re, prod_im;

  accumulator_val.val[0] = vdupq_n_f32(0.0);
  accumulator_val.val[1] = vdupq_n_f32(0.0);

  for(ii = 0; ii < quarter_points; ++ii) {
    tapsVal = vld2q_f32((float*)tapsPtr);
    input16 = vld1_s16(inputPtr);
    // widen 16-bit int to 32-bit int
    input32 = vmovl_s16(input16);
    // convert 32-bit int to float with scale
    input_float = vcvtq_f32_s32(input32);

    prod_re = vmulq_f32(input_float, tapsVal.val[0]);
    prod_im = vmulq_f32(input_float, tapsVal.val[1]);

    accumulator_val.val[0] = vaddq_f32(prod_re, accumulator_val.val[0]);
    accumulator_val.val[1] = vaddq_f32(prod_im, accumulator_val.val[1]);

    tapsPtr += 4;
    inputPtr += 4;
  }
  vst2q_f32((float*)accumulator_vec, accumulator_val);
  accumulator_vec[0] += accumulator_vec[1];
  accumulator_vec[2] += accumulator_vec[3];
  accumulator_vec[0] += accumulator_vec[2];

  for(ii = quarter_points * 4; ii < num_points; ++ii) {
    accumulator_vec[0] += *(tapsPtr++) * (float)(*(inputPtr++));
  }

  *result = accumulator_vec[0];
}

#endif /*LV_HAVE_NEON*/

#if LV_HAVE_SSE && LV_HAVE_MMX

static inline void volk_16i_32fc_dot_prod_32fc_u_sse( lv_32fc_t* result, const  short* input, const  lv_32fc_t* taps, unsigned int num_points) {

  unsigned int number = 0;
  const unsigned int sixteenthPoints = num_points / 8;

  float res[2];
  float *realpt = &res[0], *imagpt = &res[1];
  const short* aPtr = input;
  const float* bPtr = (float*)taps;

  __m64  m0, m1;
  __m128 f0, f1, f2, f3;
  __m128 a0Val, a1Val, a2Val, a3Val;
  __m128 b0Val, b1Val, b2Val, b3Val;
  __m128 c0Val, c1Val, c2Val, c3Val;

  __m128 dotProdVal0 = _mm_setzero_ps();
  __m128 dotProdVal1 = _mm_setzero_ps();
  __m128 dotProdVal2 = _mm_setzero_ps();
  __m128 dotProdVal3 = _mm_setzero_ps();

  for(;number < sixteenthPoints; number++){

    m0 = _mm_set_pi16(*(aPtr+3), *(aPtr+2), *(aPtr+1), *(aPtr+0));
    m1 = _mm_set_pi16(*(aPtr+7), *(aPtr+6), *(aPtr+5), *(aPtr+4));
    f0 = _mm_cvtpi16_ps(m0);
    f1 = _mm_cvtpi16_ps(m0);
    f2 = _mm_cvtpi16_ps(m1);
    f3 = _mm_cvtpi16_ps(m1);

    a0Val = _mm_unpacklo_ps(f0, f1);
    a1Val = _mm_unpackhi_ps(f0, f1);
    a2Val = _mm_unpacklo_ps(f2, f3);
    a3Val = _mm_unpackhi_ps(f2, f3);

    b0Val = _mm_loadu_ps(bPtr);
    b1Val = _mm_loadu_ps(bPtr+4);
    b2Val = _mm_loadu_ps(bPtr+8);
    b3Val = _mm_loadu_ps(bPtr+12);

    c0Val = _mm_mul_ps(a0Val, b0Val);
    c1Val = _mm_mul_ps(a1Val, b1Val);
    c2Val = _mm_mul_ps(a2Val, b2Val);
    c3Val = _mm_mul_ps(a3Val, b3Val);

    dotProdVal0 = _mm_add_ps(c0Val, dotProdVal0);
    dotProdVal1 = _mm_add_ps(c1Val, dotProdVal1);
    dotProdVal2 = _mm_add_ps(c2Val, dotProdVal2);
    dotProdVal3 = _mm_add_ps(c3Val, dotProdVal3);

    aPtr += 8;
    bPtr += 16;
  }

  dotProdVal0 = _mm_add_ps(dotProdVal0, dotProdVal1);
  dotProdVal0 = _mm_add_ps(dotProdVal0, dotProdVal2);
  dotProdVal0 = _mm_add_ps(dotProdVal0, dotProdVal3);

  __VOLK_ATTR_ALIGNED(16) float dotProductVector[4];

  _mm_store_ps(dotProductVector,dotProdVal0); // Store the results back into the dot product vector

  *realpt = dotProductVector[0];
  *imagpt = dotProductVector[1];
  *realpt += dotProductVector[2];
  *imagpt += dotProductVector[3];

  number = sixteenthPoints*8;
  for(;number < num_points; number++){
    *realpt += ((*aPtr)   * (*bPtr++));
    *imagpt += ((*aPtr++) * (*bPtr++));
  }

  *result = *(lv_32fc_t*)(&res[0]);
}

#endif /*LV_HAVE_SSE && LV_HAVE_MMX*/




#if LV_HAVE_SSE && LV_HAVE_MMX


static inline void volk_16i_32fc_dot_prod_32fc_a_sse( lv_32fc_t* result, const  short* input, const  lv_32fc_t* taps, unsigned int num_points) {

  unsigned int number = 0;
  const unsigned int sixteenthPoints = num_points / 8;

  float res[2];
  float *realpt = &res[0], *imagpt = &res[1];
  const short* aPtr = input;
  const float* bPtr = (float*)taps;

  __m64  m0, m1;
  __m128 f0, f1, f2, f3;
  __m128 a0Val, a1Val, a2Val, a3Val;
  __m128 b0Val, b1Val, b2Val, b3Val;
  __m128 c0Val, c1Val, c2Val, c3Val;

  __m128 dotProdVal0 = _mm_setzero_ps();
  __m128 dotProdVal1 = _mm_setzero_ps();
  __m128 dotProdVal2 = _mm_setzero_ps();
  __m128 dotProdVal3 = _mm_setzero_ps();

  for(;number < sixteenthPoints; number++){

    m0 = _mm_set_pi16(*(aPtr+3), *(aPtr+2), *(aPtr+1), *(aPtr+0));
    m1 = _mm_set_pi16(*(aPtr+7), *(aPtr+6), *(aPtr+5), *(aPtr+4));
    f0 = _mm_cvtpi16_ps(m0);
    f1 = _mm_cvtpi16_ps(m0);
    f2 = _mm_cvtpi16_ps(m1);
    f3 = _mm_cvtpi16_ps(m1);

    a0Val = _mm_unpacklo_ps(f0, f1);
    a1Val = _mm_unpackhi_ps(f0, f1);
    a2Val = _mm_unpacklo_ps(f2, f3);
    a3Val = _mm_unpackhi_ps(f2, f3);

    b0Val = _mm_load_ps(bPtr);
    b1Val = _mm_load_ps(bPtr+4);
    b2Val = _mm_load_ps(bPtr+8);
    b3Val = _mm_load_ps(bPtr+12);

    c0Val = _mm_mul_ps(a0Val, b0Val);
    c1Val = _mm_mul_ps(a1Val, b1Val);
    c2Val = _mm_mul_ps(a2Val, b2Val);
    c3Val = _mm_mul_ps(a3Val, b3Val);

    dotProdVal0 = _mm_add_ps(c0Val, dotProdVal0);
    dotProdVal1 = _mm_add_ps(c1Val, dotProdVal1);
    dotProdVal2 = _mm_add_ps(c2Val, dotProdVal2);
    dotProdVal3 = _mm_add_ps(c3Val, dotProdVal3);

    aPtr += 8;
    bPtr += 16;
  }

  dotProdVal0 = _mm_add_ps(dotProdVal0, dotProdVal1);
  dotProdVal0 = _mm_add_ps(dotProdVal0, dotProdVal2);
  dotProdVal0 = _mm_add_ps(dotProdVal0, dotProdVal3);

  __VOLK_ATTR_ALIGNED(16) float dotProductVector[4];

  _mm_store_ps(dotProductVector,dotProdVal0); // Store the results back into the dot product vector

  *realpt = dotProductVector[0];
  *imagpt = dotProductVector[1];
  *realpt += dotProductVector[2];
  *imagpt += dotProductVector[3];

  number = sixteenthPoints*8;
  for(;number < num_points; number++){
    *realpt += ((*aPtr)   * (*bPtr++));
    *imagpt += ((*aPtr++) * (*bPtr++));
  }

  *result = *(lv_32fc_t*)(&res[0]);
}

#endif /*LV_HAVE_SSE && LV_HAVE_MMX*/


#endif /*INCLUDED_volk_16i_32fc_dot_prod_32fc_H*/

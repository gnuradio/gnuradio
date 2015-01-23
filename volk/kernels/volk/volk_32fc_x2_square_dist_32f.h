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

#ifndef INCLUDED_volk_32fc_x2_square_dist_32f_a_H
#define INCLUDED_volk_32fc_x2_square_dist_32f_a_H

#include<inttypes.h>
#include<stdio.h>
#include<volk/volk_complex.h>

#ifdef LV_HAVE_SSE3
#include<xmmintrin.h>
#include<pmmintrin.h>

static inline void volk_32fc_x2_square_dist_32f_a_sse3(float* target, lv_32fc_t* src0, lv_32fc_t* points, unsigned int num_points) {

  const unsigned int num_bytes = num_points*8;

  __m128 xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;

  lv_32fc_t diff;
  float sq_dist;
  int bound = num_bytes >> 5;
  int leftovers0 = (num_bytes >> 4) & 1;
  int leftovers1 = (num_bytes >> 3) & 1;
  int i = 0;

  xmm1 = _mm_setzero_ps();
  xmm1 = _mm_loadl_pi(xmm1, (__m64*)src0);
  xmm2 = _mm_load_ps((float*)&points[0]);
  xmm1 = _mm_movelh_ps(xmm1, xmm1);
  xmm3 = _mm_load_ps((float*)&points[2]);


  for(; i < bound - 1; ++i) {
    xmm4 = _mm_sub_ps(xmm1, xmm2);
    xmm5 = _mm_sub_ps(xmm1, xmm3);
    points += 4;
    xmm6 = _mm_mul_ps(xmm4, xmm4);
    xmm7 = _mm_mul_ps(xmm5, xmm5);

    xmm2 = _mm_load_ps((float*)&points[0]);

    xmm4 = _mm_hadd_ps(xmm6, xmm7);

    xmm3 = _mm_load_ps((float*)&points[2]);

    _mm_store_ps(target, xmm4);

    target += 4;

  }

  xmm4 = _mm_sub_ps(xmm1, xmm2);
  xmm5 = _mm_sub_ps(xmm1, xmm3);



  points += 4;
  xmm6 = _mm_mul_ps(xmm4, xmm4);
  xmm7 = _mm_mul_ps(xmm5, xmm5);

  xmm4 = _mm_hadd_ps(xmm6, xmm7);

  _mm_store_ps(target, xmm4);

  target += 4;

  for(i = 0; i < leftovers0; ++i) {

    xmm2 = _mm_load_ps((float*)&points[0]);

    xmm4 = _mm_sub_ps(xmm1, xmm2);

    points += 2;

    xmm6 = _mm_mul_ps(xmm4, xmm4);

    xmm4 = _mm_hadd_ps(xmm6, xmm6);

    _mm_storeh_pi((__m64*)target, xmm4);

    target += 2;
  }

  for(i = 0; i < leftovers1; ++i) {

    diff = src0[0] - points[0];

    sq_dist = lv_creal(diff) * lv_creal(diff) + lv_cimag(diff) * lv_cimag(diff);

    target[0] = sq_dist;
  }
}

#endif /*LV_HAVE_SSE3*/

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
static inline void volk_32fc_x2_square_dist_32f_neon(float* target, lv_32fc_t* src0, lv_32fc_t* points, unsigned int num_points) {
    const unsigned int quarter_points = num_points / 4;
    unsigned int number;

    float32x4x2_t a_vec, b_vec;
    float32x4x2_t diff_vec;
    float32x4_t tmp, tmp1, dist_sq;
    a_vec.val[0] = vdupq_n_f32( lv_creal(src0[0]) );
    a_vec.val[1] = vdupq_n_f32( lv_cimag(src0[0]) );
    for(number=0; number < quarter_points; ++number) {
        b_vec = vld2q_f32((float*)points);
        diff_vec.val[0] = vsubq_f32(a_vec.val[0], b_vec.val[0]);
        diff_vec.val[1] = vsubq_f32(a_vec.val[1], b_vec.val[1]);
        tmp = vmulq_f32(diff_vec.val[0], diff_vec.val[0]);
        tmp1 = vmulq_f32(diff_vec.val[1], diff_vec.val[1]);

        dist_sq = vaddq_f32(tmp, tmp1);
        vst1q_f32(target, dist_sq);
        points += 4;
        target += 4;
    }
    for(number=quarter_points*4; number < num_points; ++number) {
        lv_32fc_t diff = src0[0] - *points++;
        *target++ = lv_creal(diff) * lv_creal(diff) + lv_cimag(diff) * lv_cimag(diff);
    }
}
#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_GENERIC
static inline void volk_32fc_x2_square_dist_32f_generic(float* target, lv_32fc_t* src0, lv_32fc_t* points, unsigned int num_points) {

  const unsigned int num_bytes = num_points*8;

  lv_32fc_t diff;
  float sq_dist;
  unsigned int i = 0;

  for(; i < num_bytes >> 3; ++i) {
    diff = src0[0] - points[i];

    sq_dist = lv_creal(diff) * lv_creal(diff) + lv_cimag(diff) * lv_cimag(diff);

    target[i] = sq_dist;
  }
}

#endif /*LV_HAVE_GENERIC*/


#endif /*INCLUDED_volk_32fc_x2_square_dist_32f_a_H*/

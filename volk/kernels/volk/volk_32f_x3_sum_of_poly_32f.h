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

#ifndef INCLUDED_volk_32f_x3_sum_of_poly_32f_a_H
#define INCLUDED_volk_32f_x3_sum_of_poly_32f_a_H

#include<inttypes.h>
#include<stdio.h>
#include<volk/volk_complex.h>

#ifndef MAX
#define MAX(X,Y) ((X) > (Y)?(X):(Y))
#endif

#ifdef LV_HAVE_SSE3
#include<xmmintrin.h>
#include<pmmintrin.h>

static inline void volk_32f_x3_sum_of_poly_32f_a_sse3(float* target, float* src0, float* center_point_array, float* cutoff, unsigned int num_points) {

  const unsigned int num_bytes = num_points*4;

  float result = 0.0;
  float fst = 0.0;
  float sq = 0.0;
  float thrd = 0.0;
  float frth = 0.0;
  //float fith = 0.0;



  __m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10;// xmm11, xmm12;

  xmm9 = _mm_setzero_ps();
  xmm1 = _mm_setzero_ps();

  xmm0 = _mm_load1_ps(&center_point_array[0]);
  xmm6 = _mm_load1_ps(&center_point_array[1]);
  xmm7 = _mm_load1_ps(&center_point_array[2]);
  xmm8 = _mm_load1_ps(&center_point_array[3]);
  //xmm11 = _mm_load1_ps(&center_point_array[4]);
  xmm10 = _mm_load1_ps(cutoff);

  int bound = num_bytes >> 4;
  int leftovers = (num_bytes >> 2) & 3;
  int i = 0;

  for(; i < bound; ++i) {
    xmm2 = _mm_load_ps(src0);
    xmm2 = _mm_max_ps(xmm10, xmm2);
    xmm3 = _mm_mul_ps(xmm2, xmm2);
    xmm4 = _mm_mul_ps(xmm2, xmm3);
    xmm5 = _mm_mul_ps(xmm3, xmm3);
    //xmm12 = _mm_mul_ps(xmm3, xmm4);

    xmm2 = _mm_mul_ps(xmm2, xmm0);
    xmm3 = _mm_mul_ps(xmm3, xmm6);
    xmm4 = _mm_mul_ps(xmm4, xmm7);
    xmm5 = _mm_mul_ps(xmm5, xmm8);
    //xmm12 = _mm_mul_ps(xmm12, xmm11);

    xmm2 = _mm_add_ps(xmm2, xmm3);
    xmm3 = _mm_add_ps(xmm4, xmm5);

    src0 += 4;

    xmm9 = _mm_add_ps(xmm2, xmm9);

    xmm1 = _mm_add_ps(xmm3, xmm1);

    //xmm9 = _mm_add_ps(xmm12, xmm9);
  }

  xmm2 = _mm_hadd_ps(xmm9, xmm1);
  xmm3 = _mm_hadd_ps(xmm2, xmm2);
  xmm4 = _mm_hadd_ps(xmm3, xmm3);

  _mm_store_ss(&result, xmm4);



  for(i = 0; i < leftovers; ++i) {
    fst = src0[i];
    fst = MAX(fst, *cutoff);
    sq = fst * fst;
    thrd = fst * sq;
    frth = sq * sq;
    //fith = sq * thrd;

    result += (center_point_array[0] * fst +
	       center_point_array[1] * sq +
	       center_point_array[2] * thrd +
	       center_point_array[3] * frth);// +
	       //center_point_array[4] * fith);
  }

  result += ((float)((bound * 4) + leftovers)) * center_point_array[4]; //center_point_array[5];

  target[0] = result;
}


#endif /*LV_HAVE_SSE3*/


#ifdef LV_HAVE_AVX
#include<immintrin.h>

static inline void volk_32f_x3_sum_of_poly_32f_a_avx(float* target, float* src0, float* center_point_array, float* cutoff, unsigned int num_points)
{
  const unsigned int eighth_points = num_points / 8;
  float fst = 0.0;
  float sq = 0.0;
  float thrd = 0.0;
  float frth = 0.0;

  __m256 cpa0, cpa1, cpa2, cpa3, cutoff_vec;
  __m256 target_vec;
  __m256 x_to_1, x_to_2, x_to_3, x_to_4;

  cpa0 = _mm256_set1_ps(center_point_array[0]);
  cpa1 = _mm256_set1_ps(center_point_array[1]);
  cpa2 = _mm256_set1_ps(center_point_array[2]);
  cpa3 = _mm256_set1_ps(center_point_array[3]);
  cutoff_vec = _mm256_set1_ps(*cutoff);
  target_vec = _mm256_setzero_ps();

  unsigned int i;

  for(i = 0; i < eighth_points; ++i) {
    x_to_1 = _mm256_load_ps(src0);
    x_to_1 = _mm256_max_ps(x_to_1, cutoff_vec);
    x_to_2 = _mm256_mul_ps(x_to_1, x_to_1); // x^2
    x_to_3 = _mm256_mul_ps(x_to_1, x_to_2); // x^3
    // x^1 * x^3 is slightly faster than x^2 * x^2
    x_to_4 = _mm256_mul_ps(x_to_1, x_to_3); // x^4

    x_to_1 = _mm256_mul_ps(x_to_1, cpa0); // cpa[0] * x^1
    x_to_2 = _mm256_mul_ps(x_to_2, cpa1); // cpa[1] * x^2
    x_to_3 = _mm256_mul_ps(x_to_3, cpa2); // cpa[2] * x^3
    x_to_4 = _mm256_mul_ps(x_to_4, cpa3); // cpa[3] * x^4

    x_to_1 = _mm256_add_ps(x_to_1, x_to_2);
    x_to_3 = _mm256_add_ps(x_to_3, x_to_4);
    // this is slightly faster than result += (x_to_1 + x_to_3)
    target_vec = _mm256_add_ps(x_to_1, target_vec);
    target_vec = _mm256_add_ps(x_to_3, target_vec);

    src0 += 8;
  }

  // the hadd for vector reduction has very very slight impact @ 50k iters
  __VOLK_ATTR_ALIGNED(32) float temp_results[8];
  target_vec = _mm256_hadd_ps(target_vec, target_vec); // x0+x1 | x2+x3 | x0+x1 | x2+x3 || x4+x5 | x6+x7 | x4+x5 | x6+x7
  _mm256_store_ps(temp_results, target_vec);
  *target = temp_results[0] + temp_results[1] + temp_results[4] + temp_results[5];


  for(i = eighth_points*8; i < num_points; ++i) {
    fst = *(src0++);
    fst = MAX(fst, *cutoff);
    sq = fst * fst;
    thrd = fst * sq;
    frth = sq * sq;

    *target += (center_point_array[0] * fst +
	       center_point_array[1] * sq +
	       center_point_array[2] * thrd +
	       center_point_array[3] * frth);
  }

  *target += ((float)(num_points)) * center_point_array[4];

}
#endif // LV_HAVE_AVX


#ifdef LV_HAVE_GENERIC

static inline void volk_32f_x3_sum_of_poly_32f_generic(float* target, float* src0, float* center_point_array, float* cutoff, unsigned int num_points) {

  const unsigned int num_bytes = num_points*4;

  float result = 0.0;
  float fst = 0.0;
  float sq = 0.0;
  float thrd = 0.0;
  float frth = 0.0;
  //float fith = 0.0;



  unsigned int i = 0;

  for(; i < num_bytes >> 2; ++i) {
    fst = src0[i];
    fst = MAX(fst, *cutoff);

    sq = fst * fst;
    thrd = fst * sq;
    frth = sq * sq;
    //fith = sq * thrd;

    result += (center_point_array[0] * fst +
	       center_point_array[1] * sq +
	       center_point_array[2] * thrd +
	       center_point_array[3] * frth); //+
	       //center_point_array[4] * fith);
    /*printf("%f12...%d\n", (center_point_array[0] * fst +
		  center_point_array[1] * sq +
		  center_point_array[2] * thrd +
			 center_point_array[3] * frth) +
	   //center_point_array[4] * fith) +
	   (center_point_array[4]), i);
    */
  }

  result += ((float)(num_bytes >> 2)) * (center_point_array[4]);//(center_point_array[5]);



  *target = result;
}

#endif /*LV_HAVE_GENERIC*/


#ifdef LV_HAVE_AVX
#include<immintrin.h>

static inline void volk_32f_x3_sum_of_poly_32f_u_avx(float* target, float* src0, float* center_point_array, float* cutoff, unsigned int num_points)
{
  const unsigned int eighth_points = num_points / 8;
  float fst = 0.0;
  float sq = 0.0;
  float thrd = 0.0;
  float frth = 0.0;

  __m256 cpa0, cpa1, cpa2, cpa3, cutoff_vec;
  __m256 target_vec;
  __m256 x_to_1, x_to_2, x_to_3, x_to_4;

  cpa0 = _mm256_set1_ps(center_point_array[0]);
  cpa1 = _mm256_set1_ps(center_point_array[1]);
  cpa2 = _mm256_set1_ps(center_point_array[2]);
  cpa3 = _mm256_set1_ps(center_point_array[3]);
  cutoff_vec = _mm256_set1_ps(*cutoff);
  target_vec = _mm256_setzero_ps();

  unsigned int i;

  for(i = 0; i < eighth_points; ++i) {
    x_to_1 = _mm256_loadu_ps(src0);
    x_to_1 = _mm256_max_ps(x_to_1, cutoff_vec);
    x_to_2 = _mm256_mul_ps(x_to_1, x_to_1); // x^2
    x_to_3 = _mm256_mul_ps(x_to_1, x_to_2); // x^3
    // x^1 * x^3 is slightly faster than x^2 * x^2
    x_to_4 = _mm256_mul_ps(x_to_1, x_to_3); // x^4

    x_to_1 = _mm256_mul_ps(x_to_1, cpa0); // cpa[0] * x^1
    x_to_2 = _mm256_mul_ps(x_to_2, cpa1); // cpa[1] * x^2
    x_to_3 = _mm256_mul_ps(x_to_3, cpa2); // cpa[2] * x^3
    x_to_4 = _mm256_mul_ps(x_to_4, cpa3); // cpa[3] * x^4

    x_to_1 = _mm256_add_ps(x_to_1, x_to_2);
    x_to_3 = _mm256_add_ps(x_to_3, x_to_4);
    // this is slightly faster than result += (x_to_1 + x_to_3)
    target_vec = _mm256_add_ps(x_to_1, target_vec);
    target_vec = _mm256_add_ps(x_to_3, target_vec);

    src0 += 8;
  }

  // the hadd for vector reduction has very very slight impact @ 50k iters
  __VOLK_ATTR_ALIGNED(32) float temp_results[8];
  target_vec = _mm256_hadd_ps(target_vec, target_vec); // x0+x1 | x2+x3 | x0+x1 | x2+x3 || x4+x5 | x6+x7 | x4+x5 | x6+x7
  _mm256_store_ps(temp_results, target_vec);
  *target = temp_results[0] + temp_results[1] + temp_results[4] + temp_results[5];


  for(i = eighth_points*8; i < num_points; ++i) {
    fst = *(src0++);
    fst = MAX(fst, *cutoff);
    sq = fst * fst;
    thrd = fst * sq;
    frth = sq * sq;

    *target += (center_point_array[0] * fst +
	       center_point_array[1] * sq +
	       center_point_array[2] * thrd +
	       center_point_array[3] * frth);
  }

  *target += ((float)(num_points)) * center_point_array[4];

}
#endif // LV_HAVE_AVX

#ifdef LV_HAVE_NEON
#include <arm_neon.h>

static inline void volk_32f_x3_sum_of_poly_32f_a_neon(float* __restrict target, float* __restrict src0, float* __restrict center_point_array, float* __restrict cutoff, unsigned int num_points) {


  unsigned int i;
  float zero[4] = {0.0f, 0.0f, 0.0f, 0.0f };

  float32x2_t x_to_1, x_to_2, x_to_3, x_to_4;
  float32x2_t cutoff_vector;
  float32x2x2_t x_low, x_high;
  float32x4_t x_qvector, c_qvector, cpa_qvector;
  float accumulator;
  float res_accumulators[4];

  c_qvector = vld1q_f32( zero );
  // load the cutoff in to a vector
  cutoff_vector = vdup_n_f32( *cutoff );
  // ... center point array
  cpa_qvector = vld1q_f32( center_point_array );

  for(i=0; i < num_points; ++i) {
    // load x  (src0)
    x_to_1 = vdup_n_f32( *src0++ );

    // Get a vector of max(src0, cutoff)
    x_to_1 = vmax_f32(x_to_1,  cutoff_vector ); // x^1
    x_to_2 = vmul_f32(x_to_1, x_to_1); // x^2
    x_to_3 = vmul_f32(x_to_2, x_to_1); // x^3
    x_to_4 = vmul_f32(x_to_3, x_to_1); // x^4
    // zip up doubles to interleave
    x_low = vzip_f32(x_to_1, x_to_2); // [x^2 | x^1 || x^2 | x^1]
    x_high = vzip_f32(x_to_3, x_to_4); // [x^4 | x^3 || x^4 | x^3]
    // float32x4_t vcombine_f32(float32x2_t low, float32x2_t high); // VMOV d0,d0
    x_qvector = vcombine_f32(x_low.val[0], x_high.val[0]);
    // now we finally have [x^4 | x^3 | x^2 | x] !

    c_qvector = vmlaq_f32(c_qvector, x_qvector, cpa_qvector);

  }
  // there should be better vector reduction techniques
  vst1q_f32(res_accumulators, c_qvector );
  accumulator = res_accumulators[0] + res_accumulators[1] +
          res_accumulators[2] + res_accumulators[3];

  *target = accumulator + center_point_array[4] * (float)num_points;
}

#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_NEON

static inline void volk_32f_x3_sum_of_poly_32f_neonvert(float* __restrict target, float* __restrict src0, float* __restrict center_point_array, float* __restrict cutoff, unsigned int num_points) {


  unsigned int i;
  float zero[4] = {0.0f, 0.0f, 0.0f, 0.0f };

  float accumulator;


  float32x4_t accumulator1_vec, accumulator2_vec, accumulator3_vec, accumulator4_vec;
  accumulator1_vec = vld1q_f32(zero);
  accumulator2_vec = vld1q_f32(zero);
  accumulator3_vec = vld1q_f32(zero);
  accumulator4_vec = vld1q_f32(zero);
  float32x4_t x_to_1, x_to_2, x_to_3, x_to_4;
  float32x4_t cutoff_vector, cpa_0, cpa_1, cpa_2, cpa_3;

  // load the cutoff in to a vector
  cutoff_vector = vdupq_n_f32( *cutoff );
  // ... center point array
  cpa_0 = vdupq_n_f32(center_point_array[0]);
  cpa_1 = vdupq_n_f32(center_point_array[1]);
  cpa_2 = vdupq_n_f32(center_point_array[2]);
  cpa_3 = vdupq_n_f32(center_point_array[3]);


  // nathan is not sure why this is slower *and* wrong compared to neonvertfma
  for(i=0; i < num_points/4; ++i) {
    // load x
    x_to_1 = vld1q_f32( src0 );

    // Get a vector of max(src0, cutoff)
    x_to_1 = vmaxq_f32(x_to_1,  cutoff_vector ); // x^1
    x_to_2 = vmulq_f32(x_to_1, x_to_1); // x^2
    x_to_3 = vmulq_f32(x_to_2, x_to_1); // x^3
    x_to_4 = vmulq_f32(x_to_3, x_to_1); // x^4
    x_to_1 = vmulq_f32(x_to_1, cpa_0);
    x_to_2 = vmulq_f32(x_to_2, cpa_1);
    x_to_3 = vmulq_f32(x_to_3, cpa_2);
    x_to_4 = vmulq_f32(x_to_4, cpa_3);
    accumulator1_vec = vaddq_f32(accumulator1_vec, x_to_1);
    accumulator2_vec = vaddq_f32(accumulator2_vec, x_to_2);
    accumulator3_vec = vaddq_f32(accumulator3_vec, x_to_3);
    accumulator4_vec = vaddq_f32(accumulator4_vec, x_to_4);

    src0 += 4;
  }
  accumulator1_vec = vaddq_f32(accumulator1_vec, accumulator2_vec);
  accumulator3_vec = vaddq_f32(accumulator3_vec, accumulator4_vec);
  accumulator1_vec = vaddq_f32(accumulator1_vec, accumulator3_vec);

  __VOLK_ATTR_ALIGNED(32) float res_accumulators[4];
  vst1q_f32(res_accumulators, accumulator1_vec );
  accumulator = res_accumulators[0] + res_accumulators[1] +
          res_accumulators[2] + res_accumulators[3];

  float fst = 0.0;
  float sq = 0.0;
  float thrd = 0.0;
  float frth = 0.0;

  for(i = 4*num_points/4; i < num_points; ++i) {
    fst = src0[i];
    fst = MAX(fst, *cutoff);

    sq = fst * fst;
    thrd = fst * sq;
    frth = sq * sq;
    //fith = sq * thrd;

    accumulator += (center_point_array[0] * fst +
           center_point_array[1] * sq +
           center_point_array[2] * thrd +
           center_point_array[3] * frth); //+
  }

  *target = accumulator + center_point_array[4] * (float)num_points;
}

#endif /* LV_HAVE_NEON */

#endif /*INCLUDED_volk_32f_x3_sum_of_poly_32f_a_H*/

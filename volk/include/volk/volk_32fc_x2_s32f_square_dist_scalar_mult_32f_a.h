#ifndef INCLUDED_volk_32fc_x2_s32f_square_dist_scalar_mult_32f_a_H
#define INCLUDED_volk_32fc_x2_s32f_square_dist_scalar_mult_32f_a_H

#include<inttypes.h>
#include<stdio.h>
#include<volk/volk_complex.h>
#include <string.h>

#ifdef LV_HAVE_SSE3
#include<xmmintrin.h>
#include<pmmintrin.h>

static inline void volk_32fc_x2_s32f_square_dist_scalar_mult_32f_a_sse3(float* target, lv_32fc_t* src0, lv_32fc_t* points, float scalar, unsigned int num_bytes) {
  

  __m128 xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;

  lv_32fc_t diff;
  memset(&diff, 0x0, 2*sizeof(float));

  float sq_dist = 0.0;
  int bound = num_bytes >> 5;
  int leftovers0 = (num_bytes >> 4) & 1;
  int leftovers1 = (num_bytes >> 3) & 1;
  int i = 0;
  
  
  
  xmm1 = _mm_setzero_ps();
  xmm1 = _mm_loadl_pi(xmm1, (__m64*)src0);  
  xmm2 = _mm_load_ps((float*)&points[0]);
  xmm8 = _mm_load1_ps(&scalar);
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
    
    xmm4 = _mm_mul_ps(xmm4, xmm8);

    _mm_store_ps(target, xmm4);

    target += 4;

  }
  
  xmm4 = _mm_sub_ps(xmm1, xmm2);
  xmm5 = _mm_sub_ps(xmm1, xmm3);
  
  

  points += 4;
  xmm6 = _mm_mul_ps(xmm4, xmm4);
  xmm7 = _mm_mul_ps(xmm5, xmm5);
    
  xmm4 = _mm_hadd_ps(xmm6, xmm7);
  
  xmm4 = _mm_mul_ps(xmm4, xmm8);
   
  _mm_store_ps(target, xmm4);
  
  target += 4;
  

  for(i = 0; i < leftovers0; ++i) {
    
    xmm2 = _mm_load_ps((float*)&points[0]);
    
    xmm4 = _mm_sub_ps(xmm1, xmm2);
    
    points += 2;
    
    xmm6 = _mm_mul_ps(xmm4, xmm4);

    xmm4 = _mm_hadd_ps(xmm6, xmm6);

    xmm4 = _mm_mul_ps(xmm4, xmm8);
    
    _mm_storeh_pi((__m64*)target, xmm4);

    target += 2;
  }

  for(i = 0; i < leftovers1; ++i) {
    
    diff = src0[0] - points[0];

    sq_dist = scalar * (lv_creal(diff) * lv_creal(diff) + lv_cimag(diff) * lv_cimag(diff));

    target[0] = sq_dist;
  }
}

#endif /*LV_HAVE_SSE3*/

#ifdef LV_HAVE_GENERIC
static inline void volk_32fc_x2_s32f_square_dist_scalar_mult_32f_a_generic(float* target, lv_32fc_t* src0, lv_32fc_t* points, float scalar, unsigned int num_bytes) {
  lv_32fc_t diff;
  float sq_dist;
  unsigned int i = 0; 
  
  for(; i < num_bytes >> 3; ++i) {
    diff = src0[0] - points[i];

    sq_dist = scalar * (lv_creal(diff) * lv_creal(diff) + lv_cimag(diff) * lv_cimag(diff));
    
    target[i] = sq_dist;
  }
}

#endif /*LV_HAVE_GENERIC*/


#endif /*INCLUDED_volk_32fc_x2_s32f_square_dist_scalar_mult_32f_a_H*/

#ifndef INCLUDED_volk_32fc_index_max_16u_a_H
#define INCLUDED_volk_32fc_index_max_16u_a_H

#include <volk/volk_common.h>
#include<inttypes.h>
#include<stdio.h>
#include<volk/volk_complex.h>

#ifdef LV_HAVE_SSE3
#include<xmmintrin.h>
#include<pmmintrin.h>


static inline void volk_32fc_index_max_16u_a_sse3(unsigned int* target, lv_32fc_t* src0, unsigned int num_bytes) {
  
  
  
  union bit128 holderf;
  union bit128 holderi;
  float sq_dist = 0.0;



  
  union bit128 xmm5, xmm4;
  __m128 xmm1, xmm2, xmm3;
  __m128i xmm8, xmm11, xmm12, xmmfive, xmmfour, xmm9, holder0, holder1, xmm10;

  xmm5.int_vec = xmmfive = _mm_setzero_si128();
  xmm4.int_vec = xmmfour = _mm_setzero_si128();
  holderf.int_vec = holder0 = _mm_setzero_si128();
  holderi.int_vec = holder1 = _mm_setzero_si128();
 
  
  int bound = num_bytes >> 5;
  int leftovers0 = (num_bytes >> 4) & 1;
  int leftovers1 = (num_bytes >> 3) & 1;
  int i = 0;
  
  
  xmm8 = _mm_set_epi32(3, 2, 1, 0);//remember the crazy reverse order!
  xmm9 = xmm8 = _mm_setzero_si128();
  xmm10 = _mm_set_epi32(4, 4, 4, 4);
  xmm3 = _mm_setzero_ps();
;
  
  //printf("%f, %f, %f, %f\n", ((float*)&xmm10)[0], ((float*)&xmm10)[1], ((float*)&xmm10)[2], ((float*)&xmm10)[3]);
  
  for(; i < bound; ++i) {
  
    xmm1 = _mm_load_ps((float*)src0);
    xmm2 = _mm_load_ps((float*)&src0[2]);
    

    src0 += 4;
  
  
    xmm1 = _mm_mul_ps(xmm1, xmm1);
    xmm2 = _mm_mul_ps(xmm2, xmm2);
    
    
    xmm1 = _mm_hadd_ps(xmm1, xmm2);

    xmm3 = _mm_max_ps(xmm1, xmm3);
  
    xmm4.float_vec = _mm_cmplt_ps(xmm1, xmm3);
    xmm5.float_vec = _mm_cmpeq_ps(xmm1, xmm3);
    
        
    
    xmm11 = _mm_and_si128(xmm8, xmm5.int_vec);
    xmm12 = _mm_and_si128(xmm9, xmm4.int_vec);
    
    xmm9 = _mm_add_epi32(xmm11,  xmm12);

    xmm8 = _mm_add_epi32(xmm8, xmm10);    

    
    //printf("%f, %f, %f, %f\n", ((float*)&xmm3)[0], ((float*)&xmm3)[1], ((float*)&xmm3)[2], ((float*)&xmm3)[3]);
    //printf("%u, %u, %u, %u\n", ((uint32_t*)&xmm10)[0], ((uint32_t*)&xmm10)[1], ((uint32_t*)&xmm10)[2], ((uint32_t*)&xmm10)[3]);

  }
  
  
  for(i = 0; i < leftovers0; ++i) {


    xmm2 = _mm_load_ps((float*)src0);
    
    xmm1 = _mm_movelh_ps(bit128_p(&xmm8)->float_vec, bit128_p(&xmm8)->float_vec);
    xmm8 = bit128_p(&xmm1)->int_vec;

    xmm2 = _mm_mul_ps(xmm2, xmm2);

    src0 += 2;

    xmm1 = _mm_hadd_ps(xmm2, xmm2);

    xmm3 = _mm_max_ps(xmm1, xmm3);

    xmm10 = _mm_set_epi32(2, 2, 2, 2);//load1_ps((float*)&init[2]);
    
    
    xmm4.float_vec = _mm_cmplt_ps(xmm1, xmm3);
    xmm5.float_vec = _mm_cmpeq_ps(xmm1, xmm3);
    
        
    
    xmm11 = _mm_and_si128(xmm8, xmm5.int_vec);
    xmm12 = _mm_and_si128(xmm9, xmm4.int_vec);
    
    xmm9 = _mm_add_epi32(xmm11, xmm12);

    xmm8 = _mm_add_epi32(xmm8, xmm10);    
    //printf("egads%u, %u, %u, %u\n", ((uint32_t*)&xmm9)[0], ((uint32_t*)&xmm9)[1], ((uint32_t*)&xmm9)[2], ((uint32_t*)&xmm9)[3]);

  }
    
  
    

  for(i = 0; i < leftovers1; ++i) {
    //printf("%u, %u, %u, %u\n", ((uint32_t*)&xmm9)[0], ((uint32_t*)&xmm9)[1], ((uint32_t*)&xmm9)[2], ((uint32_t*)&xmm9)[3]);
    

    sq_dist = lv_creal(src0[0]) * lv_creal(src0[0]) + lv_cimag(src0[0]) * lv_cimag(src0[0]);
    
    xmm2 = _mm_load1_ps(&sq_dist);

    xmm1 = xmm3;
    
    xmm3 = _mm_max_ss(xmm3, xmm2);

    
        
    xmm4.float_vec = _mm_cmplt_ps(xmm1, xmm3);
    xmm5.float_vec = _mm_cmpeq_ps(xmm1, xmm3);
    
        
    xmm8 = _mm_shuffle_epi32(xmm8, 0x00);  
    
    xmm11 = _mm_and_si128(xmm8, xmm4.int_vec);
    xmm12 = _mm_and_si128(xmm9, xmm5.int_vec);
    

    xmm9 = _mm_add_epi32(xmm11, xmm12);

  }
  
  //printf("%f, %f, %f, %f\n", ((float*)&xmm3)[0], ((float*)&xmm3)[1], ((float*)&xmm3)[2], ((float*)&xmm3)[3]);

  //printf("%u, %u, %u, %u\n", ((uint32_t*)&xmm9)[0], ((uint32_t*)&xmm9)[1], ((uint32_t*)&xmm9)[2], ((uint32_t*)&xmm9)[3]);

  _mm_store_ps((float*)&(holderf.f), xmm3);
  _mm_store_si128(&(holderi.int_vec), xmm9);
  
  target[0] = holderi.i[0];
  sq_dist = holderf.f[0]; 
  target[0] = (holderf.f[1] > sq_dist) ? holderi.i[1] : target[0];
  sq_dist = (holderf.f[1] > sq_dist) ? holderf.f[1] : sq_dist;
  target[0] = (holderf.f[2] > sq_dist) ? holderi.i[2] : target[0];
  sq_dist = (holderf.f[2] > sq_dist) ? holderf.f[2] : sq_dist;
  target[0] = (holderf.f[3] > sq_dist) ? holderi.i[3] : target[0];
  sq_dist = (holderf.f[3] > sq_dist) ? holderf.f[3] : sq_dist;

 
  
  /*
  float placeholder = 0.0;
  uint32_t temp0, temp1; 
  unsigned int g0 = (((float*)&xmm3)[0] > ((float*)&xmm3)[1]);
  unsigned int l0 = g0 ^ 1;

  unsigned int g1 = (((float*)&xmm3)[1] > ((float*)&xmm3)[2]);
  unsigned int l1 = g1 ^ 1;
  
  temp0 = g0 * ((uint32_t*)&xmm9)[0] + l0 * ((uint32_t*)&xmm9)[1];
  temp1 = g0 * ((uint32_t*)&xmm9)[2] + l0 * ((uint32_t*)&xmm9)[3];
  sq_dist = g0 * ((float*)&xmm3)[0] + l0 * ((float*)&xmm3)[1]; 
  placeholder = g0 * ((float*)&xmm3)[2] + l0 * ((float*)&xmm3)[3];
  
  g0 = (sq_dist > placeholder);
  l0 = g0 ^ 1;
  target[0] = g0 * temp0 + l0 * temp1;
  */
  
}

#endif /*LV_HAVE_SSE3*/

#ifdef LV_HAVE_GENERIC
static inline void volk_32fc_index_max_16u_a_generic(unsigned int* target, lv_32fc_t* src0, unsigned int num_bytes) {
  float sq_dist = 0.0;
  float max = 0.0;
  unsigned int index = 0;
  
  unsigned int i = 0; 
  
  for(; i < num_bytes >> 3; ++i) {

    sq_dist = lv_creal(src0[i]) * lv_creal(src0[i]) + lv_cimag(src0[i]) * lv_cimag(src0[i]);
    
    index = sq_dist > max ? i : index;
    max = sq_dist > max ? sq_dist : max;
    
    
  }
  target[0] = index;
  
}

#endif /*LV_HAVE_GENERIC*/


#endif /*INCLUDED_volk_32fc_index_max_16u_a_H*/

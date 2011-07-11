#ifndef INCLUDED_volk_16i_branch_4_state_8_a_H
#define INCLUDED_volk_16i_branch_4_state_8_a_H


#include<inttypes.h>
#include<stdio.h>	




#ifdef LV_HAVE_SSSE3

#include<xmmintrin.h>
#include<emmintrin.h>
#include<tmmintrin.h>

static inline  void volk_16i_branch_4_state_8_a_ssse3(short* target,  short* src0, char** permuters, short* cntl2, short* cntl3, short* scalars) {
	
  
  __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11;

  __m128i *p_target, *p_src0, *p_cntl2, *p_cntl3, *p_scalars;

  
  
  p_target = (__m128i*)target;
  p_src0 = (__m128i*)src0;
  p_cntl2 = (__m128i*)cntl2;
  p_cntl3 = (__m128i*)cntl3;
  p_scalars = (__m128i*)scalars;
  
  int i = 0;
  
  int bound = 1;
  
  
  xmm0 = _mm_load_si128(p_scalars);
  
  xmm1 = _mm_shufflelo_epi16(xmm0, 0);
  xmm2 = _mm_shufflelo_epi16(xmm0, 0x55);
  xmm3 = _mm_shufflelo_epi16(xmm0, 0xaa);
  xmm4 = _mm_shufflelo_epi16(xmm0, 0xff);
  
  xmm1 = _mm_shuffle_epi32(xmm1, 0x00);
  xmm2 = _mm_shuffle_epi32(xmm2, 0x00);
  xmm3 = _mm_shuffle_epi32(xmm3, 0x00);
  xmm4 = _mm_shuffle_epi32(xmm4, 0x00);

  xmm0 = _mm_load_si128((__m128i*)permuters[0]);
  xmm6 = _mm_load_si128((__m128i*)permuters[1]);
  xmm8 = _mm_load_si128((__m128i*)permuters[2]);
  xmm10 = _mm_load_si128((__m128i*)permuters[3]);

  for(; i < bound; ++i) {
    
    xmm5 = _mm_load_si128(p_src0);
    
    
    
    
    
    
    


    xmm0 = _mm_shuffle_epi8(xmm5, xmm0);
    xmm6 = _mm_shuffle_epi8(xmm5, xmm6);
    xmm8 = _mm_shuffle_epi8(xmm5, xmm8);
    xmm10 = _mm_shuffle_epi8(xmm5, xmm10);
    
    p_src0 += 4;
   
    
    xmm5 = _mm_add_epi16(xmm1, xmm2);
    
    xmm6 = _mm_add_epi16(xmm2, xmm6);
    xmm8 = _mm_add_epi16(xmm1, xmm8);
   
     
    xmm7 = _mm_load_si128(p_cntl2);
    xmm9 = _mm_load_si128(p_cntl3);
    
    xmm0 = _mm_add_epi16(xmm5, xmm0);
    
    
    xmm7 = _mm_and_si128(xmm7, xmm3);
    xmm9 = _mm_and_si128(xmm9, xmm4);
    
    xmm5 = _mm_load_si128(&p_cntl2[1]);
    xmm11 = _mm_load_si128(&p_cntl3[1]);

    xmm7 = _mm_add_epi16(xmm7, xmm9);

    xmm5 = _mm_and_si128(xmm5, xmm3);
    xmm11 = _mm_and_si128(xmm11, xmm4);

    xmm0 = _mm_add_epi16(xmm0, xmm7);
   
   
 
    xmm7 = _mm_load_si128(&p_cntl2[2]);
    xmm9 = _mm_load_si128(&p_cntl3[2]);
    
    xmm5 = _mm_add_epi16(xmm5, xmm11);
    
    xmm7 = _mm_and_si128(xmm7, xmm3);
    xmm9 = _mm_and_si128(xmm9, xmm4);
    
    xmm6 = _mm_add_epi16(xmm6, xmm5);
   
    
    xmm5 = _mm_load_si128(&p_cntl2[3]);
    xmm11 = _mm_load_si128(&p_cntl3[3]);
    
    xmm7 = _mm_add_epi16(xmm7, xmm9);
    
    xmm5 = _mm_and_si128(xmm5, xmm3);
    xmm11 = _mm_and_si128(xmm11, xmm4);
    
    xmm8 = _mm_add_epi16(xmm8, xmm7);
    
    xmm5 = _mm_add_epi16(xmm5, xmm11);
    
    _mm_store_si128(p_target, xmm0);
    _mm_store_si128(&p_target[1], xmm6);

    xmm10 = _mm_add_epi16(xmm5, xmm10);
    
    _mm_store_si128(&p_target[2], xmm8);
    
    _mm_store_si128(&p_target[3], xmm10);
    
    p_target += 3;   
  }
}
	
	
#endif /*LV_HAVE_SSEs*/

#ifdef LV_HAVE_GENERIC
static inline  void volk_16i_branch_4_state_8_a_generic(short* target,  short* src0, char** permuters, short* cntl2, short* cntl3, short* scalars) {
	int i = 0;
	
	int bound = 4;
	
	for(; i < bound; ++i) {
	  target[i* 8] = src0[((char)permuters[i][0])/2] 
	    + ((i + 1)%2  * scalars[0])
	    + (((i >> 1)^1) * scalars[1])
	    + (cntl2[i * 8] & scalars[2])
	    + (cntl3[i * 8] & scalars[3]);
	  target[i* 8 + 1] = src0[((char)permuters[i][1 * 2])/2] 
	    + ((i + 1)%2  * scalars[0])
	    + (((i >> 1)^1) * scalars[1])
	    + (cntl2[i * 8 + 1] & scalars[2])
	    + (cntl3[i * 8 + 1] & scalars[3]);
	  target[i* 8 + 2] = src0[((char)permuters[i][2 * 2])/2] 
	    + ((i + 1)%2  * scalars[0])
	    + (((i >> 1)^1) * scalars[1])
	    + (cntl2[i * 8 + 2] & scalars[2])
	    + (cntl3[i * 8 + 2] & scalars[3]);
	  target[i* 8 + 3] = src0[((char)permuters[i][3 * 2])/2] 
	    + ((i + 1)%2  * scalars[0])
	    + (((i >> 1)^1) * scalars[1])
	    + (cntl2[i * 8 + 3] & scalars[2])
	    + (cntl3[i * 8 + 3] & scalars[3]);
	  target[i* 8 + 4] = src0[((char)permuters[i][4 * 2])/2] 
	    + ((i + 1)%2  * scalars[0])
	    + (((i >> 1)^1) * scalars[1])
	    + (cntl2[i * 8 + 4] & scalars[2])
	    + (cntl3[i * 8 + 4] & scalars[3]);
	  target[i* 8 + 5] = src0[((char)permuters[i][5 * 2])/2] 
	    + ((i + 1)%2  * scalars[0])
	    + (((i >> 1)^1) * scalars[1])
	    + (cntl2[i * 8 + 5] & scalars[2])
	    + (cntl3[i * 8 + 5] & scalars[3]);
	  target[i* 8 + 6] = src0[((char)permuters[i][6 * 2])/2] 
	    + ((i + 1)%2  * scalars[0])
	    + (((i >> 1)^1) * scalars[1])
	    + (cntl2[i * 8 + 6] & scalars[2])
	    + (cntl3[i * 8 + 6] & scalars[3]);
	  target[i* 8 + 7] = src0[((char)permuters[i][7 * 2])/2] 
	    + ((i + 1)%2  * scalars[0])
	    + (((i >> 1)^1) * scalars[1])
	    + (cntl2[i * 8 + 7] & scalars[2])
	    + (cntl3[i * 8 + 7] & scalars[3]);
	  
	}
}

#endif /*LV_HAVE_GENERIC*/


#endif /*INCLUDED_volk_16i_branch_4_state_8_a_H*/

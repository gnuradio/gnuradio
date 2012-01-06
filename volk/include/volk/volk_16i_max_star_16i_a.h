#ifndef INCLUDED_volk_16i_max_star_16i_a_H
#define INCLUDED_volk_16i_max_star_16i_a_H


#include<inttypes.h>
#include<stdio.h>	


#ifdef LV_HAVE_SSSE3

#include<xmmintrin.h>
#include<emmintrin.h>
#include<tmmintrin.h>

static inline  void volk_16i_max_star_16i_a_ssse3(short* target, short* src0, unsigned int num_bytes) {


  
  short candidate = src0[0];
  short cands[8];
  __m128i xmm0, xmm1, xmm3, xmm4, xmm5, xmm6;
  

  __m128i *p_src0;
  
  p_src0 = (__m128i*)src0;

  int bound = num_bytes >> 4;
  int leftovers = (num_bytes >> 1) & 7;
  
  int i = 0;
  
  
  xmm1 = _mm_setzero_si128();
  xmm0 = _mm_setzero_si128();
  //_mm_insert_epi16(xmm0, candidate, 0);
  
  xmm0 = _mm_shuffle_epi8(xmm0, xmm1); 

  
  for(i = 0; i < bound; ++i) {
    xmm1 = _mm_load_si128(p_src0);
    p_src0 += 1;
    //xmm2 = _mm_sub_epi16(xmm1, xmm0);
  

    
  
  
  
    xmm3 = _mm_cmpgt_epi16(xmm0, xmm1);
    xmm4 = _mm_cmpeq_epi16(xmm0, xmm1);
    xmm5 = _mm_cmpgt_epi16(xmm1, xmm0);

    xmm6 = _mm_xor_si128(xmm4, xmm5);
    
    xmm3 = _mm_and_si128(xmm3, xmm0);
    xmm4 = _mm_and_si128(xmm6, xmm1);
    
    xmm0 = _mm_add_epi16(xmm3, xmm4);
    
  
  }
  
  _mm_store_si128((__m128i*)cands, xmm0);
  
  for(i = 0; i < 8; ++i) {
    candidate = ((short)(candidate - cands[i]) > 0) ? candidate : cands[i];
  }
  
 
  
  for(i = 0; i < leftovers; ++i) {
  
    candidate = ((short)(candidate - src0[(bound << 3) + i]) > 0) ? candidate : src0[(bound << 3) + i];
  }

  target[0] = candidate;
  
    
    
 

}   
 
#endif /*LV_HAVE_SSSE3*/

#ifdef LV_HAVE_GENERIC

static inline void volk_16i_max_star_16i_a_generic(short* target, short* src0, unsigned int num_bytes) {
	
	int i = 0;
	
	int bound = num_bytes >> 1;

	short candidate = src0[0];
	for(i = 1; i < bound; ++i) {
	  candidate = ((short)(candidate - src0[i]) > 0) ? candidate : src0[i];
	}
	target[0] = candidate;
	  
}


#endif /*LV_HAVE_GENERIC*/


#endif /*INCLUDED_volk_16i_max_star_16i_a_H*/

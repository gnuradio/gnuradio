#ifndef INCLUDED_volk_16i_max_star_horizontal_16i_a_H
#define INCLUDED_volk_16i_max_star_horizontal_16i_a_H

#include <volk/volk_common.h>

#include<inttypes.h>
#include<stdio.h>	


#ifdef LV_HAVE_SSSE3

#include<xmmintrin.h>
#include<emmintrin.h>
#include<tmmintrin.h>

static inline  void volk_16i_max_star_horizontal_16i_a_ssse3(int16_t* target, int16_t* src0, unsigned int num_bytes) {

  const static uint8_t shufmask0[16] = {0x00, 0x01, 0x04, 0x05, 0x08, 0x09, 0x0c, 0x0d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  const static uint8_t shufmask1[16] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0x04, 0x05, 0x08, 0x09, 0x0c, 0x0d};
  const static uint8_t andmask0[16] = {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const static uint8_t andmask1[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02};

  
  
  __m128i xmm0, xmm1, xmm2, xmm3, xmm4;
  __m128i  xmm5, xmm6, xmm7, xmm8;
  
  xmm4 = _mm_load_si128((__m128i*)shufmask0);
  xmm5 = _mm_load_si128((__m128i*)shufmask1);
  xmm6 = _mm_load_si128((__m128i*)andmask0);
  xmm7 = _mm_load_si128((__m128i*)andmask1);
  
  __m128i *p_target, *p_src0;
  
  p_target = (__m128i*)target;
  p_src0 = (__m128i*)src0;

  int bound = num_bytes >> 5;
  int intermediate = (num_bytes >> 4) & 1;
  int leftovers = (num_bytes >> 1) & 7;
  
  int i = 0;
  
  
  for(i = 0; i < bound; ++i) {
     
    xmm0 = _mm_load_si128(p_src0);
    xmm1 = _mm_load_si128(&p_src0[1]);
    
    

    xmm2 = _mm_xor_si128(xmm2, xmm2);
    p_src0 += 2;
    
    xmm3 = _mm_hsub_epi16(xmm0, xmm1);
    
    xmm2 = _mm_cmpgt_epi16(xmm2, xmm3);   

    xmm8 = _mm_and_si128(xmm2, xmm6);
    xmm3 = _mm_and_si128(xmm2, xmm7);
    

    xmm8 = _mm_add_epi8(xmm8, xmm4);
    xmm3 = _mm_add_epi8(xmm3, xmm5);

    xmm0 = _mm_shuffle_epi8(xmm0, xmm8);
    xmm1 = _mm_shuffle_epi8(xmm1, xmm3);
    
    
    xmm3 = _mm_add_epi16(xmm0, xmm1);

    
    _mm_store_si128(p_target, xmm3);
    
    p_target += 1;
  
  }

  for(i = 0; i < intermediate; ++i) {
    
    xmm0 = _mm_load_si128(p_src0);
    
    
    xmm2 = _mm_xor_si128(xmm2, xmm2);
    p_src0 += 1;
    
    xmm3 = _mm_hsub_epi16(xmm0, xmm1);
    xmm2 = _mm_cmpgt_epi16(xmm2, xmm3);

    xmm8 = _mm_and_si128(xmm2, xmm6);
    
    xmm3 = _mm_add_epi8(xmm8, xmm4);
    
    xmm0 = _mm_shuffle_epi8(xmm0, xmm3);
    
    _mm_storel_pd((double*)p_target, bit128_p(&xmm0)->double_vec);
    
    p_target = (__m128i*)((int8_t*)p_target + 8);

  }
    
  for(i = (bound << 4) + (intermediate << 3); i < (bound << 4) + (intermediate << 3) + leftovers ; i += 2) { 
    target[i>>1] = ((int16_t)(src0[i] - src0[i + 1]) > 0) ? src0[i] : src0[i + 1];
  }
  

}   
 
#endif /*LV_HAVE_SSSE3*/


#ifdef LV_HAVE_GENERIC
static inline void volk_16i_max_star_horizontal_16i_a_generic(int16_t* target, int16_t* src0, unsigned int num_bytes) {
	
	int i = 0;
	
	int bound = num_bytes >> 1;

      
	for(i = 0; i < bound; i += 2) {
	  target[i >> 1] = ((int16_t) (src0[i] - src0[i + 1]) > 0) ? src0[i] : src0[i+1];
	}
		
}



#endif /*LV_HAVE_GENERIC*/

#endif /*INCLUDED_volk_16i_max_star_horizontal_16i_a_H*/

#ifndef INCLUDED_volk_16i_x5_add_quad_16i_x4_a_H
#define INCLUDED_volk_16i_x5_add_quad_16i_x4_a_H


#include<inttypes.h>
#include<stdio.h>	





#ifdef LV_HAVE_SSE2
#include<xmmintrin.h>
#include<emmintrin.h>

static inline  void volk_16i_x5_add_quad_16i_x4_a_sse2(short* target0, short* target1, short* target2, short* target3, short* src0, short* src1, short* src2, short* src3, short* src4, unsigned int num_bytes) {
  
  __m128i xmm0, xmm1, xmm2, xmm3, xmm4;
  __m128i *p_target0, *p_target1, *p_target2, *p_target3,  *p_src0, *p_src1, *p_src2, *p_src3, *p_src4;
  p_target0 = (__m128i*)target0;
  p_target1 = (__m128i*)target1;
  p_target2 = (__m128i*)target2;
  p_target3 = (__m128i*)target3;

  p_src0 = (__m128i*)src0;
  p_src1 = (__m128i*)src1;
  p_src2 = (__m128i*)src2;
  p_src3 = (__m128i*)src3;
  p_src4 = (__m128i*)src4;

  int i = 0;

  int bound = (num_bytes >> 4);
  int leftovers = (num_bytes >> 1) & 7;

  for(; i < bound; ++i) {
    xmm0 = _mm_load_si128(p_src0);
    xmm1 = _mm_load_si128(p_src1);
    xmm2 = _mm_load_si128(p_src2);
    xmm3 = _mm_load_si128(p_src3);
    xmm4 = _mm_load_si128(p_src4);
    
    p_src0 += 1;
    p_src1 += 1;
    
    xmm1 = _mm_add_epi16(xmm0, xmm1);
    xmm2 = _mm_add_epi16(xmm0, xmm2);
    xmm3 = _mm_add_epi16(xmm0, xmm3);
    xmm4 = _mm_add_epi16(xmm0, xmm4);
    
    
    p_src2 += 1;
    p_src3 += 1;
    p_src4 += 1;

    _mm_store_si128(p_target0, xmm1);
    _mm_store_si128(p_target1, xmm2);
    _mm_store_si128(p_target2, xmm3);
    _mm_store_si128(p_target3, xmm4);
    
    p_target0 += 1;
    p_target1 += 1;
    p_target2 += 1;
    p_target3 += 1;
  }
    /*asm volatile
		(
		 ".%=volk_16i_x5_add_quad_16i_x4_a_sse2_L1:\n\t"
		 "cmp $0, %[bound]\n\t"
		 "je .%=volk_16i_x5_add_quad_16i_x4_a_sse2_END\n\t"
		 "movaps (%[src0]), %%xmm1\n\t"
		 "movaps (%[src1]), %%xmm2\n\t"
		 "movaps (%[src2]), %%xmm3\n\t"
		 "movaps (%[src3]), %%xmm4\n\t"
		 "movaps (%[src4]), %%xmm5\n\t"
		 "add $16, %[src0]\n\t"
		 "add $16, %[src1]\n\t"
		 "add $16, %[src2]\n\t"
		 "add $16, %[src3]\n\t"
		 "add $16, %[src4]\n\t"
		 "paddw %%xmm1, %%xmm2\n\t"
		 "paddw %%xmm1, %%xmm3\n\t"
		 "paddw %%xmm1, %%xmm4\n\t"
		 "paddw %%xmm1, %%xmm5\n\t"
		 "add $-1, %[bound]\n\t"
		 "movaps %%xmm2, (%[target0])\n\t"
		 "movaps %%xmm3, (%[target1])\n\t"
		 "movaps %%xmm4, (%[target2])\n\t"
		 "movaps %%xmm5, (%[target3])\n\t"
		 "add $16, %[target0]\n\t"
		 "add $16, %[target1]\n\t"
		 "add $16, %[target2]\n\t"
		 "add $16, %[target3]\n\t"
		 "jmp .%=volk_16i_x5_add_quad_16i_x4_a_sse2_L1\n\t"
		 ".%=volk_16i_x5_add_quad_16i_x4_a_sse2_END:\n\t"
		 :
		 :[bound]"r"(bound), [src0]"r"(src0), [src1]"r"(src1), [src2]"r"(src2), [src3]"r"(src3), [src4]"r"(src4), [target0]"r"(target0), [target1]"r"(target1), [target2]"r"(target2), [target3]"r"(target3)
		 :"xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
		 );
		 
    */
	 

  for(i = bound * 8; i < (bound * 8) + leftovers; ++i) {
    target0[i] = src0[i] + src1[i];
    target1[i] = src0[i] + src2[i];
    target2[i] = src0[i] + src3[i];
    target3[i] = src0[i] + src4[i];
  }
}
#endif /*LV_HAVE_SSE2*/


#ifdef LV_HAVE_GENERIC

static inline void volk_16i_x5_add_quad_16i_x4_a_generic(short* target0, short* target1, short* target2, short* target3, short* src0, short* src1, short* src2, short* src3, short* src4, unsigned int num_bytes) {
	
	int i = 0;
	
	int bound = num_bytes >> 1;

	for(i = 0; i < bound; ++i) {
		target0[i] = src0[i] + src1[i];
		target1[i] = src0[i] + src2[i];
		target2[i] = src0[i] + src3[i];
		target3[i] = src0[i] + src4[i];
	}
}

#endif /* LV_HAVE_GENERIC */





#endif /*INCLUDED_volk_16i_x5_add_quad_16i_x4_a_H*/

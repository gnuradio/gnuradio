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

#ifndef INCLUDED_volk_16i_x4_quad_max_star_16i_a_H
#define INCLUDED_volk_16i_x4_quad_max_star_16i_a_H


#include<inttypes.h>
#include<stdio.h>





#ifdef LV_HAVE_SSE2

#include<emmintrin.h>

static inline  void volk_16i_x4_quad_max_star_16i_a_sse2(short* target, short* src0, short* src1, short* src2, short* src3, unsigned int num_points) {

	const unsigned int num_bytes = num_points*2;

	int i = 0;

	int bound = (num_bytes >> 4);
	int bound_copy = bound;
	int leftovers = (num_bytes >> 1) & 7;

	__m128i *p_target, *p_src0, *p_src1, *p_src2, *p_src3;
	p_target = (__m128i*) target;
	p_src0 =  (__m128i*)src0;
	p_src1 =  (__m128i*)src1;
	p_src2 =  (__m128i*)src2;
	p_src3 =  (__m128i*)src3;



	__m128i xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;

	while(bound_copy > 0) {

	  xmm1 = _mm_load_si128(p_src0);
	  xmm2 = _mm_load_si128(p_src1);
	  xmm3 = _mm_load_si128(p_src2);
	  xmm4 = _mm_load_si128(p_src3);

	  xmm5 = _mm_setzero_si128();
	  xmm6 = _mm_setzero_si128();
	  xmm7 = xmm1;
	  xmm8 = xmm3;


	  xmm1 = _mm_sub_epi16(xmm2, xmm1);



	  xmm3 = _mm_sub_epi16(xmm4, xmm3);

	  xmm5 = _mm_cmpgt_epi16(xmm1, xmm5);
	  xmm6 = _mm_cmpgt_epi16(xmm3, xmm6);



	  xmm2 = _mm_and_si128(xmm5, xmm2);
	  xmm4 = _mm_and_si128(xmm6, xmm4);
	  xmm5 = _mm_andnot_si128(xmm5, xmm7);
	  xmm6 = _mm_andnot_si128(xmm6, xmm8);

	  xmm5 = _mm_add_epi16(xmm2, xmm5);
	  xmm6 = _mm_add_epi16(xmm4, xmm6);


	  xmm1 = _mm_xor_si128(xmm1, xmm1);
	  xmm2 = xmm5;
	  xmm5 = _mm_sub_epi16(xmm6, xmm5);
	  p_src0 += 1;
	  bound_copy -= 1;

	  xmm1 = _mm_cmpgt_epi16(xmm5, xmm1);
	  p_src1 += 1;

	  xmm6 = _mm_and_si128(xmm1, xmm6);

	  xmm1 = _mm_andnot_si128(xmm1, xmm2);
	  p_src2 += 1;



	  xmm1 = _mm_add_epi16(xmm6, xmm1);
	  p_src3 += 1;


	  _mm_store_si128(p_target, xmm1);
	  p_target += 1;

	}


	/*asm volatile
		(
		 "volk_16i_x4_quad_max_star_16i_a_sse2_L1:\n\t"
		 "cmp $0, %[bound]\n\t"
		 "je volk_16i_x4_quad_max_star_16i_a_sse2_END\n\t"

		 "movaps (%[src0]), %%xmm1\n\t"
		 "movaps (%[src1]), %%xmm2\n\t"
		 "movaps (%[src2]), %%xmm3\n\t"
		 "movaps (%[src3]), %%xmm4\n\t"

		 "pxor %%xmm5, %%xmm5\n\t"
		 "pxor %%xmm6, %%xmm6\n\t"
		 "movaps %%xmm1, %%xmm7\n\t"
		 "movaps %%xmm3, %%xmm8\n\t"
		 "psubw %%xmm2, %%xmm1\n\t"
		 "psubw %%xmm4, %%xmm3\n\t"

		 "pcmpgtw %%xmm1, %%xmm5\n\t"
		 "pcmpgtw %%xmm3, %%xmm6\n\t"

		 "pand %%xmm5, %%xmm2\n\t"
		 "pand %%xmm6, %%xmm4\n\t"
		 "pandn %%xmm7, %%xmm5\n\t"
		 "pandn %%xmm8, %%xmm6\n\t"

		 "paddw %%xmm2, %%xmm5\n\t"
		 "paddw %%xmm4, %%xmm6\n\t"

		 "pxor %%xmm1, %%xmm1\n\t"
		 "movaps %%xmm5, %%xmm2\n\t"

		 "psubw %%xmm6, %%xmm5\n\t"
		 "add $16, %[src0]\n\t"
		 "add $-1, %[bound]\n\t"

		 "pcmpgtw %%xmm5, %%xmm1\n\t"
		 "add $16, %[src1]\n\t"

		 "pand %%xmm1, %%xmm6\n\t"

		 "pandn %%xmm2, %%xmm1\n\t"
		 "add $16, %[src2]\n\t"

		 "paddw %%xmm6, %%xmm1\n\t"
		 "add $16, %[src3]\n\t"

		 "movaps %%xmm1, (%[target])\n\t"
		 "addw $16, %[target]\n\t"
		 "jmp volk_16i_x4_quad_max_star_16i_a_sse2_L1\n\t"

		 "volk_16i_x4_quad_max_star_16i_a_sse2_END:\n\t"
		 :
		 :[bound]"r"(bound), [src0]"r"(src0), [src1]"r"(src1), [src2]"r"(src2), [src3]"r"(src3), [target]"r"(target)
		 :
		 );
	*/

	short temp0 = 0;
	short temp1 = 0;
	for(i = bound * 8; i < (bound * 8) + leftovers; ++i) {
	  temp0 = ((short)(src0[i] - src1[i]) > 0) ? src0[i] : src1[i];
	  temp1 = ((short)(src2[i] - src3[i])>0) ? src2[i] : src3[i];
	  target[i] = ((short)(temp0 - temp1)>0) ? temp0 : temp1;
	}
	return;


}

#endif /*LV_HAVE_SSE2*/

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
static inline void volk_16i_x4_quad_max_star_16i_neon(short* target, short* src0, short* src1, short* src2, short* src3, unsigned int num_points) {
    const unsigned int eighth_points = num_points / 8;
    unsigned i;

    int16x8_t src0_vec, src1_vec, src2_vec, src3_vec;
    int16x8_t diff12, diff34;
    int16x8_t comp0, comp1, comp2, comp3;
    int16x8_t result1_vec, result2_vec;
    int16x8_t zeros;
    zeros = veorq_s16(zeros, zeros);
    for(i=0; i < eighth_points; ++i) {
        src0_vec = vld1q_s16(src0);
        src1_vec = vld1q_s16(src1);
        src2_vec = vld1q_s16(src2);
        src3_vec = vld1q_s16(src3);
        diff12 = vsubq_s16(src0_vec, src1_vec);
        diff34  = vsubq_s16(src2_vec, src3_vec);
        comp0 = (int16x8_t)vcgeq_s16(diff12, zeros);
        comp1 = (int16x8_t)vcltq_s16(diff12, zeros);
        comp2 = (int16x8_t)vcgeq_s16(diff34, zeros);
        comp3 = (int16x8_t)vcltq_s16(diff34, zeros);
        comp0 = vandq_s16(src0_vec, comp0);
        comp1 = vandq_s16(src1_vec, comp1);
        comp2 = vandq_s16(src2_vec, comp2);
        comp3 = vandq_s16(src3_vec, comp3);

        result1_vec = vaddq_s16(comp0, comp1);
        result2_vec = vaddq_s16(comp2, comp3);

        diff12 = vsubq_s16(result1_vec, result2_vec);
        comp0 = (int16x8_t)vcgeq_s16(diff12, zeros);
        comp1 = (int16x8_t)vcltq_s16(diff12, zeros);
        comp0 = vandq_s16(result1_vec, comp0);
        comp1 = vandq_s16(result2_vec, comp1);
        result1_vec = vaddq_s16(comp0, comp1);
        vst1q_s16(target, result1_vec);
    src0 += 8;
    src1 += 8;
    src2 += 8;
    src3 += 8;
    target += 8;
    }


    short temp0 = 0;
    short temp1 = 0;
    for(i=eighth_points*8; i < num_points; ++i) {
      temp0 = ((short)(*src0 - *src1) > 0) ? *src0 : *src1;
      temp1 = ((short)(*src2 - *src3) > 0) ? *src2 : *src3;
      *target++ = ((short)(temp0 - temp1)>0) ? temp0 : temp1;
    src0++;
    src1++;
    src2++;
    src3++;
    }
}
#endif /* LV_HAVE_NEON */


#ifdef LV_HAVE_GENERIC
static inline void volk_16i_x4_quad_max_star_16i_generic(short* target, short* src0, short* src1, short* src2, short* src3, unsigned int num_points) {

	const unsigned int num_bytes = num_points*2;

	int i = 0;

	int bound = num_bytes >> 1;

	short temp0 = 0;
	short temp1 = 0;
	for(i = 0; i < bound; ++i) {
	  temp0 = ((short)(src0[i] - src1[i]) > 0) ? src0[i] : src1[i];
	  temp1 = ((short)(src2[i] - src3[i])>0) ? src2[i] : src3[i];
	  target[i] = ((short)(temp0 - temp1)>0) ? temp0 : temp1;
	}
}




#endif /*LV_HAVE_GENERIC*/

#endif /*INCLUDED_volk_16i_x4_quad_max_star_16i_a_H*/

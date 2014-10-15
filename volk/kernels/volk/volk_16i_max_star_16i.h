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

#ifndef INCLUDED_volk_16i_max_star_16i_a_H
#define INCLUDED_volk_16i_max_star_16i_a_H


#include<inttypes.h>
#include<stdio.h>


#ifdef LV_HAVE_SSSE3

#include<xmmintrin.h>
#include<emmintrin.h>
#include<tmmintrin.h>

static inline  void volk_16i_max_star_16i_a_ssse3(short* target, short* src0, unsigned int num_points) {

  const unsigned int num_bytes = num_points*2;

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

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
static inline void volk_16i_max_star_16i_neon(short* target, short* src0, unsigned int num_points) {
    const unsigned int eighth_points = num_points / 8;
    unsigned number;
    int16x8_t input_vec;
    int16x8_t diff, zeros;
    uint16x8_t comp1, comp2;
    zeros = veorq_s16(zeros, zeros);

    int16x8x2_t tmpvec;

    int16x8_t candidate_vec = vld1q_dup_s16(src0 );
    short candidate;
    ++src0;

    for(number=0; number < eighth_points; ++number) {
        input_vec = vld1q_s16(src0);
        __builtin_prefetch(src0+16);
        diff = vsubq_s16(candidate_vec, input_vec);
        comp1 = vcgeq_s16(diff, zeros);
        comp2 = vcltq_s16(diff, zeros);

        tmpvec.val[0] = vandq_s16(candidate_vec, (int16x8_t)comp1);
        tmpvec.val[1] = vandq_s16(input_vec, (int16x8_t)comp2);

        candidate_vec = vaddq_s16(tmpvec.val[0], tmpvec.val[1]);
        src0 += 8;
    }
        vst1q_s16(&candidate, candidate_vec);

    for(number=0; number < num_points%8; number++) {
        candidate = ((int16_t)(candidate - src0[number]) > 0) ? candidate : src0[number];
    }
    target[0] = candidate;
}
#endif /*LV_HAVE_NEON*/

#ifdef LV_HAVE_GENERIC

static inline void volk_16i_max_star_16i_generic(short* target, short* src0, unsigned int num_points) {

	const unsigned int num_bytes = num_points*2;

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

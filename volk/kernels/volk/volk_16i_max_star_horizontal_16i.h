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

#ifndef INCLUDED_volk_16i_max_star_horizontal_16i_a_H
#define INCLUDED_volk_16i_max_star_horizontal_16i_a_H

#include <volk/volk_common.h>

#include<inttypes.h>
#include<stdio.h>


#ifdef LV_HAVE_SSSE3

#include<xmmintrin.h>
#include<emmintrin.h>
#include<tmmintrin.h>

static inline  void volk_16i_max_star_horizontal_16i_a_ssse3(int16_t* target, int16_t* src0, unsigned int num_points) {

  const unsigned int num_bytes = num_points*2;

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

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
static inline void volk_16i_max_star_horizontal_16i_neon(int16_t* target, int16_t* src0, unsigned int num_points) {
    const unsigned int eighth_points = num_points / 16;
    unsigned number;
    int16x8x2_t input_vec;
    int16x8_t diff, max_vec, zeros;
    uint16x8_t comp1, comp2;
    zeros = veorq_s16(zeros, zeros);
    for(number=0; number < eighth_points; ++number) {
        input_vec = vld2q_s16(src0);
        //__builtin_prefetch(src0+16);
        diff = vsubq_s16(input_vec.val[0], input_vec.val[1]);
        comp1 = vcgeq_s16(diff, zeros);
        comp2 = vcltq_s16(diff, zeros);

        input_vec.val[0] = vandq_s16(input_vec.val[0], (int16x8_t)comp1);
        input_vec.val[1] = vandq_s16(input_vec.val[1], (int16x8_t)comp2);

        max_vec = vaddq_s16(input_vec.val[0], input_vec.val[1]);
        vst1q_s16(target, max_vec);
        src0 += 16;
        target += 8;
    }
    for(number=0; number < num_points%16; number+=2) {
        target[number >> 1] = ((int16_t)(src0[number] - src0[number + 1]) > 0) ? src0[number] : src0[number+1];
    }

}
#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_NEON
extern void volk_16i_max_star_horizontal_16i_neonasm(int16_t* target, int16_t* src0, unsigned int num_points);
#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_GENERIC
static inline void volk_16i_max_star_horizontal_16i_generic(int16_t* target, int16_t* src0, unsigned int num_points) {

	const unsigned int num_bytes = num_points*2;

	int i = 0;

	int bound = num_bytes >> 1;


	for(i = 0; i < bound; i += 2) {
	  target[i >> 1] = ((int16_t) (src0[i] - src0[i + 1]) > 0) ? src0[i] : src0[i+1];
	}

}



#endif /*LV_HAVE_GENERIC*/

#endif /*INCLUDED_volk_16i_max_star_horizontal_16i_a_H*/

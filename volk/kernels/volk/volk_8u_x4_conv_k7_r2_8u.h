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

#ifndef INCLUDED_volk_8u_x4_conv_k7_r2_8u_H
#define INCLUDED_volk_8u_x4_conv_k7_r2_8u_H



typedef union {
  unsigned char/*DECISIONTYPE*/ t[64/*NUMSTATES*//8/*DECISIONTYPE_BITSIZE*/];
  unsigned int w[64/*NUMSTATES*//32];
  unsigned short s[64/*NUMSTATES*//16];
  unsigned char c[64/*NUMSTATES*//8];
#ifdef _MSC_VER
} decision_t;
#else
} decision_t __attribute__ ((aligned (16)));
#endif

static inline void renormalize(unsigned char* X, unsigned char threshold){
  int NUMSTATES = 64;
  int i;

    unsigned char min=X[0];
    //if(min > threshold) {
    for(i=0;i<NUMSTATES;i++)
    if (min>X[i])
      min=X[i];
    for(i=0;i<NUMSTATES;i++)
      X[i]-=min;
    //}
}



//helper BFLY for GENERIC version
static inline void BFLY(int i, int s, unsigned char * syms, unsigned char *Y, unsigned char *X, decision_t * d, unsigned char* Branchtab) {
  int j, decision0, decision1;
  unsigned char metric,m0,m1,m2,m3;

  int NUMSTATES = 64;
  int RATE = 2;
  int METRICSHIFT = 1;
  int PRECISIONSHIFT = 2;



  metric =0;
  for(j=0;j<RATE;j++)
    metric += (Branchtab[i+j*NUMSTATES/2] ^ syms[s*RATE+j])>>METRICSHIFT ;
  metric=metric>>PRECISIONSHIFT;

  unsigned char max = ((RATE*((256 -1)>>METRICSHIFT))>>PRECISIONSHIFT);

  m0 = X[i] + metric;
  m1 = X[i+NUMSTATES/2] + (max - metric);
  m2 = X[i] + (max - metric);
  m3 = X[i+NUMSTATES/2] + metric;

  decision0 = (signed int)(m0-m1) > 0;
  decision1 = (signed int)(m2-m3) > 0;

  Y[2*i] = decision0 ? m1 : m0;
  Y[2*i+1] =  decision1 ? m3 : m2;

  d->w[i/(sizeof(unsigned int)*8/2)+s*(sizeof(decision_t)/sizeof(unsigned int))] |=
    (decision0|decision1<<1) << ((2*i)&(sizeof(unsigned int)*8-1));
}


#if LV_HAVE_SSE3

#include <pmmintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <mmintrin.h>
#include <stdio.h>

static inline void volk_8u_x4_conv_k7_r2_8u_spiral(unsigned char* Y, unsigned char* X, unsigned char* syms, unsigned char* dec, unsigned int framebits, unsigned int excess, unsigned char* Branchtab) {
  unsigned int i9;
  for(i9 = 0; i9 < ((framebits + excess) >> 1); i9++) {
    unsigned char a75, a81;
    int a73, a92;
    short int s20, s21, s26, s27;
    unsigned char  *a74, *a80, *b6;
    short int  *a110, *a111, *a91, *a93, *a94;
    __m128i  *a102, *a112, *a113, *a71, *a72, *a77, *a83
      , *a95, *a96, *a97, *a98, *a99;
    __m128i a105, a106, a86, a87;
    __m128i a100, a101, a103, a104, a107, a108, a109
      , a76, a78, a79, a82, a84, a85, a88, a89
      , a90, d10, d11, d12, d9, m23, m24, m25
      , m26, m27, m28, m29, m30, s18, s19, s22
      , s23, s24, s25, s28, s29, t13, t14, t15
      , t16, t17, t18;
    a71 = ((__m128i  *) X);
    s18 = *(a71);
    a72 = (a71 + 2);
    s19 = *(a72);
    a73 = (4 * i9);
    a74 = (syms + a73);
    a75 = *(a74);
    a76 = _mm_set1_epi8(a75);
    a77 = ((__m128i  *) Branchtab);
    a78 = *(a77);
    a79 = _mm_xor_si128(a76, a78);
    b6 = (a73 + syms);
    a80 = (b6 + 1);
    a81 = *(a80);
    a82 = _mm_set1_epi8(a81);
    a83 = (a77 + 2);
    a84 = *(a83);
    a85 = _mm_xor_si128(a82, a84);
    t13 = _mm_avg_epu8(a79,a85);
    a86 = ((__m128i ) t13);
    a87 = _mm_srli_epi16(a86, 2);
    a88 = ((__m128i ) a87);
    t14 = _mm_and_si128(a88, _mm_set_epi8(63, 63, 63, 63, 63, 63, 63
					  , 63, 63, 63, 63, 63, 63, 63, 63
					  , 63));
    t15 = _mm_subs_epu8(_mm_set_epi8(63, 63, 63, 63, 63, 63, 63
				     , 63, 63, 63, 63, 63, 63, 63, 63
				     , 63), t14);
    m23 = _mm_adds_epu8(s18, t14);
    m24 = _mm_adds_epu8(s19, t15);
    m25 = _mm_adds_epu8(s18, t15);
    m26 = _mm_adds_epu8(s19, t14);
    a89 = _mm_min_epu8(m24, m23);
    d9 = _mm_cmpeq_epi8(a89, m24);
    a90 = _mm_min_epu8(m26, m25);
    d10 = _mm_cmpeq_epi8(a90, m26);
    s20 = _mm_movemask_epi8(_mm_unpacklo_epi8(d9,d10));
    a91 = ((short int  *) dec);
    a92 = (8 * i9);
    a93 = (a91 + a92);
    *(a93) = s20;
    s21 = _mm_movemask_epi8(_mm_unpackhi_epi8(d9,d10));
    a94 = (a93 + 1);
    *(a94) = s21;
    s22 = _mm_unpacklo_epi8(a89, a90);
    s23 = _mm_unpackhi_epi8(a89, a90);
    a95 = ((__m128i  *) Y);
    *(a95) = s22;
    a96 = (a95 + 1);
    *(a96) = s23;
    a97 = (a71 + 1);
    s24 = *(a97);
    a98 = (a71 + 3);
    s25 = *(a98);
    a99 = (a77 + 1);
    a100 = *(a99);
    a101 = _mm_xor_si128(a76, a100);
    a102 = (a77 + 3);
    a103 = *(a102);
    a104 = _mm_xor_si128(a82, a103);
    t16 = _mm_avg_epu8(a101,a104);
    a105 = ((__m128i ) t16);
    a106 = _mm_srli_epi16(a105, 2);
    a107 = ((__m128i ) a106);
    t17 = _mm_and_si128(a107, _mm_set_epi8(63, 63, 63, 63, 63, 63, 63
					   , 63, 63, 63, 63, 63, 63, 63, 63
					   , 63));
    t18 = _mm_subs_epu8(_mm_set_epi8(63, 63, 63, 63, 63, 63, 63
				     , 63, 63, 63, 63, 63, 63, 63, 63
				     , 63), t17);
    m27 = _mm_adds_epu8(s24, t17);
    m28 = _mm_adds_epu8(s25, t18);
    m29 = _mm_adds_epu8(s24, t18);
    m30 = _mm_adds_epu8(s25, t17);
    a108 = _mm_min_epu8(m28, m27);
    d11 = _mm_cmpeq_epi8(a108, m28);
    a109 = _mm_min_epu8(m30, m29);
    d12 = _mm_cmpeq_epi8(a109, m30);
    s26 = _mm_movemask_epi8(_mm_unpacklo_epi8(d11,d12));
    a110 = (a93 + 2);
    *(a110) = s26;
    s27 = _mm_movemask_epi8(_mm_unpackhi_epi8(d11,d12));
    a111 = (a93 + 3);
    *(a111) = s27;
    s28 = _mm_unpacklo_epi8(a108, a109);
    s29 = _mm_unpackhi_epi8(a108, a109);
    a112 = (a95 + 2);
    *(a112) = s28;
    a113 = (a95 + 3);
    *(a113) = s29;
    if ((((unsigned char  *) Y)[0]>210)) {
      __m128i m5, m6;
      m5 = ((__m128i  *) Y)[0];
      m5 = _mm_min_epu8(m5, ((__m128i  *) Y)[1]);
      m5 = _mm_min_epu8(m5, ((__m128i  *) Y)[2]);
      m5 = _mm_min_epu8(m5, ((__m128i  *) Y)[3]);
      __m128i m7;
      m7 = _mm_min_epu8(_mm_srli_si128(m5, 8), m5);
      m7 = ((__m128i ) _mm_min_epu8(((__m128i ) _mm_srli_epi64(m7, 32)), ((__m128i ) m7)));
      m7 = ((__m128i ) _mm_min_epu8(((__m128i ) _mm_srli_epi64(m7, 16)), ((__m128i ) m7)));
      m7 = ((__m128i ) _mm_min_epu8(((__m128i ) _mm_srli_epi64(m7, 8)), ((__m128i ) m7)));
      m7 = _mm_unpacklo_epi8(m7, m7);
      m7 = _mm_shufflelo_epi16(m7, _MM_SHUFFLE(0, 0, 0, 0));
      m6 = _mm_unpacklo_epi64(m7, m7);
      ((__m128i  *) Y)[0] = _mm_subs_epu8(((__m128i  *) Y)[0], m6);
      ((__m128i  *) Y)[1] = _mm_subs_epu8(((__m128i  *) Y)[1], m6);
      ((__m128i  *) Y)[2] = _mm_subs_epu8(((__m128i  *) Y)[2], m6);
      ((__m128i  *) Y)[3] = _mm_subs_epu8(((__m128i  *) Y)[3], m6);
    }
    unsigned char a188, a194;
    int a186, a205;
    short int s48, s49, s54, s55;
    unsigned char  *a187, *a193, *b15;
    short int  *a204, *a206, *a207, *a223, *a224, *b16;
    __m128i  *a184, *a185, *a190, *a196, *a208, *a209, *a210
      , *a211, *a212, *a215, *a225, *a226;
    __m128i a199, a200, a218, a219;
    __m128i a189, a191, a192, a195, a197, a198, a201
      , a202, a203, a213, a214, a216, a217, a220, a221
      , a222, d17, d18, d19, d20, m39, m40, m41
      , m42, m43, m44, m45, m46, s46, s47, s50
      , s51, s52, s53, s56, s57, t25, t26, t27
      , t28, t29, t30;
    a184 = ((__m128i  *) Y);
    s46 = *(a184);
    a185 = (a184 + 2);
    s47 = *(a185);
    a186 = (4 * i9);
    b15 = (a186 + syms);
    a187 = (b15 + 2);
    a188 = *(a187);
    a189 = _mm_set1_epi8(a188);
    a190 = ((__m128i  *) Branchtab);
    a191 = *(a190);
    a192 = _mm_xor_si128(a189, a191);
    a193 = (b15 + 3);
    a194 = *(a193);
    a195 = _mm_set1_epi8(a194);
    a196 = (a190 + 2);
    a197 = *(a196);
    a198 = _mm_xor_si128(a195, a197);
    t25 = _mm_avg_epu8(a192,a198);
    a199 = ((__m128i ) t25);
    a200 = _mm_srli_epi16(a199, 2);
    a201 = ((__m128i ) a200);
    t26 = _mm_and_si128(a201, _mm_set_epi8(63, 63, 63, 63, 63, 63, 63
					   , 63, 63, 63, 63, 63, 63, 63, 63
					   , 63));
    t27 = _mm_subs_epu8(_mm_set_epi8(63, 63, 63, 63, 63, 63, 63
				     , 63, 63, 63, 63, 63, 63, 63, 63
				     , 63), t26);
    m39 = _mm_adds_epu8(s46, t26);
    m40 = _mm_adds_epu8(s47, t27);
    m41 = _mm_adds_epu8(s46, t27);
    m42 = _mm_adds_epu8(s47, t26);
    a202 = _mm_min_epu8(m40, m39);
    d17 = _mm_cmpeq_epi8(a202, m40);
    a203 = _mm_min_epu8(m42, m41);
    d18 = _mm_cmpeq_epi8(a203, m42);
    s48 = _mm_movemask_epi8(_mm_unpacklo_epi8(d17,d18));
    a204 = ((short int  *) dec);
    a205 = (8 * i9);
    b16 = (a204 + a205);
    a206 = (b16 + 4);
    *(a206) = s48;
    s49 = _mm_movemask_epi8(_mm_unpackhi_epi8(d17,d18));
    a207 = (b16 + 5);
    *(a207) = s49;
    s50 = _mm_unpacklo_epi8(a202, a203);
    s51 = _mm_unpackhi_epi8(a202, a203);
    a208 = ((__m128i  *) X);
    *(a208) = s50;
    a209 = (a208 + 1);
    *(a209) = s51;
    a210 = (a184 + 1);
    s52 = *(a210);
    a211 = (a184 + 3);
    s53 = *(a211);
    a212 = (a190 + 1);
    a213 = *(a212);
    a214 = _mm_xor_si128(a189, a213);
    a215 = (a190 + 3);
    a216 = *(a215);
    a217 = _mm_xor_si128(a195, a216);
    t28 = _mm_avg_epu8(a214,a217);
    a218 = ((__m128i ) t28);
    a219 = _mm_srli_epi16(a218, 2);
    a220 = ((__m128i ) a219);
    t29 = _mm_and_si128(a220, _mm_set_epi8(63, 63, 63, 63, 63, 63, 63
					   , 63, 63, 63, 63, 63, 63, 63, 63
					   , 63));
    t30 = _mm_subs_epu8(_mm_set_epi8(63, 63, 63, 63, 63, 63, 63
				     , 63, 63, 63, 63, 63, 63, 63, 63
				     , 63), t29);
    m43 = _mm_adds_epu8(s52, t29);
    m44 = _mm_adds_epu8(s53, t30);
    m45 = _mm_adds_epu8(s52, t30);
    m46 = _mm_adds_epu8(s53, t29);
    a221 = _mm_min_epu8(m44, m43);
    d19 = _mm_cmpeq_epi8(a221, m44);
    a222 = _mm_min_epu8(m46, m45);
    d20 = _mm_cmpeq_epi8(a222, m46);
    s54 = _mm_movemask_epi8(_mm_unpacklo_epi8(d19,d20));
    a223 = (b16 + 6);
    *(a223) = s54;
    s55 = _mm_movemask_epi8(_mm_unpackhi_epi8(d19,d20));
    a224 = (b16 + 7);
    *(a224) = s55;
    s56 = _mm_unpacklo_epi8(a221, a222);
    s57 = _mm_unpackhi_epi8(a221, a222);
    a225 = (a208 + 2);
    *(a225) = s56;
    a226 = (a208 + 3);
    *(a226) = s57;
    if ((((unsigned char  *) X)[0]>210)) {
      __m128i m12, m13;
      m12 = ((__m128i  *) X)[0];
      m12 = _mm_min_epu8(m12, ((__m128i  *) X)[1]);
      m12 = _mm_min_epu8(m12, ((__m128i  *) X)[2]);
      m12 = _mm_min_epu8(m12, ((__m128i  *) X)[3]);
      __m128i m14;
      m14 = _mm_min_epu8(_mm_srli_si128(m12, 8), m12);
      m14 = ((__m128i ) _mm_min_epu8(((__m128i ) _mm_srli_epi64(m14, 32)), ((__m128i ) m14)));
      m14 = ((__m128i ) _mm_min_epu8(((__m128i ) _mm_srli_epi64(m14, 16)), ((__m128i ) m14)));
      m14 = ((__m128i ) _mm_min_epu8(((__m128i ) _mm_srli_epi64(m14, 8)), ((__m128i ) m14)));
      m14 = _mm_unpacklo_epi8(m14, m14);
      m14 = _mm_shufflelo_epi16(m14, _MM_SHUFFLE(0, 0, 0, 0));
      m13 = _mm_unpacklo_epi64(m14, m14);
      ((__m128i  *) X)[0] = _mm_subs_epu8(((__m128i  *) X)[0], m13);
      ((__m128i  *) X)[1] = _mm_subs_epu8(((__m128i  *) X)[1], m13);
      ((__m128i  *) X)[2] = _mm_subs_epu8(((__m128i  *) X)[2], m13);
      ((__m128i  *) X)[3] = _mm_subs_epu8(((__m128i  *) X)[3], m13);
    }
  }

  renormalize(X, 210);

  /*int ch;
  for(ch = 0; ch < 64; ch++) {
    printf("%d,", X[ch]);
  }
  printf("\n");*/

  unsigned int j;
  for(j=0; j < (framebits + excess) % 2; ++j) {
    int i;
    for(i=0;i<64/2;i++){
      BFLY(i, (((framebits+excess) >> 1) << 1) + j , syms, Y, X, (decision_t *)dec, Branchtab);
    }


    renormalize(Y, 210);

    /*printf("\n");
    for(ch = 0; ch < 64; ch++) {
      printf("%d,", Y[ch]);
    }
    printf("\n");*/

  }
  /*skip*/
  return;
}

#endif /*LV_HAVE_SSE3*/







#if LV_HAVE_GENERIC


static inline void volk_8u_x4_conv_k7_r2_8u_generic(unsigned char* Y, unsigned char* X, unsigned char* syms, unsigned char* dec, unsigned int framebits, unsigned int excess, unsigned char* Branchtab) {
  int nbits = framebits + excess;
  int NUMSTATES = 64;
  int RENORMALIZE_THRESHOLD = 210;


  int s,i;



  for (s=0;s<nbits;s++){
    void *tmp;
    for(i=0;i<NUMSTATES/2;i++){
      BFLY(i, s, syms, Y, X, (decision_t *)dec, Branchtab);
    }



    renormalize(Y, RENORMALIZE_THRESHOLD);

    ///     Swap pointers to old and new metrics
    tmp = (void *)X;
    X = Y;
    Y = (unsigned char*)tmp;
  }


  return;
}

#endif /* LV_HAVE_GENERIC */

#endif /*INCLUDED_volk_8u_x4_conv_k7_r2_8u_H*/

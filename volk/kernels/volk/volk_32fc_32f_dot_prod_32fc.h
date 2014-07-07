#ifndef INCLUDED_volk_32fc_32f_dot_prod_32fc_a_H
#define INCLUDED_volk_32fc_32f_dot_prod_32fc_a_H

#include <volk/volk_common.h>
#include<stdio.h>


#ifdef LV_HAVE_GENERIC


static inline void volk_32fc_32f_dot_prod_32fc_generic(lv_32fc_t* result, const lv_32fc_t* input, const float * taps, unsigned int num_points) {

  float res[2];
  float *realpt = &res[0], *imagpt = &res[1];
  const float* aPtr = (float*)input;
  const float* bPtr=  taps;
  unsigned int number = 0;

  *realpt = 0;
  *imagpt = 0;

  for(number = 0; number < num_points; number++){
    *realpt += ((*aPtr++) * (*bPtr));
    *imagpt += ((*aPtr++) * (*bPtr++));
  }

  *result = *(lv_32fc_t*)(&res[0]);
}

#endif /*LV_HAVE_GENERIC*/


#ifdef LV_HAVE_AVX

#include <immintrin.h>

static inline void volk_32fc_32f_dot_prod_32fc_a_avx( lv_32fc_t* result, const lv_32fc_t* input, const float* taps, unsigned int num_points) {

  unsigned int number = 0;
  const unsigned int sixteenthPoints = num_points / 16;

  float res[2];
  float *realpt = &res[0], *imagpt = &res[1];
  const float* aPtr = (float*)input;
  const float* bPtr = taps;

  __m256 a0Val, a1Val, a2Val, a3Val;
  __m256 b0Val, b1Val, b2Val, b3Val;
  __m256 x0Val, x1Val, x0loVal, x0hiVal, x1loVal, x1hiVal;
  __m256 c0Val, c1Val, c2Val, c3Val;

  __m256 dotProdVal0 = _mm256_setzero_ps();
  __m256 dotProdVal1 = _mm256_setzero_ps();
  __m256 dotProdVal2 = _mm256_setzero_ps();
  __m256 dotProdVal3 = _mm256_setzero_ps();

  for(;number < sixteenthPoints; number++){

    a0Val = _mm256_load_ps(aPtr);
    a1Val = _mm256_load_ps(aPtr+8);
    a2Val = _mm256_load_ps(aPtr+16);
    a3Val = _mm256_load_ps(aPtr+24);

    x0Val = _mm256_load_ps(bPtr); // t0|t1|t2|t3|t4|t5|t6|t7
    x1Val = _mm256_load_ps(bPtr+8);
    x0loVal = _mm256_unpacklo_ps(x0Val, x0Val); // t0|t0|t1|t1|t4|t4|t5|t5
    x0hiVal = _mm256_unpackhi_ps(x0Val, x0Val); // t2|t2|t3|t3|t6|t6|t7|t7
    x1loVal = _mm256_unpacklo_ps(x1Val, x1Val);
    x1hiVal = _mm256_unpackhi_ps(x1Val, x1Val);

    // TODO: it may be possible to rearrange swizzling to better pipeline data
    b0Val = _mm256_permute2f128_ps(x0loVal, x0hiVal, 0x20); // t0|t0|t1|t1|t2|t2|t3|t3
    b1Val = _mm256_permute2f128_ps(x0loVal, x0hiVal, 0x31); // t4|t4|t5|t5|t6|t6|t7|t7
    b2Val = _mm256_permute2f128_ps(x1loVal, x1hiVal, 0x20);
    b3Val = _mm256_permute2f128_ps(x1loVal, x1hiVal, 0x31);

    c0Val = _mm256_mul_ps(a0Val, b0Val);
    c1Val = _mm256_mul_ps(a1Val, b1Val);
    c2Val = _mm256_mul_ps(a2Val, b2Val);
    c3Val = _mm256_mul_ps(a3Val, b3Val);

    dotProdVal0 = _mm256_add_ps(c0Val, dotProdVal0);
    dotProdVal1 = _mm256_add_ps(c1Val, dotProdVal1);
    dotProdVal2 = _mm256_add_ps(c2Val, dotProdVal2);
    dotProdVal3 = _mm256_add_ps(c3Val, dotProdVal3);

    aPtr += 32;
    bPtr += 16;
  }

  dotProdVal0 = _mm256_add_ps(dotProdVal0, dotProdVal1);
  dotProdVal0 = _mm256_add_ps(dotProdVal0, dotProdVal2);
  dotProdVal0 = _mm256_add_ps(dotProdVal0, dotProdVal3);

  __VOLK_ATTR_ALIGNED(32) float dotProductVector[8];

  _mm256_store_ps(dotProductVector,dotProdVal0); // Store the results back into the dot product vector

  *realpt = dotProductVector[0];
  *imagpt = dotProductVector[1];
  *realpt += dotProductVector[2];
  *imagpt += dotProductVector[3];
  *realpt += dotProductVector[4];
  *imagpt += dotProductVector[5];
  *realpt += dotProductVector[6];
  *imagpt += dotProductVector[7];

  number = sixteenthPoints*16;
  for(;number < num_points; number++){
    *realpt += ((*aPtr++) * (*bPtr));
    *imagpt += ((*aPtr++) * (*bPtr++));
  }

  *result = *(lv_32fc_t*)(&res[0]);
}

#endif /*LV_HAVE_AVX*/




#ifdef LV_HAVE_SSE


static inline void volk_32fc_32f_dot_prod_32fc_a_sse( lv_32fc_t* result, const  lv_32fc_t* input, const  float* taps, unsigned int num_points) {

  unsigned int number = 0;
  const unsigned int sixteenthPoints = num_points / 8;

  float res[2];
  float *realpt = &res[0], *imagpt = &res[1];
  const float* aPtr = (float*)input;
  const float* bPtr = taps;

  __m128 a0Val, a1Val, a2Val, a3Val;
  __m128 b0Val, b1Val, b2Val, b3Val;
  __m128 x0Val, x1Val, x2Val, x3Val;
  __m128 c0Val, c1Val, c2Val, c3Val;

  __m128 dotProdVal0 = _mm_setzero_ps();
  __m128 dotProdVal1 = _mm_setzero_ps();
  __m128 dotProdVal2 = _mm_setzero_ps();
  __m128 dotProdVal3 = _mm_setzero_ps();

  for(;number < sixteenthPoints; number++){

    a0Val = _mm_load_ps(aPtr);
    a1Val = _mm_load_ps(aPtr+4);
    a2Val = _mm_load_ps(aPtr+8);
    a3Val = _mm_load_ps(aPtr+12);

    x0Val = _mm_load_ps(bPtr);
    x1Val = _mm_load_ps(bPtr);
    x2Val = _mm_load_ps(bPtr+4);
    x3Val = _mm_load_ps(bPtr+4);
    b0Val = _mm_unpacklo_ps(x0Val, x1Val);
    b1Val = _mm_unpackhi_ps(x0Val, x1Val);
    b2Val = _mm_unpacklo_ps(x2Val, x3Val);
    b3Val = _mm_unpackhi_ps(x2Val, x3Val);

    c0Val = _mm_mul_ps(a0Val, b0Val);
    c1Val = _mm_mul_ps(a1Val, b1Val);
    c2Val = _mm_mul_ps(a2Val, b2Val);
    c3Val = _mm_mul_ps(a3Val, b3Val);

    dotProdVal0 = _mm_add_ps(c0Val, dotProdVal0);
    dotProdVal1 = _mm_add_ps(c1Val, dotProdVal1);
    dotProdVal2 = _mm_add_ps(c2Val, dotProdVal2);
    dotProdVal3 = _mm_add_ps(c3Val, dotProdVal3);

    aPtr += 16;
    bPtr += 8;
  }

  dotProdVal0 = _mm_add_ps(dotProdVal0, dotProdVal1);
  dotProdVal0 = _mm_add_ps(dotProdVal0, dotProdVal2);
  dotProdVal0 = _mm_add_ps(dotProdVal0, dotProdVal3);

  __VOLK_ATTR_ALIGNED(16) float dotProductVector[4];

  _mm_store_ps(dotProductVector,dotProdVal0); // Store the results back into the dot product vector

  *realpt = dotProductVector[0];
  *imagpt = dotProductVector[1];
  *realpt += dotProductVector[2];
  *imagpt += dotProductVector[3];

  number = sixteenthPoints*8;
  for(;number < num_points; number++){
    *realpt += ((*aPtr++) * (*bPtr));
    *imagpt += ((*aPtr++) * (*bPtr++));
  }

  *result = *(lv_32fc_t*)(&res[0]);
}

#endif /*LV_HAVE_SSE*/



#ifdef LV_HAVE_AVX

#include <immintrin.h>

static inline void volk_32fc_32f_dot_prod_32fc_u_avx( lv_32fc_t* result, const lv_32fc_t* input, const float* taps, unsigned int num_points) {

  unsigned int number = 0;
  const unsigned int sixteenthPoints = num_points / 16;

  float res[2];
  float *realpt = &res[0], *imagpt = &res[1];
  const float* aPtr = (float*)input;
  const float* bPtr = taps;

  __m256 a0Val, a1Val, a2Val, a3Val;
  __m256 b0Val, b1Val, b2Val, b3Val;
  __m256 x0Val, x1Val, x0loVal, x0hiVal, x1loVal, x1hiVal;
  __m256 c0Val, c1Val, c2Val, c3Val;

  __m256 dotProdVal0 = _mm256_setzero_ps();
  __m256 dotProdVal1 = _mm256_setzero_ps();
  __m256 dotProdVal2 = _mm256_setzero_ps();
  __m256 dotProdVal3 = _mm256_setzero_ps();

  for(;number < sixteenthPoints; number++){

    a0Val = _mm256_loadu_ps(aPtr);
    a1Val = _mm256_loadu_ps(aPtr+8);
    a2Val = _mm256_loadu_ps(aPtr+16);
    a3Val = _mm256_loadu_ps(aPtr+24);

    x0Val = _mm256_loadu_ps(bPtr); // t0|t1|t2|t3|t4|t5|t6|t7
    x1Val = _mm256_loadu_ps(bPtr+8);
    x0loVal = _mm256_unpacklo_ps(x0Val, x0Val); // t0|t0|t1|t1|t4|t4|t5|t5
    x0hiVal = _mm256_unpackhi_ps(x0Val, x0Val); // t2|t2|t3|t3|t6|t6|t7|t7
    x1loVal = _mm256_unpacklo_ps(x1Val, x1Val);
    x1hiVal = _mm256_unpackhi_ps(x1Val, x1Val);

    // TODO: it may be possible to rearrange swizzling to better pipeline data
    b0Val = _mm256_permute2f128_ps(x0loVal, x0hiVal, 0x20); // t0|t0|t1|t1|t2|t2|t3|t3
    b1Val = _mm256_permute2f128_ps(x0loVal, x0hiVal, 0x31); // t4|t4|t5|t5|t6|t6|t7|t7
    b2Val = _mm256_permute2f128_ps(x1loVal, x1hiVal, 0x20);
    b3Val = _mm256_permute2f128_ps(x1loVal, x1hiVal, 0x31);

    c0Val = _mm256_mul_ps(a0Val, b0Val);
    c1Val = _mm256_mul_ps(a1Val, b1Val);
    c2Val = _mm256_mul_ps(a2Val, b2Val);
    c3Val = _mm256_mul_ps(a3Val, b3Val);

    dotProdVal0 = _mm256_add_ps(c0Val, dotProdVal0);
    dotProdVal1 = _mm256_add_ps(c1Val, dotProdVal1);
    dotProdVal2 = _mm256_add_ps(c2Val, dotProdVal2);
    dotProdVal3 = _mm256_add_ps(c3Val, dotProdVal3);

    aPtr += 32;
    bPtr += 16;
  }

  dotProdVal0 = _mm256_add_ps(dotProdVal0, dotProdVal1);
  dotProdVal0 = _mm256_add_ps(dotProdVal0, dotProdVal2);
  dotProdVal0 = _mm256_add_ps(dotProdVal0, dotProdVal3);

  __VOLK_ATTR_ALIGNED(32) float dotProductVector[8];

  _mm256_store_ps(dotProductVector,dotProdVal0); // Store the results back into the dot product vector

  *realpt = dotProductVector[0];
  *imagpt = dotProductVector[1];
  *realpt += dotProductVector[2];
  *imagpt += dotProductVector[3];
  *realpt += dotProductVector[4];
  *imagpt += dotProductVector[5];
  *realpt += dotProductVector[6];
  *imagpt += dotProductVector[7];

  number = sixteenthPoints*16;
  for(;number < num_points; number++){
    *realpt += ((*aPtr++) * (*bPtr));
    *imagpt += ((*aPtr++) * (*bPtr++));
  }

  *result = *(lv_32fc_t*)(&res[0]);
}
#endif /*LV_HAVE_AVX*/


#ifdef LV_HAVE_SSE

static inline void volk_32fc_32f_dot_prod_32fc_u_sse( lv_32fc_t* result, const  lv_32fc_t* input, const  float* taps, unsigned int num_points) {

  unsigned int number = 0;
  const unsigned int sixteenthPoints = num_points / 8;

  float res[2];
  float *realpt = &res[0], *imagpt = &res[1];
  const float* aPtr = (float*)input;
  const float* bPtr = taps;

  __m128 a0Val, a1Val, a2Val, a3Val;
  __m128 b0Val, b1Val, b2Val, b3Val;
  __m128 x0Val, x1Val, x2Val, x3Val;
  __m128 c0Val, c1Val, c2Val, c3Val;

  __m128 dotProdVal0 = _mm_setzero_ps();
  __m128 dotProdVal1 = _mm_setzero_ps();
  __m128 dotProdVal2 = _mm_setzero_ps();
  __m128 dotProdVal3 = _mm_setzero_ps();

  for(;number < sixteenthPoints; number++){

    a0Val = _mm_loadu_ps(aPtr);
    a1Val = _mm_loadu_ps(aPtr+4);
    a2Val = _mm_loadu_ps(aPtr+8);
    a3Val = _mm_loadu_ps(aPtr+12);

    x0Val = _mm_loadu_ps(bPtr);
    x1Val = _mm_loadu_ps(bPtr);
    x2Val = _mm_loadu_ps(bPtr+4);
    x3Val = _mm_loadu_ps(bPtr+4);
    b0Val = _mm_unpacklo_ps(x0Val, x1Val);
    b1Val = _mm_unpackhi_ps(x0Val, x1Val);
    b2Val = _mm_unpacklo_ps(x2Val, x3Val);
    b3Val = _mm_unpackhi_ps(x2Val, x3Val);

    c0Val = _mm_mul_ps(a0Val, b0Val);
    c1Val = _mm_mul_ps(a1Val, b1Val);
    c2Val = _mm_mul_ps(a2Val, b2Val);
    c3Val = _mm_mul_ps(a3Val, b3Val);

    dotProdVal0 = _mm_add_ps(c0Val, dotProdVal0);
    dotProdVal1 = _mm_add_ps(c1Val, dotProdVal1);
    dotProdVal2 = _mm_add_ps(c2Val, dotProdVal2);
    dotProdVal3 = _mm_add_ps(c3Val, dotProdVal3);

    aPtr += 16;
    bPtr += 8;
  }

  dotProdVal0 = _mm_add_ps(dotProdVal0, dotProdVal1);
  dotProdVal0 = _mm_add_ps(dotProdVal0, dotProdVal2);
  dotProdVal0 = _mm_add_ps(dotProdVal0, dotProdVal3);

  __VOLK_ATTR_ALIGNED(16) float dotProductVector[4];

  _mm_store_ps(dotProductVector,dotProdVal0); // Store the results back into the dot product vector

  *realpt = dotProductVector[0];
  *imagpt = dotProductVector[1];
  *realpt += dotProductVector[2];
  *imagpt += dotProductVector[3];

  number = sixteenthPoints*8;
  for(;number < num_points; number++){
    *realpt += ((*aPtr++) * (*bPtr));
    *imagpt += ((*aPtr++) * (*bPtr++));
  }

  *result = *(lv_32fc_t*)(&res[0]);
}

#endif /*LV_HAVE_SSE*/


#endif /*INCLUDED_volk_32fc_32f_dot_prod_32fc_H*/

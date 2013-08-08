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


#endif /*INCLUDED_volk_32fc_32f_dot_prod_32fc_a_H*/

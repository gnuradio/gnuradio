#ifndef INCLUDED_volk_32f_x2_dot_prod_16i_a_H
#define INCLUDED_volk_32f_x2_dot_prod_16i_a_H

#include <volk/volk_common.h>
#include<stdio.h>


#ifdef LV_HAVE_GENERIC


static inline void volk_32f_x2_dot_prod_16i_generic(int16_t* result, const float* input, const float* taps, unsigned int num_points) {

  float dotProduct = 0;
  const float* aPtr = input;
  const float* bPtr=  taps;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    dotProduct += ((*aPtr++) * (*bPtr++));
  }

  *result = (int16_t)dotProduct;
}

#endif /*LV_HAVE_GENERIC*/


#ifdef LV_HAVE_SSE


static inline void volk_32f_x2_dot_prod_16i_a_sse(int16_t* result, const  float* input, const  float* taps, unsigned int num_points) {

  unsigned int number = 0;
  const unsigned int sixteenthPoints = num_points / 16;

  float dotProduct = 0;
  const float* aPtr = input;
  const float* bPtr = taps;

  __m128 a0Val, a1Val, a2Val, a3Val;
  __m128 b0Val, b1Val, b2Val, b3Val;
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
    b0Val = _mm_load_ps(bPtr);
    b1Val = _mm_load_ps(bPtr+4);
    b2Val = _mm_load_ps(bPtr+8);
    b3Val = _mm_load_ps(bPtr+12);

    c0Val = _mm_mul_ps(a0Val, b0Val);
    c1Val = _mm_mul_ps(a1Val, b1Val);
    c2Val = _mm_mul_ps(a2Val, b2Val);
    c3Val = _mm_mul_ps(a3Val, b3Val);

    dotProdVal0 = _mm_add_ps(c0Val, dotProdVal0);
    dotProdVal1 = _mm_add_ps(c1Val, dotProdVal1);
    dotProdVal2 = _mm_add_ps(c2Val, dotProdVal2);
    dotProdVal3 = _mm_add_ps(c3Val, dotProdVal3);

    aPtr += 16;
    bPtr += 16;
  }

  dotProdVal0 = _mm_add_ps(dotProdVal0, dotProdVal1);
  dotProdVal0 = _mm_add_ps(dotProdVal0, dotProdVal2);
  dotProdVal0 = _mm_add_ps(dotProdVal0, dotProdVal3);

  __VOLK_ATTR_ALIGNED(16) float dotProductVector[4];

  _mm_store_ps(dotProductVector,dotProdVal0); // Store the results back into the dot product vector

  dotProduct = dotProductVector[0];
  dotProduct += dotProductVector[1];
  dotProduct += dotProductVector[2];
  dotProduct += dotProductVector[3];

  number = sixteenthPoints*16;
  for(;number < num_points; number++){
    dotProduct += ((*aPtr++) * (*bPtr++));
  }

  *result = (short)dotProduct;

}

#endif /*LV_HAVE_SSE*/

#endif /*INCLUDED_volk_32f_x2_dot_prod_16i_a_H*/

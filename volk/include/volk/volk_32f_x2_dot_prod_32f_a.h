#ifndef INCLUDED_volk_32f_x2_dot_prod_32f_a_H
#define INCLUDED_volk_32f_x2_dot_prod_32f_a_H

#include <volk/volk_common.h>
#include<stdio.h>


#ifdef LV_HAVE_GENERIC


static inline void volk_32f_x2_dot_prod_32f_a_generic(float * result, const float * input, const float * taps, unsigned int num_points) {

  float dotProduct = 0;
  const float* aPtr = input;
  const float* bPtr=  taps;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    dotProduct += ((*aPtr++) * (*bPtr++));
  }
  
  *result = dotProduct;
}

#endif /*LV_HAVE_GENERIC*/


#ifdef LV_HAVE_SSE


static inline void volk_32f_x2_dot_prod_32f_a_sse( float* result, const  float* input, const  float* taps, unsigned int num_points) {
  
  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;

  float dotProduct = 0;
  const float* aPtr = input;
  const float* bPtr = taps;

  __m128 aVal, bVal, cVal;

  __m128 dotProdVal = _mm_setzero_ps();

  for(;number < quarterPoints; number++){
      
    aVal = _mm_load_ps(aPtr); 
    bVal = _mm_load_ps(bPtr);
      
    cVal = _mm_mul_ps(aVal, bVal); 

    dotProdVal = _mm_add_ps(cVal, dotProdVal);

    aPtr += 4;
    bPtr += 4;
  }

  __VOLK_ATTR_ALIGNED(16) float dotProductVector[4];

  _mm_store_ps(dotProductVector,dotProdVal); // Store the results back into the dot product vector

  dotProduct = dotProductVector[0];
  dotProduct += dotProductVector[1];
  dotProduct += dotProductVector[2];
  dotProduct += dotProductVector[3];

  number = quarterPoints * 4;
  for(;number < num_points; number++){
    dotProduct += ((*aPtr++) * (*bPtr++));
  }

  *result = dotProduct;
  
}

#endif /*LV_HAVE_SSE*/  

#ifdef LV_HAVE_SSE3

#include <pmmintrin.h>

static inline void volk_32f_x2_dot_prod_32f_a_sse3(float * result, const float * input, const float * taps, unsigned int num_points) {
  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;

  float dotProduct = 0;
  const float* aPtr = input;
  const float* bPtr = taps;

  __m128 aVal, bVal, cVal;

  __m128 dotProdVal = _mm_setzero_ps();

  for(;number < quarterPoints; number++){
      
    aVal = _mm_load_ps(aPtr); 
    bVal = _mm_load_ps(bPtr);
      
    cVal = _mm_mul_ps(aVal, bVal); 

    dotProdVal = _mm_hadd_ps(dotProdVal, cVal);

    aPtr += 4;
    bPtr += 4;
  }

  __VOLK_ATTR_ALIGNED(16) float dotProductVector[4];
  dotProdVal = _mm_hadd_ps(dotProdVal, dotProdVal);

  _mm_store_ps(dotProductVector,dotProdVal); // Store the results back into the dot product vector

  dotProduct = dotProductVector[0];
  dotProduct += dotProductVector[1];

  number = quarterPoints * 4;
  for(;number < num_points; number++){
    dotProduct += ((*aPtr++) * (*bPtr++));
  }

  *result = dotProduct;
}  

#endif /*LV_HAVE_SSE3*/

#ifdef LV_HAVE_SSE4_1

#include <smmintrin.h>

static inline void volk_32f_x2_dot_prod_32f_a_sse4_1(float * result, const float * input, const float* taps, unsigned int num_points) {
  unsigned int number = 0;
  const unsigned int sixteenthPoints = num_points / 16;

  float dotProduct = 0;
  const float* aPtr = input;
  const float* bPtr = taps;

  __m128 aVal1, bVal1, cVal1;
  __m128 aVal2, bVal2, cVal2;
  __m128 aVal3, bVal3, cVal3;
  __m128 aVal4, bVal4, cVal4;

  __m128 dotProdVal = _mm_setzero_ps();

  for(;number < sixteenthPoints; number++){      

    aVal1 = _mm_load_ps(aPtr); aPtr += 4;
    aVal2 = _mm_load_ps(aPtr); aPtr += 4;
    aVal3 = _mm_load_ps(aPtr); aPtr += 4;
    aVal4 = _mm_load_ps(aPtr); aPtr += 4;

    bVal1 = _mm_load_ps(bPtr); bPtr += 4;
    bVal2 = _mm_load_ps(bPtr); bPtr += 4;
    bVal3 = _mm_load_ps(bPtr); bPtr += 4;
    bVal4 = _mm_load_ps(bPtr); bPtr += 4;
    
    cVal1 = _mm_dp_ps(aVal1, bVal1, 0xF1);
    cVal2 = _mm_dp_ps(aVal2, bVal2, 0xF2);
    cVal3 = _mm_dp_ps(aVal3, bVal3, 0xF4);
    cVal4 = _mm_dp_ps(aVal4, bVal4, 0xF8);

    cVal1 = _mm_or_ps(cVal1, cVal2);
    cVal3 = _mm_or_ps(cVal3, cVal4);
    cVal1 = _mm_or_ps(cVal1, cVal3);

    dotProdVal = _mm_add_ps(dotProdVal, cVal1);
  }

  __VOLK_ATTR_ALIGNED(16) float dotProductVector[4];
  _mm_store_ps(dotProductVector, dotProdVal); // Store the results back into the dot product vector

  dotProduct = dotProductVector[0];
  dotProduct += dotProductVector[1];
  dotProduct += dotProductVector[2];
  dotProduct += dotProductVector[3];

  number = sixteenthPoints * 16;
  for(;number < num_points; number++){
    dotProduct += ((*aPtr++) * (*bPtr++));
  }

  *result = dotProduct;
}  

#endif /*LV_HAVE_SSE4_1*/

#endif /*INCLUDED_volk_32f_x2_dot_prod_32f_a_H*/

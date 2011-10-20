#ifndef INCLUDED_volk_32f_index_max_16u_a_H
#define INCLUDED_volk_32f_index_max_16u_a_H

#include <volk/volk_common.h>
#include <volk/volk_common.h>
#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE4_1
#include<smmintrin.h>

static inline void volk_32f_index_max_16u_a_sse4_1(unsigned int* target, const float* src0, unsigned int num_points) {
  if(num_points > 0){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    float* inputPtr = (float*)src0;

    __m128 indexIncrementValues = _mm_set1_ps(4);
    __m128 currentIndexes = _mm_set_ps(-1,-2,-3,-4);

    float max = src0[0];
    float index = 0;
    __m128 maxValues = _mm_set1_ps(max);
    __m128 maxValuesIndex = _mm_setzero_ps();
    __m128 compareResults;
    __m128 currentValues;

    __VOLK_ATTR_ALIGNED(16) float maxValuesBuffer[4];
    __VOLK_ATTR_ALIGNED(16) float maxIndexesBuffer[4];

    for(;number < quarterPoints; number++){

      currentValues  = _mm_load_ps(inputPtr); inputPtr += 4;
      currentIndexes = _mm_add_ps(currentIndexes, indexIncrementValues);

      compareResults = _mm_cmpgt_ps(maxValues, currentValues);

      maxValuesIndex = _mm_blendv_ps(currentIndexes, maxValuesIndex, compareResults);
      maxValues      = _mm_blendv_ps(currentValues, maxValues, compareResults);
    }

    // Calculate the largest value from the remaining 4 points
    _mm_store_ps(maxValuesBuffer, maxValues);
    _mm_store_ps(maxIndexesBuffer, maxValuesIndex);

    for(number = 0; number < 4; number++){
      if(maxValuesBuffer[number] > max){
	index = maxIndexesBuffer[number];
	max = maxValuesBuffer[number];
      }
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){      
      if(src0[number] > max){
	index = number;
	max = src0[number];
      }
    }
    target[0] = (unsigned int)index;
  }
}

#endif /*LV_HAVE_SSE4_1*/

#ifdef LV_HAVE_SSE
#include<xmmintrin.h>

static inline void volk_32f_index_max_16u_a_sse(unsigned int* target, const float* src0, unsigned int num_points) {
  if(num_points > 0){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    float* inputPtr = (float*)src0;

    __m128 indexIncrementValues = _mm_set1_ps(4);
    __m128 currentIndexes = _mm_set_ps(-1,-2,-3,-4);

    float max = src0[0];
    float index = 0;
    __m128 maxValues = _mm_set1_ps(max);
    __m128 maxValuesIndex = _mm_setzero_ps();
    __m128 compareResults;
    __m128 currentValues;

    __VOLK_ATTR_ALIGNED(16) float maxValuesBuffer[4];
    __VOLK_ATTR_ALIGNED(16) float maxIndexesBuffer[4];

    for(;number < quarterPoints; number++){

      currentValues  = _mm_load_ps(inputPtr); inputPtr += 4;
      currentIndexes = _mm_add_ps(currentIndexes, indexIncrementValues);

      compareResults = _mm_cmpgt_ps(maxValues, currentValues);

      maxValuesIndex = _mm_or_ps(_mm_and_ps(compareResults, maxValuesIndex) , _mm_andnot_ps(compareResults, currentIndexes));

      maxValues      = _mm_or_ps(_mm_and_ps(compareResults, maxValues) , _mm_andnot_ps(compareResults, currentValues));
    }

    // Calculate the largest value from the remaining 4 points
    _mm_store_ps(maxValuesBuffer, maxValues);
    _mm_store_ps(maxIndexesBuffer, maxValuesIndex);

    for(number = 0; number < 4; number++){
      if(maxValuesBuffer[number] > max){
	index = maxIndexesBuffer[number];
	max = maxValuesBuffer[number];
      }
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){      
      if(src0[number] > max){
	index = number;
	max = src0[number];
      }
    }
    target[0] = (unsigned int)index;
  }
}

#endif /*LV_HAVE_SSE*/

#ifdef LV_HAVE_GENERIC
static inline void volk_32f_index_max_16u_a_generic(unsigned int* target, const float* src0, unsigned int num_points) {
  if(num_points > 0){
    float max = src0[0];
    unsigned int index = 0;
    
    unsigned int i = 1; 
    
    for(; i < num_points; ++i) {
      
      if(src0[i] > max){
	index = i;
	max = src0[i];
      }

    }
    target[0] = index;
  }
}

#endif /*LV_HAVE_GENERIC*/


#endif /*INCLUDED_volk_32f_index_max_16u_a_H*/

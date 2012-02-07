#ifndef INCLUDED_volk_32f_s32f_stddev_32f_a_H
#define INCLUDED_volk_32f_s32f_stddev_32f_a_H

#include <volk/volk_common.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Calculates the standard deviation of the input buffer using the supplied mean
  \param stddev The calculated standard deviation
  \param inputBuffer The buffer of points to calculate the std deviation for
  \param mean The mean of the input buffer
  \param num_points The number of values in input buffer to used in the stddev calculation
*/
static inline void volk_32f_s32f_stddev_32f_a_sse4_1(float* stddev, const float* inputBuffer, const float mean, unsigned int num_points){
  float returnValue = 0;
  if(num_points > 0){
    unsigned int number = 0;
    const unsigned int sixteenthPoints = num_points / 16;

    const float* aPtr = inputBuffer;

    __VOLK_ATTR_ALIGNED(16) float squareBuffer[4];

    __m128 squareAccumulator = _mm_setzero_ps();
    __m128 aVal1, aVal2, aVal3, aVal4;
    __m128 cVal1, cVal2, cVal3, cVal4;
    for(;number < sixteenthPoints; number++) {
      aVal1 = _mm_load_ps(aPtr); aPtr += 4;    
      cVal1 = _mm_dp_ps(aVal1, aVal1, 0xF1);

      aVal2 = _mm_load_ps(aPtr); aPtr += 4;
      cVal2 = _mm_dp_ps(aVal2, aVal2, 0xF2);

      aVal3 = _mm_load_ps(aPtr); aPtr += 4;
      cVal3 = _mm_dp_ps(aVal3, aVal3, 0xF4);

      aVal4 = _mm_load_ps(aPtr); aPtr += 4;
      cVal4 = _mm_dp_ps(aVal4, aVal4, 0xF8);

      cVal1 = _mm_or_ps(cVal1, cVal2);
      cVal3 = _mm_or_ps(cVal3, cVal4);
      cVal1 = _mm_or_ps(cVal1, cVal3);

      squareAccumulator = _mm_add_ps(squareAccumulator, cVal1); // squareAccumulator += x^2
    }
    _mm_store_ps(squareBuffer,squareAccumulator); // Store the results back into the C container  
    returnValue = squareBuffer[0];
    returnValue += squareBuffer[1];
    returnValue += squareBuffer[2];
    returnValue += squareBuffer[3];
  
    number = sixteenthPoints * 16;
    for(;number < num_points; number++){
      returnValue += (*aPtr) * (*aPtr);
      aPtr++;
    }
    returnValue /= num_points;
    returnValue -= (mean * mean);
    returnValue = sqrtf(returnValue);
  }
  *stddev = returnValue;
}
#endif /* LV_HAVE_SSE4_1 */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Calculates the standard deviation of the input buffer using the supplied mean
  \param stddev The calculated standard deviation
  \param inputBuffer The buffer of points to calculate the std deviation for
  \param mean The mean of the input buffer
  \param num_points The number of values in input buffer to used in the stddev calculation
*/
static inline void volk_32f_s32f_stddev_32f_a_sse(float* stddev, const float* inputBuffer, const float mean, unsigned int num_points){
  float returnValue = 0;
  if(num_points > 0){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    const float* aPtr = inputBuffer;

    __VOLK_ATTR_ALIGNED(16) float squareBuffer[4];

    __m128 squareAccumulator = _mm_setzero_ps();
    __m128 aVal = _mm_setzero_ps();
    for(;number < quarterPoints; number++) {
      aVal = _mm_load_ps(aPtr);                     // aVal = x
      aVal = _mm_mul_ps(aVal, aVal);                // squareAccumulator += x^2
      squareAccumulator = _mm_add_ps(squareAccumulator, aVal);
      aPtr += 4;
    }
    _mm_store_ps(squareBuffer,squareAccumulator); // Store the results back into the C container  
    returnValue = squareBuffer[0];
    returnValue += squareBuffer[1];
    returnValue += squareBuffer[2];
    returnValue += squareBuffer[3];
  
    number = quarterPoints * 4;
    for(;number < num_points; number++){
      returnValue += (*aPtr) * (*aPtr);
      aPtr++;
    }
    returnValue /= num_points;
    returnValue -= (mean * mean);
    returnValue = sqrtf(returnValue);
  }
  *stddev = returnValue;
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Calculates the standard deviation of the input buffer using the supplied mean
  \param stddev The calculated standard deviation
  \param inputBuffer The buffer of points to calculate the std deviation for
  \param mean The mean of the input buffer
  \param num_points The number of values in input buffer to used in the stddev calculation
*/
static inline void volk_32f_s32f_stddev_32f_a_generic(float* stddev, const float* inputBuffer, const float mean, unsigned int num_points){
  float returnValue = 0;
  if(num_points > 0){
    const float* aPtr = inputBuffer;
    unsigned int number = 0;
      
    for(number = 0; number < num_points; number++){
      returnValue += (*aPtr) * (*aPtr);
      aPtr++;
    }

    returnValue /= num_points;
    returnValue -= (mean * mean);
    returnValue = sqrtf(returnValue);
  }
  *stddev = returnValue;
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32f_s32f_stddev_32f_a_H */

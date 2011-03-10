#ifndef INCLUDED_volk_16i_s32f_convert_32f_u_H
#define INCLUDED_volk_16i_s32f_convert_32f_u_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>

  /*!
    \brief Converts the input 16 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 16 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
    \note Output buffer does NOT need to be properly aligned
  */
static inline void volk_16i_s32f_convert_32f_u_sse4_1(float* outputVector, const int16_t* inputVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int eighthPoints = num_points / 8;
    
     float* outputVectorPtr = outputVector;
    __m128 invScalar = _mm_set_ps1(1.0/scalar);
    int16_t* inputPtr = (int16_t*)inputVector;
    __m128i inputVal;
    __m128i inputVal2;
    __m128 ret;

    for(;number < eighthPoints; number++){

      // Load the 8 values
      inputVal = _mm_loadu_si128((__m128i*)inputPtr);

      // Shift the input data to the right by 64 bits ( 8 bytes )
      inputVal2 = _mm_srli_si128(inputVal, 8);

      // Convert the lower 4 values into 32 bit words
      inputVal = _mm_cvtepi16_epi32(inputVal);
      inputVal2 = _mm_cvtepi16_epi32(inputVal2);
      
      ret = _mm_cvtepi32_ps(inputVal);
      ret = _mm_mul_ps(ret, invScalar);
      _mm_storeu_ps(outputVectorPtr, ret);
      outputVectorPtr += 4;

      ret = _mm_cvtepi32_ps(inputVal2);
      ret = _mm_mul_ps(ret, invScalar);
      _mm_storeu_ps(outputVectorPtr, ret);

      outputVectorPtr += 4;

      inputPtr += 8;
    }

    number = eighthPoints * 8;
    for(; number < num_points; number++){
      outputVector[number] =((float)(inputVector[number])) / scalar;
    }
}
#endif /* LV_HAVE_SSE4_1 */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>

  /*!
    \brief Converts the input 16 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 16 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
    \note Output buffer does NOT need to be properly aligned
  */
static inline void volk_16i_s32f_convert_32f_u_sse(float* outputVector, const int16_t* inputVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;
    
    float* outputVectorPtr = outputVector;
    __m128 invScalar = _mm_set_ps1(1.0/scalar);
    int16_t* inputPtr = (int16_t*)inputVector;
    __m128 ret;

    for(;number < quarterPoints; number++){
      ret = _mm_set_ps((float)(inputPtr[3]), (float)(inputPtr[2]), (float)(inputPtr[1]), (float)(inputPtr[0]));
      
      ret = _mm_mul_ps(ret, invScalar);
      _mm_storeu_ps(outputVectorPtr, ret);

      inputPtr += 4;
      outputVectorPtr += 4;
    }

    number = quarterPoints * 4;
    for(; number < num_points; number++){
      outputVector[number] = (float)(inputVector[number]) / scalar;
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Converts the input 16 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 16 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
    \note Output buffer does NOT need to be properly aligned
  */
static inline void volk_16i_s32f_convert_32f_u_generic(float* outputVector, const int16_t* inputVector, const float scalar, unsigned int num_points){
  float* outputVectorPtr = outputVector;
  const int16_t* inputVectorPtr = inputVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *outputVectorPtr++ = ((float)(*inputVectorPtr++)) / scalar;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_16i_s32f_convert_32f_u_H */

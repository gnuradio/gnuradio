#ifndef INCLUDED_volk_32i_s32f_convert_32f_u_H
#define INCLUDED_volk_32i_s32f_convert_32f_u_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>

  /*!
    \brief Converts the input 32 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 32 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
    \note Output buffer does NOT need to be properly aligned
  */
static inline void volk_32i_s32f_convert_32f_u_sse2(float* outputVector, const int32_t* inputVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;
    
     float* outputVectorPtr = outputVector;
     const float iScalar = 1.0 / scalar;
    __m128 invScalar = _mm_set_ps1(iScalar);
    int32_t* inputPtr = (int32_t*)inputVector;
    __m128i inputVal;
    __m128 ret;

    for(;number < quarterPoints; number++){

      // Load the 4 values
      inputVal = _mm_loadu_si128((__m128i*)inputPtr);

      ret = _mm_cvtepi32_ps(inputVal);
      ret = _mm_mul_ps(ret, invScalar);

      _mm_storeu_ps(outputVectorPtr, ret);

      outputVectorPtr += 4;
      inputPtr += 4;
    }

    number = quarterPoints * 4;
    for(; number < num_points; number++){
      outputVector[number] =((float)(inputVector[number])) * iScalar;
    }
}
#endif /* LV_HAVE_SSE2 */


#ifdef LV_HAVE_GENERIC
  /*!
    \brief Converts the input 32 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 32 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
    \note Output buffer does NOT need to be properly aligned
  */
static inline void volk_32i_s32f_convert_32f_u_generic(float* outputVector, const int32_t* inputVector, const float scalar, unsigned int num_points){
  float* outputVectorPtr = outputVector;
  const int32_t* inputVectorPtr = inputVector;
  unsigned int number = 0;
  const float iScalar = 1.0 / scalar;

  for(number = 0; number < num_points; number++){
    *outputVectorPtr++ = ((float)(*inputVectorPtr++)) * iScalar;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32i_s32f_convert_32f_u_H */

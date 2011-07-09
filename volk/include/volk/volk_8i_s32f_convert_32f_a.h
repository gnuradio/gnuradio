#ifndef INCLUDED_volk_8i_s32f_convert_32f_a_H
#define INCLUDED_volk_8i_s32f_convert_32f_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>

  /*!
    \brief Converts the input 8 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 8 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_8i_s32f_convert_32f_a_sse4_1(float* outputVector, const int8_t* inputVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int sixteenthPoints = num_points / 16;
    
    float* outputVectorPtr = outputVector;
    const float iScalar = 1.0 / scalar;
    __m128 invScalar = _mm_set_ps1(iScalar);
    const int8_t* inputVectorPtr = inputVector;
    __m128 ret;
    __m128i inputVal;
    __m128i interimVal;

    for(;number < sixteenthPoints; number++){
      inputVal = _mm_load_si128((__m128i*)inputVectorPtr);

      interimVal = _mm_cvtepi8_epi32(inputVal);
      ret = _mm_cvtepi32_ps(interimVal);
      ret = _mm_mul_ps(ret, invScalar);
      _mm_store_ps(outputVectorPtr, ret);
      outputVectorPtr += 4;

      inputVal = _mm_srli_si128(inputVal, 4);
      interimVal = _mm_cvtepi8_epi32(inputVal);
      ret = _mm_cvtepi32_ps(interimVal);
      ret = _mm_mul_ps(ret, invScalar);
      _mm_store_ps(outputVectorPtr, ret);
      outputVectorPtr += 4;

      inputVal = _mm_srli_si128(inputVal, 4);
      interimVal = _mm_cvtepi8_epi32(inputVal);
      ret = _mm_cvtepi32_ps(interimVal);
      ret = _mm_mul_ps(ret, invScalar);
      _mm_store_ps(outputVectorPtr, ret);
      outputVectorPtr += 4;

      inputVal = _mm_srli_si128(inputVal, 4);
      interimVal = _mm_cvtepi8_epi32(inputVal);
      ret = _mm_cvtepi32_ps(interimVal);
      ret = _mm_mul_ps(ret, invScalar);
      _mm_store_ps(outputVectorPtr, ret);
      outputVectorPtr += 4;

      inputVectorPtr += 16;
    }

    number = sixteenthPoints * 16;
    for(; number < num_points; number++){
      outputVector[number] = (float)(inputVector[number]) * iScalar;
    }
}
#endif /* LV_HAVE_SSE4_1 */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Converts the input 8 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 8 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_8i_s32f_convert_32f_a_generic(float* outputVector, const int8_t* inputVector, const float scalar, unsigned int num_points){
  float* outputVectorPtr = outputVector;
  const int8_t* inputVectorPtr = inputVector;
  unsigned int number = 0;
  const float iScalar = 1.0 / scalar;

  for(number = 0; number < num_points; number++){
    *outputVectorPtr++ = ((float)(*inputVectorPtr++)) * iScalar;
  }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_ORC
  /*!
    \brief Converts the input 8 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 8 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
  */
extern void volk_8i_s32f_convert_32f_a_orc_impl(float* outputVector, const int8_t* inputVector, const float scalar, unsigned int num_points);
static inline void volk_8i_s32f_convert_32f_a_orc(float* outputVector, const int8_t* inputVector, const float scalar, unsigned int num_points){
    float invscalar = 1.0 / scalar;
    volk_8i_s32f_convert_32f_a_orc_impl(outputVector, inputVector, invscalar, num_points);
}
#endif /* LV_HAVE_ORC */



#endif /* INCLUDED_VOLK_8s_CONVERT_32f_ALIGNED8_H */

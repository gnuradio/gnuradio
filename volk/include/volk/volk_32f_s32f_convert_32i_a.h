#ifndef INCLUDED_volk_32f_s32f_convert_32i_a_H
#define INCLUDED_volk_32f_s32f_convert_32i_a_H

#include <volk/volk_common.h>
#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_AVX
#include <immintrin.h>
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 32 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 32 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_32f_s32f_convert_32i_a_avx(int32_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;

  const unsigned int eighthPoints = num_points / 8;
    
  const float* inputVectorPtr = (const float*)inputVector;
  int32_t* outputVectorPtr = outputVector;
  __m256 vScalar = _mm256_set1_ps(scalar);
  __m256 inputVal1;
  __m256i intInputVal1;

  for(;number < eighthPoints; number++){
    inputVal1 = _mm256_load_ps(inputVectorPtr); inputVectorPtr += 8;

    intInputVal1 = _mm256_cvtps_epi32(_mm256_mul_ps(inputVal1, vScalar));

    _mm256_store_si256((__m256i*)outputVectorPtr, intInputVal1);
    outputVectorPtr += 8;
  }

  number = eighthPoints * 8;    
  for(; number < num_points; number++){
    outputVector[number] = (int32_t)(inputVector[number] * scalar);
  }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 32 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 32 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_32f_s32f_convert_32i_a_sse2(int32_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;

  const unsigned int quarterPoints = num_points / 4;
    
  const float* inputVectorPtr = (const float*)inputVector;
  int32_t* outputVectorPtr = outputVector;
  __m128 vScalar = _mm_set_ps1(scalar);
  __m128 inputVal1;
  __m128i intInputVal1;

  for(;number < quarterPoints; number++){
    inputVal1 = _mm_load_ps(inputVectorPtr); inputVectorPtr += 4;

    intInputVal1 = _mm_cvtps_epi32(_mm_mul_ps(inputVal1, vScalar));

    _mm_store_si128((__m128i*)outputVectorPtr, intInputVal1);
    outputVectorPtr += 4;
  }

  number = quarterPoints * 4;    
  for(; number < num_points; number++){
    outputVector[number] = (int32_t)(inputVector[number] * scalar);
  }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 32 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 32 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_32f_s32f_convert_32i_a_sse(int32_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;

  const unsigned int quarterPoints = num_points / 4;
    
  const float* inputVectorPtr = (const float*)inputVector;
  int32_t* outputVectorPtr = outputVector;
  __m128 vScalar = _mm_set_ps1(scalar);
  __m128 ret;

  __VOLK_ATTR_ALIGNED(16) float outputFloatBuffer[4];

  for(;number < quarterPoints; number++){
    ret = _mm_load_ps(inputVectorPtr);
    inputVectorPtr += 4;

    ret = _mm_mul_ps(ret, vScalar);

    _mm_store_ps(outputFloatBuffer, ret);
    *outputVectorPtr++ = (int32_t)(outputFloatBuffer[0]);
    *outputVectorPtr++ = (int32_t)(outputFloatBuffer[1]);
    *outputVectorPtr++ = (int32_t)(outputFloatBuffer[2]);
    *outputVectorPtr++ = (int32_t)(outputFloatBuffer[3]);
  }

  number = quarterPoints * 4;    
  for(; number < num_points; number++){
    outputVector[number] = (int32_t)(inputVector[number] * scalar);
  }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 32 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 32 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_32f_s32f_convert_32i_a_generic(int32_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  int32_t* outputVectorPtr = outputVector;
  const float* inputVectorPtr = inputVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *outputVectorPtr++ = ((int32_t)(*inputVectorPtr++  * scalar));
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32f_s32f_convert_32i_a_H */

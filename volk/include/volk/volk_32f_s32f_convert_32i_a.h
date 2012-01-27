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

  float min_val = -2147483647;
  float max_val = 2147483647;
  float r;

  __m256 vScalar = _mm256_set1_ps(scalar);
  __m256 inputVal1;
  __m256i intInputVal1;
  __m256 vmin_val = _mm256_set1_ps(min_val);
  __m256 vmax_val = _mm256_set1_ps(max_val);

  for(;number < eighthPoints; number++){
    inputVal1 = _mm256_load_ps(inputVectorPtr); inputVectorPtr += 8;

    inputVal1 = _mm256_max_ps(_mm256_min_ps(_mm256_mul_ps(inputVal1, vScalar), vmax_val), vmin_val);
    intInputVal1 = _mm256_cvtps_epi32(inputVal1);

    _mm256_store_si256((__m256i*)outputVectorPtr, intInputVal1);
    outputVectorPtr += 8;
  }

  number = eighthPoints * 8;    
  for(; number < num_points; number++){
    r = inputVector[number] * scalar;
    if(r > max_val)
      r = max_val;
    else if(r < min_val)
      r = min_val;
    outputVector[number] = (int32_t)(r);
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

  float min_val = -2147483647;
  float max_val = 2147483647;
  float r;

  __m128 vScalar = _mm_set_ps1(scalar);
  __m128 inputVal1;
  __m128i intInputVal1;
  __m128 vmin_val = _mm_set_ps1(min_val);
  __m128 vmax_val = _mm_set_ps1(max_val);

  for(;number < quarterPoints; number++){
    inputVal1 = _mm_load_ps(inputVectorPtr); inputVectorPtr += 4;

    inputVal1 = _mm_max_ps(_mm_min_ps(_mm_mul_ps(inputVal1, vScalar), vmax_val), vmin_val);
    intInputVal1 = _mm_cvtps_epi32(inputVal1);

    _mm_store_si128((__m128i*)outputVectorPtr, intInputVal1);
    outputVectorPtr += 4;
  }

  number = quarterPoints * 4;    
  for(; number < num_points; number++){
    r = inputVector[number] * scalar;
    if(r > max_val)
      r = max_val;
    else if(r < min_val)
      r = min_val;
    outputVector[number] = (int32_t)(r);
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

  float min_val = -2147483647;
  float max_val = 2147483647;
  float r;

  __m128 vScalar = _mm_set_ps1(scalar);
  __m128 ret;
  __m128 vmin_val = _mm_set_ps1(min_val);
  __m128 vmax_val = _mm_set_ps1(max_val);

  __VOLK_ATTR_ALIGNED(16) float outputFloatBuffer[4];

  for(;number < quarterPoints; number++){
    ret = _mm_load_ps(inputVectorPtr);
    inputVectorPtr += 4;

    ret = _mm_max_ps(_mm_min_ps(_mm_mul_ps(ret, vScalar), vmax_val), vmin_val);

    _mm_store_ps(outputFloatBuffer, ret);
    *outputVectorPtr++ = (int32_t)(outputFloatBuffer[0]);
    *outputVectorPtr++ = (int32_t)(outputFloatBuffer[1]);
    *outputVectorPtr++ = (int32_t)(outputFloatBuffer[2]);
    *outputVectorPtr++ = (int32_t)(outputFloatBuffer[3]);
  }

  number = quarterPoints * 4;    
  for(; number < num_points; number++){
    r = inputVector[number] * scalar;
    if(r > max_val)
      r = max_val;
    else if(r < min_val)
      r = min_val;
    outputVector[number] = (int32_t)(r);
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
  float min_val = -2147483647;
  float max_val = 2147483647;
  float r;

  for(number = 0; number < num_points; number++){
    r = *inputVectorPtr++ * scalar;
    if(r > max_val)
      r = max_val;
    else if(r < min_val)
      r = min_val;
    *outputVectorPtr++ = (int32_t)(r);
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32f_s32f_convert_32i_a_H */

#ifndef INCLUDED_VOLK_32f_CONVERT_16s_UNALIGNED16_H
#define INCLUDED_VOLK_32f_CONVERT_16s_UNALIGNED16_H

#include <inttypes.h>
#include <stdio.h>

#if LV_HAVE_SSE2
#include <emmintrin.h>
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 16 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 16 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
    \note Input buffer does NOT need to be properly aligned
  */
static inline void volk_32f_convert_16s_unaligned16_sse2(int16_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;

  const unsigned int eighthPoints = num_points / 8;
    
  const float* inputVectorPtr = (const float*)inputVector;
  int16_t* outputVectorPtr = outputVector;
  __m128 vScalar = _mm_set_ps1(scalar);
  __m128 inputVal1, inputVal2;
  __m128i intInputVal1, intInputVal2;

  for(;number < eighthPoints; number++){
    inputVal1 = _mm_loadu_ps(inputVectorPtr); inputVectorPtr += 4;
    inputVal2 = _mm_loadu_ps(inputVectorPtr); inputVectorPtr += 4;

    intInputVal1 = _mm_cvtps_epi32(_mm_mul_ps(inputVal1, vScalar));
    intInputVal2 = _mm_cvtps_epi32(_mm_mul_ps(inputVal2, vScalar));
    
    intInputVal1 = _mm_packs_epi32(intInputVal1, intInputVal2);

    _mm_storeu_si128((__m128i*)outputVectorPtr, intInputVal1);
    outputVectorPtr += 8;
  }

  number = eighthPoints * 8;    
  for(; number < num_points; number++){
    outputVector[number] = (int16_t)(inputVector[number] * scalar);
  }
}
#endif /* LV_HAVE_SSE2 */

#if LV_HAVE_SSE
#include <xmmintrin.h>
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 16 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 16 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
    \note Input buffer does NOT need to be properly aligned
  */
static inline void volk_32f_convert_16s_unaligned16_sse(int16_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;

  const unsigned int quarterPoints = num_points / 4;
    
  const float* inputVectorPtr = (const float*)inputVector;
  int16_t* outputVectorPtr = outputVector;
  __m128 vScalar = _mm_set_ps1(scalar);
  __m128 ret;

  float outputFloatBuffer[4] __attribute__((aligned(128)));

  for(;number < quarterPoints; number++){
    ret = _mm_loadu_ps(inputVectorPtr);
    inputVectorPtr += 4;

    ret = _mm_mul_ps(ret, vScalar);

    _mm_store_ps(outputFloatBuffer, ret);
    *outputVectorPtr++ = (int16_t)(outputFloatBuffer[0]);
    *outputVectorPtr++ = (int16_t)(outputFloatBuffer[1]);
    *outputVectorPtr++ = (int16_t)(outputFloatBuffer[2]);
    *outputVectorPtr++ = (int16_t)(outputFloatBuffer[3]);
  }

  number = quarterPoints * 4;    
  for(; number < num_points; number++){
    outputVector[number] = (int16_t)(inputVector[number] * scalar);
  }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 16 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 16 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
    \note Input buffer does NOT need to be properly aligned
  */
static inline void volk_32f_convert_16s_unaligned16_generic(int16_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  int16_t* outputVectorPtr = outputVector;
  const float* inputVectorPtr = inputVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *outputVectorPtr++ = ((int16_t)(*inputVectorPtr++  * scalar));
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_VOLK_32f_CONVERT_16s_UNALIGNED16_H */

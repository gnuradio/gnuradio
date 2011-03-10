#ifndef INCLUDED_volk_32f_s32f_convert_8i_a16_H
#define INCLUDED_volk_32f_s32f_convert_8i_a16_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 8 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 8 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_32f_s32f_convert_8i_a16_sse2(int8_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;

  const unsigned int sixteenthPoints = num_points / 16;
    
  const float* inputVectorPtr = (const float*)inputVector;
  int8_t* outputVectorPtr = outputVector;
  __m128 vScalar = _mm_set_ps1(scalar);
  __m128 inputVal1, inputVal2, inputVal3, inputVal4;
  __m128i intInputVal1, intInputVal2, intInputVal3, intInputVal4;

  for(;number < sixteenthPoints; number++){
    inputVal1 = _mm_load_ps(inputVectorPtr); inputVectorPtr += 4;
    inputVal2 = _mm_load_ps(inputVectorPtr); inputVectorPtr += 4;
    inputVal3 = _mm_load_ps(inputVectorPtr); inputVectorPtr += 4;
    inputVal4 = _mm_load_ps(inputVectorPtr); inputVectorPtr += 4;

    intInputVal1 = _mm_cvtps_epi32(_mm_mul_ps(inputVal1, vScalar));
    intInputVal2 = _mm_cvtps_epi32(_mm_mul_ps(inputVal2, vScalar));
    intInputVal3 = _mm_cvtps_epi32(_mm_mul_ps(inputVal3, vScalar));
    intInputVal4 = _mm_cvtps_epi32(_mm_mul_ps(inputVal4, vScalar));
    
    intInputVal1 = _mm_packs_epi32(intInputVal1, intInputVal2);
    intInputVal3 = _mm_packs_epi32(intInputVal3, intInputVal4);

    intInputVal1 = _mm_packs_epi16(intInputVal1, intInputVal3);

    _mm_store_si128((__m128i*)outputVectorPtr, intInputVal1);
    outputVectorPtr += 16;
  }

  number = sixteenthPoints * 16;    
  for(; number < num_points; number++){
    outputVector[number] = (int8_t)(inputVector[number] * scalar);
  }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 8 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 8 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_32f_s32f_convert_8i_a16_sse(int8_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;

  const unsigned int quarterPoints = num_points / 4;
    
  const float* inputVectorPtr = (const float*)inputVector;
  int8_t* outputVectorPtr = outputVector;
  __m128 vScalar = _mm_set_ps1(scalar);
  __m128 ret;

  float outputFloatBuffer[4] __attribute__((aligned(128)));

  for(;number < quarterPoints; number++){
    ret = _mm_load_ps(inputVectorPtr);
    inputVectorPtr += 4;

    ret = _mm_mul_ps(ret, vScalar);

    _mm_store_ps(outputFloatBuffer, ret);
    *outputVectorPtr++ = (int8_t)(outputFloatBuffer[0]);
    *outputVectorPtr++ = (int8_t)(outputFloatBuffer[1]);
    *outputVectorPtr++ = (int8_t)(outputFloatBuffer[2]);
    *outputVectorPtr++ = (int8_t)(outputFloatBuffer[3]);
  }

  number = quarterPoints * 4;    
  for(; number < num_points; number++){
    outputVector[number] = (int8_t)(inputVector[number] * scalar);
  }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 8 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 8 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_32f_s32f_convert_8i_a16_generic(int8_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  int8_t* outputVectorPtr = outputVector;
  const float* inputVectorPtr = inputVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *outputVectorPtr++ = (int8_t)(*inputVectorPtr++  * scalar);
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32f_s32f_convert_8i_a16_H */

#ifndef INCLUDED_volk_8i_convert_16i_u_H
#define INCLUDED_volk_8i_convert_16i_u_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>

  /*!
    \brief Converts the input 8 bit integer data into 16 bit integer data
    \param inputVector The 8 bit input data buffer
    \param outputVector The 16 bit output data buffer
    \param num_points The number of data values to be converted
    \note Input and output buffers do NOT need to be properly aligned
  */
static inline void volk_8i_convert_16i_u_sse4_1(int16_t* outputVector, const int8_t* inputVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int sixteenthPoints = num_points / 16;

    const __m128i* inputVectorPtr = (const __m128i*)inputVector;
    __m128i* outputVectorPtr = (__m128i*)outputVector;
    __m128i inputVal;
    __m128i ret;

    for(;number < sixteenthPoints; number++){
      inputVal = _mm_loadu_si128(inputVectorPtr);
      ret = _mm_cvtepi8_epi16(inputVal);
      ret = _mm_slli_epi16(ret, 8); // Multiply by 256
      _mm_storeu_si128(outputVectorPtr, ret);

      outputVectorPtr++;

      inputVal = _mm_srli_si128(inputVal, 8);
      ret = _mm_cvtepi8_epi16(inputVal);
      ret = _mm_slli_epi16(ret, 8); // Multiply by 256
      _mm_storeu_si128(outputVectorPtr, ret);

      outputVectorPtr++;

      inputVectorPtr++;
    }

    number = sixteenthPoints * 16;
    for(; number < num_points; number++){
      outputVector[number] = (int16_t)(inputVector[number])*256;
    }
}
#endif /* LV_HAVE_SSE4_1 */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Converts the input 8 bit integer data into 16 bit integer data
    \param inputVector The 8 bit input data buffer
    \param outputVector The 16 bit output data buffer
    \param num_points The number of data values to be converted
    \note Input and output buffers do NOT need to be properly aligned
  */
static inline void volk_8i_convert_16i_u_generic(int16_t* outputVector, const int8_t* inputVector, unsigned int num_points){
  int16_t* outputVectorPtr = outputVector;
  const int8_t* inputVectorPtr = inputVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *outputVectorPtr++ = ((int16_t)(*inputVectorPtr++)) * 256;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_VOLK_8s_CONVERT_16s_UNALIGNED8_H */

#ifndef INCLUDED_volk_16i_convert_8i_a_H
#define INCLUDED_volk_16i_convert_8i_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
/*!
  \brief Converts the input 16 bit integer data into 8 bit integer data
  \param inputVector The 16 bit input data buffer
  \param outputVector The 8 bit output data buffer
  \param num_points The number of data values to be converted
*/
static inline void volk_16i_convert_8i_a_sse2(int8_t* outputVector, const int16_t* inputVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int sixteenthPoints = num_points / 16;
    
     int8_t* outputVectorPtr = outputVector;
    int16_t* inputPtr = (int16_t*)inputVector;
    __m128i inputVal1;
    __m128i inputVal2;
    __m128i ret;

    for(;number < sixteenthPoints; number++){

      // Load the 16 values
      inputVal1 = _mm_load_si128((__m128i*)inputPtr); inputPtr += 8;
      inputVal2 = _mm_load_si128((__m128i*)inputPtr); inputPtr += 8;

      inputVal1 = _mm_srai_epi16(inputVal1, 8);
      inputVal2 = _mm_srai_epi16(inputVal2, 8);
      
      ret = _mm_packs_epi16(inputVal1, inputVal2);

      _mm_store_si128((__m128i*)outputVectorPtr, ret);

      outputVectorPtr += 16;
    }

    number = sixteenthPoints * 16;
    for(; number < num_points; number++){
      outputVector[number] =(int8_t)(inputVector[number] >> 8);
    }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Converts the input 16 bit integer data into 8 bit integer data
  \param inputVector The 16 bit input data buffer
  \param outputVector The 8 bit output data buffer
  \param num_points The number of data values to be converted
*/
static inline void volk_16i_convert_8i_a_generic(int8_t* outputVector, const int16_t* inputVector, unsigned int num_points){
  int8_t* outputVectorPtr = outputVector;
  const int16_t* inputVectorPtr = inputVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *outputVectorPtr++ = ((int8_t)(*inputVectorPtr++ >> 8));
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_16i_convert_8i_a_H */

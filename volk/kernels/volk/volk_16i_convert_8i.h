/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_volk_16i_convert_8i_u_H
#define INCLUDED_volk_16i_convert_8i_u_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
/*!
  \brief Converts the input 16 bit integer data into 8 bit integer data
  \param inputVector The 16 bit input data buffer
  \param outputVector The 8 bit output data buffer
  \param num_points The number of data values to be converted
  \note Input and output buffers do NOT need to be properly aligned
*/
static inline void volk_16i_convert_8i_u_sse2(int8_t* outputVector, const int16_t* inputVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int sixteenthPoints = num_points / 16;

     int8_t* outputVectorPtr = outputVector;
    int16_t* inputPtr = (int16_t*)inputVector;
    __m128i inputVal1;
    __m128i inputVal2;
    __m128i ret;

    for(;number < sixteenthPoints; number++){

      // Load the 16 values
      inputVal1 = _mm_loadu_si128((__m128i*)inputPtr); inputPtr += 8;
      inputVal2 = _mm_loadu_si128((__m128i*)inputPtr); inputPtr += 8;

      inputVal1 = _mm_srai_epi16(inputVal1, 8);
      inputVal2 = _mm_srai_epi16(inputVal2, 8);

      ret = _mm_packs_epi16(inputVal1, inputVal2);

      _mm_storeu_si128((__m128i*)outputVectorPtr, ret);

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
  \note Input and output buffers do NOT need to be properly aligned
*/
static inline void volk_16i_convert_8i_generic(int8_t* outputVector, const int16_t* inputVector, unsigned int num_points){
  int8_t* outputVectorPtr = outputVector;
  const int16_t* inputVectorPtr = inputVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *outputVectorPtr++ = ((int8_t)(*inputVectorPtr++  >> 8));
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_16i_convert_8i_u_H */
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

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
/*!
  \brief Converts the input 16 bit integer data into 8 bit integer data
  \param inputVector The 16 bit input data buffer
  \param outputVector The 8 bit output data buffer
  \param num_points The number of data values to be converted
*/
static inline void volk_16i_convert_8i_neon(int8_t* outputVector, const int16_t* inputVector, unsigned int num_points){
  int8_t* outputVectorPtr = outputVector;
  const int16_t* inputVectorPtr = inputVector;
  unsigned int number = 0;
  unsigned int sixteenth_points = num_points / 16;

  int16x8_t inputVal0;
  int16x8_t inputVal1;
  int8x8_t outputVal0;
  int8x8_t outputVal1;
  int8x16_t outputVal;
  
  for(number = 0; number < sixteenth_points; number++){
    // load two input vectors
    inputVal0 = vld1q_s16(inputVectorPtr);
    inputVal1 = vld1q_s16(inputVectorPtr+8);
    // shift right
    outputVal0 = vshrn_n_s16(inputVal0, 8);
    outputVal1 = vshrn_n_s16(inputVal1, 8);
    // squash two vectors and write output
    outputVal = vcombine_s8(outputVal0, outputVal1);
    vst1q_s8(outputVectorPtr, outputVal);
    inputVectorPtr += 16;
    outputVectorPtr += 16;
  }

  for(number = sixteenth_points * 16; number < num_points; number++){
    *outputVectorPtr++ = ((int8_t)(*inputVectorPtr++ >> 8));
  }
}
#endif /* LV_HAVE_NEON */

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

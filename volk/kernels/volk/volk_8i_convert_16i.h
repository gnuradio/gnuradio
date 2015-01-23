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
static inline void volk_8i_convert_16i_generic(int16_t* outputVector, const int8_t* inputVector, unsigned int num_points){
  int16_t* outputVectorPtr = outputVector;
  const int8_t* inputVectorPtr = inputVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *outputVectorPtr++ = ((int16_t)(*inputVectorPtr++)) * 256;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_VOLK_8s_CONVERT_16s_UNALIGNED8_H */
#ifndef INCLUDED_volk_8i_convert_16i_a_H
#define INCLUDED_volk_8i_convert_16i_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>

  /*!
    \brief Converts the input 8 bit integer data into 16 bit integer data
    \param inputVector The 8 bit input data buffer
    \param outputVector The 16 bit output data buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_8i_convert_16i_a_sse4_1(int16_t* outputVector, const int8_t* inputVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int sixteenthPoints = num_points / 16;

    const __m128i* inputVectorPtr = (const __m128i*)inputVector;
    __m128i* outputVectorPtr = (__m128i*)outputVector;
    __m128i inputVal;
    __m128i ret;

    for(;number < sixteenthPoints; number++){
      inputVal = _mm_load_si128(inputVectorPtr);
      ret = _mm_cvtepi8_epi16(inputVal);
      ret = _mm_slli_epi16(ret, 8); // Multiply by 256
      _mm_store_si128(outputVectorPtr, ret);

      outputVectorPtr++;

      inputVal = _mm_srli_si128(inputVal, 8);
      ret = _mm_cvtepi8_epi16(inputVal);
      ret = _mm_slli_epi16(ret, 8); // Multiply by 256
      _mm_store_si128(outputVectorPtr, ret);

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
  */
static inline void volk_8i_convert_16i_a_generic(int16_t* outputVector, const int8_t* inputVector, unsigned int num_points){
  int16_t* outputVectorPtr = outputVector;
  const int8_t* inputVectorPtr = inputVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *outputVectorPtr++ = ((int16_t)(*inputVectorPtr++)) * 256;
  }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_NEON
#include <arm_neon.h>

  /*!
    \brief Converts the input 8 bit integer data into 16 bit integer data
    \param inputVector The 8 bit input data buffer
    \param outputVector The 16 bit output data buffer
    \param num_points The number of data values to be converted
    \note Input and output buffers do NOT need to be properly aligned
  */
static inline void volk_8i_convert_16i_neon(int16_t* outputVector, const int8_t* inputVector, unsigned int num_points){
    int16_t* outputVectorPtr = outputVector;
    const int8_t* inputVectorPtr = inputVector;
    unsigned int number;
    const unsigned int eighth_points = num_points / 8;

    int8x8_t input_vec ;
    int16x8_t converted_vec;

    // NEON doesn't have a concept of 8 bit registers, so we are really
    // dealing with the low half of 16-bit registers. Since this requires
    // a move instruction we likely do better with ASM here.
    for(number = 0; number < eighth_points; ++number) {
        input_vec = vld1_s8(inputVectorPtr);
        converted_vec = vmovl_s8(input_vec);
        //converted_vec = vmulq_s16(converted_vec, scale_factor);
        converted_vec = vshlq_n_s16(converted_vec, 8);
        vst1q_s16( outputVectorPtr, converted_vec);

        inputVectorPtr += 8;
        outputVectorPtr += 8;
    }

    for(number = eighth_points * 8; number < num_points; number++){
      *outputVectorPtr++ = ((int16_t)(*inputVectorPtr++)) * 256;
    }
}
#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_ORC
  /*!
    \brief Converts the input 8 bit integer data into 16 bit integer data
    \param inputVector The 8 bit input data buffer
    \param outputVector The 16 bit output data buffer
    \param num_points The number of data values to be converted
  */
extern void volk_8i_convert_16i_a_orc_impl(int16_t* outputVector, const int8_t* inputVector, unsigned int num_points);
static inline void volk_8i_convert_16i_u_orc(int16_t* outputVector, const int8_t* inputVector, unsigned int num_points){
    volk_8i_convert_16i_a_orc_impl(outputVector, inputVector, num_points);
}
#endif /* LV_HAVE_ORC */



#endif /* INCLUDED_VOLK_8s_CONVERT_16s_ALIGNED8_H */

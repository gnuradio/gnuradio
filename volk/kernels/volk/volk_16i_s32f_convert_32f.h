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

#ifndef INCLUDED_volk_16i_s32f_convert_32f_u_H
#define INCLUDED_volk_16i_s32f_convert_32f_u_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_AVX
#include <immintrin.h>

  /*!
    \brief Converts the input 16 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 16 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_16i_s32f_convert_32f_u_avx(float* outputVector, const int16_t* inputVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int eighthPoints = num_points / 8;

     float* outputVectorPtr = outputVector;
    __m128 invScalar = _mm_set_ps1(1.0/scalar);
    int16_t* inputPtr = (int16_t*)inputVector;
    __m128i inputVal, inputVal2;
    __m128 ret;
    __m256 output;
    __m256 dummy = _mm256_setzero_ps();

    for(;number < eighthPoints; number++){

      // Load the 8 values
      //inputVal = _mm_loadu_si128((__m128i*)inputPtr);
      inputVal = _mm_loadu_si128((__m128i*)inputPtr);

      // Shift the input data to the right by 64 bits ( 8 bytes )
      inputVal2 = _mm_srli_si128(inputVal, 8);

      // Convert the lower 4 values into 32 bit words
      inputVal = _mm_cvtepi16_epi32(inputVal);
      inputVal2 = _mm_cvtepi16_epi32(inputVal2);

      ret = _mm_cvtepi32_ps(inputVal);
      ret = _mm_mul_ps(ret, invScalar);
      output = _mm256_insertf128_ps(dummy, ret, 0);

      ret = _mm_cvtepi32_ps(inputVal2);
      ret = _mm_mul_ps(ret, invScalar);
      output = _mm256_insertf128_ps(output, ret, 1);

      _mm256_storeu_ps(outputVectorPtr, output);

      outputVectorPtr += 8;

      inputPtr += 8;
    }

    number = eighthPoints * 8;
    for(; number < num_points; number++){
      outputVector[number] =((float)(inputVector[number])) / scalar;
    }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>

  /*!
    \brief Converts the input 16 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 16 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
    \note Output buffer does NOT need to be properly aligned
  */
static inline void volk_16i_s32f_convert_32f_u_sse4_1(float* outputVector, const int16_t* inputVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int eighthPoints = num_points / 8;

     float* outputVectorPtr = outputVector;
    __m128 invScalar = _mm_set_ps1(1.0/scalar);
    int16_t* inputPtr = (int16_t*)inputVector;
    __m128i inputVal;
    __m128i inputVal2;
    __m128 ret;

    for(;number < eighthPoints; number++){

      // Load the 8 values
      inputVal = _mm_loadu_si128((__m128i*)inputPtr);

      // Shift the input data to the right by 64 bits ( 8 bytes )
      inputVal2 = _mm_srli_si128(inputVal, 8);

      // Convert the lower 4 values into 32 bit words
      inputVal = _mm_cvtepi16_epi32(inputVal);
      inputVal2 = _mm_cvtepi16_epi32(inputVal2);

      ret = _mm_cvtepi32_ps(inputVal);
      ret = _mm_mul_ps(ret, invScalar);
      _mm_storeu_ps(outputVectorPtr, ret);
      outputVectorPtr += 4;

      ret = _mm_cvtepi32_ps(inputVal2);
      ret = _mm_mul_ps(ret, invScalar);
      _mm_storeu_ps(outputVectorPtr, ret);

      outputVectorPtr += 4;

      inputPtr += 8;
    }

    number = eighthPoints * 8;
    for(; number < num_points; number++){
      outputVector[number] =((float)(inputVector[number])) / scalar;
    }
}
#endif /* LV_HAVE_SSE4_1 */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>

  /*!
    \brief Converts the input 16 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 16 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
    \note Output buffer does NOT need to be properly aligned
  */
static inline void volk_16i_s32f_convert_32f_u_sse(float* outputVector, const int16_t* inputVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    float* outputVectorPtr = outputVector;
    __m128 invScalar = _mm_set_ps1(1.0/scalar);
    int16_t* inputPtr = (int16_t*)inputVector;
    __m128 ret;

    for(;number < quarterPoints; number++){
      ret = _mm_set_ps((float)(inputPtr[3]), (float)(inputPtr[2]), (float)(inputPtr[1]), (float)(inputPtr[0]));

      ret = _mm_mul_ps(ret, invScalar);
      _mm_storeu_ps(outputVectorPtr, ret);

      inputPtr += 4;
      outputVectorPtr += 4;
    }

    number = quarterPoints * 4;
    for(; number < num_points; number++){
      outputVector[number] = (float)(inputVector[number]) / scalar;
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Converts the input 16 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 16 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
    \note Output buffer does NOT need to be properly aligned
  */
static inline void volk_16i_s32f_convert_32f_generic(float* outputVector, const int16_t* inputVector, const float scalar, unsigned int num_points){
  float* outputVectorPtr = outputVector;
  const int16_t* inputVectorPtr = inputVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *outputVectorPtr++ = ((float)(*inputVectorPtr++)) / scalar;
  }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_NEON
  /*!
    \brief Converts the input 16 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 16 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
    \note Output buffer does NOT need to be properly aligned
  */
static inline void volk_16i_s32f_convert_32f_neon(float* outputVector, const int16_t* inputVector, const float scalar, unsigned int num_points){
  float* outputPtr = outputVector;
  const int16_t* inputPtr = inputVector;
  unsigned int number = 0;
  unsigned int eighth_points = num_points / 8;

  int16x4x2_t input16;
  int32x4_t input32_0, input32_1;
  float32x4_t input_float_0, input_float_1;
  float32x4x2_t output_float;
  float32x4_t inv_scale;

  inv_scale = vdupq_n_f32(1.0/scalar);

  // the generic disassembles to a 128-bit load
  // and duplicates every instruction to operate on 64-bits
  // at a time. This is only possible with lanes, which is faster
  // than just doing a vld1_s16, but still slower.
  for(number = 0; number < eighth_points; number++){
    input16 = vld2_s16(inputPtr);
    // widen 16-bit int to 32-bit int
    input32_0 = vmovl_s16(input16.val[0]);
    input32_1 = vmovl_s16(input16.val[1]);
    // convert 32-bit int to float with scale
    input_float_0 = vcvtq_f32_s32(input32_0);
    input_float_1 = vcvtq_f32_s32(input32_1);
    output_float.val[0] = vmulq_f32(input_float_0, inv_scale);
    output_float.val[1] = vmulq_f32(input_float_1, inv_scale);
    vst2q_f32(outputPtr, output_float);
    inputPtr += 8;
    outputPtr += 8;
  }

  for(number = eighth_points*8; number < num_points; number++){
    *outputPtr++ = ((float)(*inputPtr++)) / scalar;
  }
}
#endif /* LV_HAVE_NEON */


#endif /* INCLUDED_volk_16i_s32f_convert_32f_u_H */
#ifndef INCLUDED_volk_16i_s32f_convert_32f_a_H
#define INCLUDED_volk_16i_s32f_convert_32f_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_AVX
#include <immintrin.h>

  /*!
    \brief Converts the input 16 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 16 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_16i_s32f_convert_32f_a_avx(float* outputVector, const int16_t* inputVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int eighthPoints = num_points / 8;

     float* outputVectorPtr = outputVector;
    __m128 invScalar = _mm_set_ps1(1.0/scalar);
    int16_t* inputPtr = (int16_t*)inputVector;
    __m128i inputVal, inputVal2;
    __m128 ret;
    __m256 output;
    __m256 dummy = _mm256_setzero_ps();

    for(;number < eighthPoints; number++){

      // Load the 8 values
      //inputVal = _mm_loadu_si128((__m128i*)inputPtr);
      inputVal = _mm_load_si128((__m128i*)inputPtr);

      // Shift the input data to the right by 64 bits ( 8 bytes )
      inputVal2 = _mm_srli_si128(inputVal, 8);

      // Convert the lower 4 values into 32 bit words
      inputVal = _mm_cvtepi16_epi32(inputVal);
      inputVal2 = _mm_cvtepi16_epi32(inputVal2);

      ret = _mm_cvtepi32_ps(inputVal);
      ret = _mm_mul_ps(ret, invScalar);
      output = _mm256_insertf128_ps(dummy, ret, 0);

      ret = _mm_cvtepi32_ps(inputVal2);
      ret = _mm_mul_ps(ret, invScalar);
      output = _mm256_insertf128_ps(output, ret, 1);

      _mm256_store_ps(outputVectorPtr, output);

      outputVectorPtr += 8;

      inputPtr += 8;
    }

    number = eighthPoints * 8;
    for(; number < num_points; number++){
      outputVector[number] =((float)(inputVector[number])) / scalar;
    }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>

  /*!
    \brief Converts the input 16 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 16 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_16i_s32f_convert_32f_a_sse4_1(float* outputVector, const int16_t* inputVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int eighthPoints = num_points / 8;

     float* outputVectorPtr = outputVector;
    __m128 invScalar = _mm_set_ps1(1.0/scalar);
    int16_t* inputPtr = (int16_t*)inputVector;
    __m128i inputVal;
    __m128i inputVal2;
    __m128 ret;

    for(;number < eighthPoints; number++){

      // Load the 8 values
      inputVal = _mm_loadu_si128((__m128i*)inputPtr);

      // Shift the input data to the right by 64 bits ( 8 bytes )
      inputVal2 = _mm_srli_si128(inputVal, 8);

      // Convert the lower 4 values into 32 bit words
      inputVal = _mm_cvtepi16_epi32(inputVal);
      inputVal2 = _mm_cvtepi16_epi32(inputVal2);

      ret = _mm_cvtepi32_ps(inputVal);
      ret = _mm_mul_ps(ret, invScalar);
      _mm_storeu_ps(outputVectorPtr, ret);
      outputVectorPtr += 4;

      ret = _mm_cvtepi32_ps(inputVal2);
      ret = _mm_mul_ps(ret, invScalar);
      _mm_storeu_ps(outputVectorPtr, ret);

      outputVectorPtr += 4;

      inputPtr += 8;
    }

    number = eighthPoints * 8;
    for(; number < num_points; number++){
      outputVector[number] =((float)(inputVector[number])) / scalar;
    }
}
#endif /* LV_HAVE_SSE4_1 */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>

  /*!
    \brief Converts the input 16 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 16 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_16i_s32f_convert_32f_a_sse(float* outputVector, const int16_t* inputVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    float* outputVectorPtr = outputVector;
    __m128 invScalar = _mm_set_ps1(1.0/scalar);
    int16_t* inputPtr = (int16_t*)inputVector;
    __m128 ret;

    for(;number < quarterPoints; number++){
      ret = _mm_set_ps((float)(inputPtr[3]), (float)(inputPtr[2]), (float)(inputPtr[1]), (float)(inputPtr[0]));

      ret = _mm_mul_ps(ret, invScalar);
      _mm_storeu_ps(outputVectorPtr, ret);

      inputPtr += 4;
      outputVectorPtr += 4;
    }

    number = quarterPoints * 4;
    for(; number < num_points; number++){
      outputVector[number] = (float)(inputVector[number]) / scalar;
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Converts the input 16 bit integer data into floating point data, and divides the each floating point output data point by the scalar value
    \param inputVector The 16 bit input data buffer
    \param outputVector The floating point output data buffer
    \param scalar The value divided against each point in the output buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_16i_s32f_convert_32f_a_generic(float* outputVector, const int16_t* inputVector, const float scalar, unsigned int num_points){
  float* outputVectorPtr = outputVector;
  const int16_t* inputVectorPtr = inputVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *outputVectorPtr++ = ((float)(*inputVectorPtr++)) / scalar;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_16i_s32f_convert_32f_a_H */

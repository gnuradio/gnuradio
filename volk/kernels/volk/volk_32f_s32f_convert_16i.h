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

#ifndef INCLUDED_volk_32f_s32f_convert_16i_u_H
#define INCLUDED_volk_32f_s32f_convert_16i_u_H

#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#ifdef LV_HAVE_AVX
#include <immintrin.h>
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 16 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 16 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_32f_s32f_convert_16i_u_avx(int16_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;

  const unsigned int eighthPoints = num_points / 8;

  const float* inputVectorPtr = (const float*)inputVector;
  int16_t* outputVectorPtr = outputVector;

  float min_val = -32768;
  float max_val = 32767;
  float r;

  __m256 vScalar = _mm256_set1_ps(scalar);
  __m256 inputVal, ret;
  __m256i intInputVal;
  __m128i intInputVal1, intInputVal2;
  __m256 vmin_val = _mm256_set1_ps(min_val);
  __m256 vmax_val = _mm256_set1_ps(max_val);

  for(;number < eighthPoints; number++){
    inputVal = _mm256_loadu_ps(inputVectorPtr); inputVectorPtr += 8;

    // Scale and clip
    ret = _mm256_max_ps(_mm256_min_ps(_mm256_mul_ps(inputVal, vScalar), vmax_val), vmin_val);

    intInputVal = _mm256_cvtps_epi32(ret);

    intInputVal1 = _mm256_extractf128_si256(intInputVal, 0);
    intInputVal2 = _mm256_extractf128_si256(intInputVal, 1);

    intInputVal1 = _mm_packs_epi32(intInputVal1, intInputVal2);

    _mm_storeu_si128((__m128i*)outputVectorPtr, intInputVal1);
    outputVectorPtr += 8;
  }

  number = eighthPoints * 8;
  for(; number < num_points; number++){
    r = inputVector[number] * scalar;
    if(r > max_val)
      r = max_val;
    else if(r < min_val)
      r = min_val;
    outputVector[number] = (int16_t)rintf(r);
  }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 16 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 16 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
    \note Input buffer does NOT need to be properly aligned
  */
static inline void volk_32f_s32f_convert_16i_u_sse2(int16_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;

  const unsigned int eighthPoints = num_points / 8;

  const float* inputVectorPtr = (const float*)inputVector;
  int16_t* outputVectorPtr = outputVector;

  float min_val = -32768;
  float max_val = 32767;
  float r;

  __m128 vScalar = _mm_set_ps1(scalar);
  __m128 inputVal1, inputVal2;
  __m128i intInputVal1, intInputVal2;
  __m128 ret1, ret2;
  __m128 vmin_val = _mm_set_ps1(min_val);
  __m128 vmax_val = _mm_set_ps1(max_val);

  for(;number < eighthPoints; number++){
    inputVal1 = _mm_loadu_ps(inputVectorPtr); inputVectorPtr += 4;
    inputVal2 = _mm_loadu_ps(inputVectorPtr); inputVectorPtr += 4;

    // Scale and clip
    ret1 = _mm_max_ps(_mm_min_ps(_mm_mul_ps(inputVal1, vScalar), vmax_val), vmin_val);
    ret2 = _mm_max_ps(_mm_min_ps(_mm_mul_ps(inputVal2, vScalar), vmax_val), vmin_val);

    intInputVal1 = _mm_cvtps_epi32(ret1);
    intInputVal2 = _mm_cvtps_epi32(ret2);

    intInputVal1 = _mm_packs_epi32(intInputVal1, intInputVal2);

    _mm_storeu_si128((__m128i*)outputVectorPtr, intInputVal1);
    outputVectorPtr += 8;
  }

  number = eighthPoints * 8;
  for(; number < num_points; number++){
    r = inputVector[number] * scalar;
    if(r > max_val)
      r = max_val;
    else if(r < min_val)
      r = min_val;
    outputVector[number] = (int16_t)rintf(r);
  }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 16 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 16 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
    \note Input buffer does NOT need to be properly aligned
  */
static inline void volk_32f_s32f_convert_16i_u_sse(int16_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;

  const unsigned int quarterPoints = num_points / 4;

  const float* inputVectorPtr = (const float*)inputVector;
  int16_t* outputVectorPtr = outputVector;

  float min_val = -32768;
  float max_val = 32767;
  float r;

  __m128 vScalar = _mm_set_ps1(scalar);
  __m128 ret;
  __m128 vmin_val = _mm_set_ps1(min_val);
  __m128 vmax_val = _mm_set_ps1(max_val);

  __VOLK_ATTR_ALIGNED(16) float outputFloatBuffer[4];

  for(;number < quarterPoints; number++){
    ret = _mm_loadu_ps(inputVectorPtr);
    inputVectorPtr += 4;

    // Scale and clip
    ret = _mm_max_ps(_mm_min_ps(_mm_mul_ps(ret, vScalar), vmax_val), vmin_val);

    _mm_store_ps(outputFloatBuffer, ret);
    *outputVectorPtr++ = (int16_t)rintf(outputFloatBuffer[0]);
    *outputVectorPtr++ = (int16_t)rintf(outputFloatBuffer[1]);
    *outputVectorPtr++ = (int16_t)rintf(outputFloatBuffer[2]);
    *outputVectorPtr++ = (int16_t)rintf(outputFloatBuffer[3]);
  }

  number = quarterPoints * 4;
  for(; number < num_points; number++){
    r = inputVector[number] * scalar;
    if(r > max_val)
      r = max_val;
    else if(r < min_val)
      r = min_val;
    outputVector[number] = (int16_t)rintf(r);
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
static inline void volk_32f_s32f_convert_16i_generic(int16_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  int16_t* outputVectorPtr = outputVector;
  const float* inputVectorPtr = inputVector;
  unsigned int number = 0;
  float min_val = -32768;
  float max_val = 32767;
  float r;

  for(number = 0; number < num_points; number++){
    r = *inputVectorPtr++  * scalar;
    if(r > max_val)
      r = max_val;
    else if(r < min_val)
      r = min_val;
    *outputVectorPtr++ = (int16_t)rintf(r);
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32f_s32f_convert_16i_u_H */
#ifndef INCLUDED_volk_32f_s32f_convert_16i_a_H
#define INCLUDED_volk_32f_s32f_convert_16i_a_H

#include <volk/volk_common.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#ifdef LV_HAVE_AVX
#include <immintrin.h>
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 16 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 16 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_32f_s32f_convert_16i_a_avx(int16_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;

  const unsigned int eighthPoints = num_points / 8;

  const float* inputVectorPtr = (const float*)inputVector;
  int16_t* outputVectorPtr = outputVector;

  float min_val = -32768;
  float max_val = 32767;
  float r;

  __m256 vScalar = _mm256_set1_ps(scalar);
  __m256 inputVal, ret;
  __m256i intInputVal;
  __m128i intInputVal1, intInputVal2;
  __m256 vmin_val = _mm256_set1_ps(min_val);
  __m256 vmax_val = _mm256_set1_ps(max_val);

  for(;number < eighthPoints; number++){
    inputVal = _mm256_load_ps(inputVectorPtr); inputVectorPtr += 8;

    // Scale and clip
    ret = _mm256_max_ps(_mm256_min_ps(_mm256_mul_ps(inputVal, vScalar), vmax_val), vmin_val);

    intInputVal = _mm256_cvtps_epi32(ret);

    intInputVal1 = _mm256_extractf128_si256(intInputVal, 0);
    intInputVal2 = _mm256_extractf128_si256(intInputVal, 1);

    intInputVal1 = _mm_packs_epi32(intInputVal1, intInputVal2);

    _mm_store_si128((__m128i*)outputVectorPtr, intInputVal1);
    outputVectorPtr += 8;
  }

  number = eighthPoints * 8;
  for(; number < num_points; number++){
    r = inputVector[number] * scalar;
    if(r > max_val)
      r = max_val;
    else if(r < min_val)
      r = min_val;
    outputVector[number] = (int16_t)rintf(r);
  }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 16 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 16 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_32f_s32f_convert_16i_a_sse2(int16_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;

  const unsigned int eighthPoints = num_points / 8;

  const float* inputVectorPtr = (const float*)inputVector;
  int16_t* outputVectorPtr = outputVector;

  float min_val = -32768;
  float max_val = 32767;
  float r;

  __m128 vScalar = _mm_set_ps1(scalar);
  __m128 inputVal1, inputVal2;
  __m128i intInputVal1, intInputVal2;
  __m128 ret1, ret2;
  __m128 vmin_val = _mm_set_ps1(min_val);
  __m128 vmax_val = _mm_set_ps1(max_val);

  for(;number < eighthPoints; number++){
    inputVal1 = _mm_load_ps(inputVectorPtr); inputVectorPtr += 4;
    inputVal2 = _mm_load_ps(inputVectorPtr); inputVectorPtr += 4;

    // Scale and clip
    ret1 = _mm_max_ps(_mm_min_ps(_mm_mul_ps(inputVal1, vScalar), vmax_val), vmin_val);
    ret2 = _mm_max_ps(_mm_min_ps(_mm_mul_ps(inputVal2, vScalar), vmax_val), vmin_val);

    intInputVal1 = _mm_cvtps_epi32(ret1);
    intInputVal2 = _mm_cvtps_epi32(ret2);

    intInputVal1 = _mm_packs_epi32(intInputVal1, intInputVal2);

    _mm_store_si128((__m128i*)outputVectorPtr, intInputVal1);
    outputVectorPtr += 8;
  }

  number = eighthPoints * 8;
  for(; number < num_points; number++){
    r = inputVector[number] * scalar;
    if(r > max_val)
      r = max_val;
    else if(r < min_val)
      r = min_val;
    outputVector[number] = (int16_t)rintf(r);
  }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
  /*!
    \brief Multiplies each point in the input buffer by the scalar value, then converts the result into a 16 bit integer value
    \param inputVector The floating point input data buffer
    \param outputVector The 16 bit output data buffer
    \param scalar The value multiplied against each point in the input buffer
    \param num_points The number of data values to be converted
  */
static inline void volk_32f_s32f_convert_16i_a_sse(int16_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;

  const unsigned int quarterPoints = num_points / 4;

  const float* inputVectorPtr = (const float*)inputVector;
  int16_t* outputVectorPtr = outputVector;

  float min_val = -32768;
  float max_val = 32767;
  float r;

  __m128 vScalar = _mm_set_ps1(scalar);
  __m128 ret;
  __m128 vmin_val = _mm_set_ps1(min_val);
  __m128 vmax_val = _mm_set_ps1(max_val);

  __VOLK_ATTR_ALIGNED(16) float outputFloatBuffer[4];

  for(;number < quarterPoints; number++){
    ret = _mm_load_ps(inputVectorPtr);
    inputVectorPtr += 4;

    // Scale and clip
    ret = _mm_max_ps(_mm_min_ps(_mm_mul_ps(ret, vScalar), vmax_val), vmin_val);

    _mm_store_ps(outputFloatBuffer, ret);
    *outputVectorPtr++ = (int16_t)rintf(outputFloatBuffer[0]);
    *outputVectorPtr++ = (int16_t)rintf(outputFloatBuffer[1]);
    *outputVectorPtr++ = (int16_t)rintf(outputFloatBuffer[2]);
    *outputVectorPtr++ = (int16_t)rintf(outputFloatBuffer[3]);
  }

  number = quarterPoints * 4;
  for(; number < num_points; number++){
    r = inputVector[number] * scalar;
    if(r > max_val)
      r = max_val;
    else if(r < min_val)
      r = min_val;
    outputVector[number] = (int16_t)rintf(r);
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
  */
static inline void volk_32f_s32f_convert_16i_a_generic(int16_t* outputVector, const float* inputVector, const float scalar, unsigned int num_points){
  int16_t* outputVectorPtr = outputVector;
  const float* inputVectorPtr = inputVector;
  unsigned int number = 0;
  float min_val = -32768;
  float max_val = 32767;
  float r;

  for(number = 0; number < num_points; number++){
    r  = *inputVectorPtr++ * scalar;
    if(r < min_val)
      r = min_val;
    else if(r > max_val)
      r = max_val;
    *outputVectorPtr++ = (int16_t)rintf(r);
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32f_s32f_convert_16i_a_H */

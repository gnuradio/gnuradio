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

#ifndef INCLUDED_volk_32f_accumulator_s32f_a_H
#define INCLUDED_volk_32f_accumulator_s32f_a_H

#include <volk/volk_common.h>
#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Accumulates the values in the input buffer
  \param result The accumulated result
  \param inputBuffer The buffer of data to be accumulated
  \param num_points The number of values in inputBuffer to be accumulated
*/
static inline void volk_32f_accumulator_s32f_a_sse(float* result, const float* inputBuffer, unsigned int num_points){
  float returnValue = 0;
  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;

  const float* aPtr = inputBuffer;
  __VOLK_ATTR_ALIGNED(16) float tempBuffer[4];

  __m128 accumulator = _mm_setzero_ps();
  __m128 aVal = _mm_setzero_ps();

  for(;number < quarterPoints; number++){
    aVal = _mm_load_ps(aPtr);
    accumulator = _mm_add_ps(accumulator, aVal);
    aPtr += 4;
  }
  _mm_store_ps(tempBuffer,accumulator); // Store the results back into the C container
  returnValue = tempBuffer[0];
  returnValue += tempBuffer[1];
  returnValue += tempBuffer[2];
  returnValue += tempBuffer[3];

  number = quarterPoints * 4;
  for(;number < num_points; number++){
    returnValue += (*aPtr++);
  }
  *result = returnValue;
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Accumulates the values in the input buffer
  \param result The accumulated result
  \param inputBuffer The buffer of data to be accumulated
  \param num_points The number of values in inputBuffer to be accumulated
*/
static inline void volk_32f_accumulator_s32f_generic(float* result, const float* inputBuffer, unsigned int num_points){
  const float* aPtr = inputBuffer;
  unsigned int number = 0;
  float returnValue = 0;

  for(;number < num_points; number++){
    returnValue += (*aPtr++);
  }
  *result = returnValue;
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32f_accumulator_s32f_a_H */

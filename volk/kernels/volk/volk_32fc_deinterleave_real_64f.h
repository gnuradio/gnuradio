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

#ifndef INCLUDED_volk_32fc_deinterleave_real_64f_a_H
#define INCLUDED_volk_32fc_deinterleave_real_64f_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
/*!
  \brief Deinterleaves the complex vector into I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_32fc_deinterleave_real_64f_a_sse2(double* iBuffer, const lv_32fc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;

  const float* complexVectorPtr = (float*)complexVector;
  double* iBufferPtr = iBuffer;

  const unsigned int halfPoints = num_points / 2;
  __m128 cplxValue, fVal;
  __m128d dVal;
  for(;number < halfPoints; number++){

    cplxValue = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;

    // Arrange in i1i2i1i2 format
    fVal = _mm_shuffle_ps(cplxValue, cplxValue, _MM_SHUFFLE(2,0,2,0));
    dVal = _mm_cvtps_pd(fVal);
    _mm_store_pd(iBufferPtr, dVal);

    iBufferPtr += 2;
  }

  number = halfPoints * 2;
  for(; number < num_points; number++){
    *iBufferPtr++ = (double)*complexVectorPtr++;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Deinterleaves the complex vector into I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_32fc_deinterleave_real_64f_generic(double* iBuffer, const lv_32fc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const float* complexVectorPtr = (float*)complexVector;
  double* iBufferPtr = iBuffer;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = (double)*complexVectorPtr++;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32fc_deinterleave_real_64f_a_H */

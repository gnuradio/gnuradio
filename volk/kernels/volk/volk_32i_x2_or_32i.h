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

#ifndef INCLUDED_volk_32i_x2_or_32i_a_H
#define INCLUDED_volk_32i_x2_or_32i_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Ors the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be ored
  \param bVector One of the vectors to be ored
  \param num_points The number of values in aVector and bVector to be ored together and stored into cVector
*/
static inline void volk_32i_x2_or_32i_a_sse(int32_t* cVector, const int32_t* aVector, const int32_t* bVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    float* cPtr = (float*)cVector;
    const float* aPtr = (float*)aVector;
    const float* bPtr = (float*)bVector;

    __m128 aVal, bVal, cVal;
    for(;number < quarterPoints; number++){

      aVal = _mm_load_ps(aPtr);
      bVal = _mm_load_ps(bPtr);

      cVal = _mm_or_ps(aVal, bVal);

      _mm_store_ps(cPtr,cVal); // Store the results back into the C container

      aPtr += 4;
      bPtr += 4;
      cPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
      cVector[number] = aVector[number] | bVector[number];
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
/*!
  \brief Ands the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors
  \param bVector One of the vectors
  \param num_points The number of values in aVector and bVector to be anded together and stored into cVector
*/
static inline void volk_32i_x2_or_32i_neon(int32_t* cVector, const int32_t* aVector, const int32_t* bVector, unsigned int num_points){
    int32_t* cPtr = cVector;
    const int32_t* aPtr = aVector;
    const int32_t* bPtr=  bVector;
    unsigned int number = 0;
    unsigned int quarter_points = num_points / 4;

    int32x4_t a_val, b_val, c_val;

    for(number = 0; number < quarter_points; number++){
        a_val = vld1q_s32(aPtr);
        b_val = vld1q_s32(bPtr);
        c_val = vorrq_s32(a_val, b_val);
        vst1q_s32(cPtr, c_val);
        aPtr += 4;
        bPtr += 4;
        cPtr += 4;
    }

    for(number = quarter_points * 4; number < num_points; number++){
      *cPtr++ = (*aPtr++) | (*bPtr++);
    }
}
#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Ors the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be ored
  \param bVector One of the vectors to be ored
  \param num_points The number of values in aVector and bVector to be ored together and stored into cVector
*/
static inline void volk_32i_x2_or_32i_generic(int32_t* cVector, const int32_t* aVector, const int32_t* bVector, unsigned int num_points){
    int32_t* cPtr = cVector;
    const int32_t* aPtr = aVector;
    const int32_t* bPtr=  bVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
      *cPtr++ = (*aPtr++) | (*bPtr++);
    }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_ORC
/*!
  \brief Ors the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be ored
  \param bVector One of the vectors to be ored
  \param num_points The number of values in aVector and bVector to be ored together and stored into cVector
*/
extern void volk_32i_x2_or_32i_a_orc_impl(int32_t* cVector, const int32_t* aVector, const int32_t* bVector, unsigned int num_points);
static inline void volk_32i_x2_or_32i_u_orc(int32_t* cVector, const int32_t* aVector, const int32_t* bVector, unsigned int num_points){
    volk_32i_x2_or_32i_a_orc_impl(cVector, aVector, bVector, num_points);
}
#endif /* LV_HAVE_ORC */


#endif /* INCLUDED_volk_32i_x2_or_32i_a_H */

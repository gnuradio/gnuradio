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

#ifndef INCLUDED_volk_32f_x2_min_32f_a_H
#define INCLUDED_volk_32f_x2_min_32f_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Selects minimum value from each entry between bVector and aVector and store their results in the cVector
  \param cVector The vector where the results will be stored
  \param aVector The vector to be checked
  \param bVector The vector to be checked
  \param num_points The number of values in aVector and bVector to be checked and stored into cVector
*/
static inline void volk_32f_x2_min_32f_a_sse(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    float* cPtr = cVector;
    const float* aPtr = aVector;
    const float* bPtr=  bVector;

    __m128 aVal, bVal, cVal;
    for(;number < quarterPoints; number++){

      aVal = _mm_load_ps(aPtr);
      bVal = _mm_load_ps(bPtr);

      cVal = _mm_min_ps(aVal, bVal);

      _mm_store_ps(cPtr,cVal); // Store the results back into the C container

      aPtr += 4;
      bPtr += 4;
      cPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
      const float a = *aPtr++;
      const float b = *bPtr++;
      *cPtr++ = ( a < b ? a : b);
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_NEON
#include <arm_neon.h>

/*!
  \brief Selects minimum value from each entry between bVector and aVector and store their results in the cVector
  \param cVector The vector where the results will be stored
  \param aVector The vector to be checked
  \param bVector The vector to be checked
  \param num_points The number of values in aVector and bVector to be checked and stored into cVector
*/
static inline void volk_32f_x2_min_32f_neon(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    float* cPtr = cVector;
    const float* aPtr = aVector;
    const float* bPtr=  bVector;
    unsigned int number = 0;
    unsigned int quarter_points = num_points / 4;

    float32x4_t a_vec, b_vec, c_vec;
    for(number = 0; number < quarter_points; number++){
        a_vec = vld1q_f32(aPtr);
        b_vec = vld1q_f32(bPtr);

        c_vec = vminq_f32(a_vec, b_vec);

        vst1q_f32(cPtr, c_vec);
        aPtr += 4;
        bPtr += 4;
        cPtr += 4;
    }

    for(number = quarter_points*4; number < num_points; number++){
      const float a = *aPtr++;
      const float b = *bPtr++;
      *cPtr++ = ( a < b ? a : b);
    }
}
#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Selects minimum value from each entry between bVector and aVector and store their results in the cVector
  \param cVector The vector where the results will be stored
  \param aVector The vector to be checked
  \param bVector The vector to be checked
  \param num_points The number of values in aVector and bVector to be checked and stored into cVector
*/
static inline void volk_32f_x2_min_32f_generic(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    float* cPtr = cVector;
    const float* aPtr = aVector;
    const float* bPtr=  bVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
      const float a = *aPtr++;
      const float b = *bPtr++;
      *cPtr++ = ( a < b ? a : b);
    }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_ORC
/*!
  \brief Selects minimum value from each entry between bVector and aVector and store their results in the cVector
  \param cVector The vector where the results will be stored
  \param aVector The vector to be checked
  \param bVector The vector to be checked
  \param num_points The number of values in aVector and bVector to be checked and stored into cVector
*/
extern void volk_32f_x2_min_32f_a_orc_impl(float* cVector, const float* aVector, const float* bVector, unsigned int num_points);
static inline void volk_32f_x2_min_32f_u_orc(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    volk_32f_x2_min_32f_a_orc_impl(cVector, aVector, bVector, num_points);
}
#endif /* LV_HAVE_ORC */


#endif /* INCLUDED_volk_32f_x2_min_32f_a_H */

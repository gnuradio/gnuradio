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

#ifndef INCLUDED_volk_64f_x2_max_64f_a_H
#define INCLUDED_volk_64f_x2_max_64f_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
/*!
  \brief Selects maximum value from each entry between bVector and aVector and store their results in the cVector
  \param cVector The vector where the results will be stored
  \param aVector The vector to be checked
  \param bVector The vector to be checked
  \param num_points The number of values in aVector and bVector to be checked and stored into cVector
*/
static inline void volk_64f_x2_max_64f_a_sse2(double* cVector, const double* aVector, const double* bVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int halfPoints = num_points / 2;

    double* cPtr = cVector;
    const double* aPtr = aVector;
    const double* bPtr=  bVector;

    __m128d aVal, bVal, cVal;
    for(;number < halfPoints; number++){

      aVal = _mm_load_pd(aPtr);
      bVal = _mm_load_pd(bPtr);

      cVal = _mm_max_pd(aVal, bVal);

      _mm_store_pd(cPtr,cVal); // Store the results back into the C container

      aPtr += 2;
      bPtr += 2;
      cPtr += 2;
    }

    number = halfPoints * 2;
    for(;number < num_points; number++){
      const double a = *aPtr++;
      const double b = *bPtr++;
      *cPtr++ = ( a > b ? a : b);
    }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Selects maximum value from each entry between bVector and aVector and store their results in the cVector
  \param cVector The vector where the results will be stored
  \param aVector The vector to be checked
  \param bVector The vector to be checked
  \param num_points The number of values in aVector and bVector to be checked and stored into cVector
*/
static inline void volk_64f_x2_max_64f_generic(double* cVector, const double* aVector, const double* bVector, unsigned int num_points){
    double* cPtr = cVector;
    const double* aPtr = aVector;
    const double* bPtr=  bVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
      const double a = *aPtr++;
      const double b = *bPtr++;
      *cPtr++ = ( a > b ? a : b);
    }
}
#endif /* LV_HAVE_GENERIC */


#endif /* INCLUDED_volk_64f_x2_max_64f_a_H */

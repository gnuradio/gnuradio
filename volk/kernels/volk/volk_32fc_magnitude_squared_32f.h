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

#ifndef INCLUDED_volk_32fc_magnitude_squared_32f_u_H
#define INCLUDED_volk_32fc_magnitude_squared_32f_u_H

#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#ifdef LV_HAVE_AVX
#include <immintrin.h>
  /*!
    \brief Calculates the magnitude squared of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_squared_32f_u_avx(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int eighthPoints = num_points / 8;

    const float* complexVectorPtr = (float*)complexVector;
    float* magnitudeVectorPtr = magnitudeVector;

    __m256 cplxValue1, cplxValue2, complex1, complex2, result;
    for(;number < eighthPoints; number++){
      cplxValue1 = _mm256_loadu_ps(complexVectorPtr);
      complexVectorPtr += 8;

      cplxValue2 = _mm256_loadu_ps(complexVectorPtr);
      complexVectorPtr += 8;

      cplxValue1 = _mm256_mul_ps(cplxValue1, cplxValue1); // Square the values
      cplxValue2 = _mm256_mul_ps(cplxValue2, cplxValue2); // Square the Values

      complex1 = _mm256_permute2f128_ps(cplxValue1, cplxValue2, 0x20);
      complex2 = _mm256_permute2f128_ps(cplxValue1, cplxValue2, 0x31);

      result = _mm256_hadd_ps(complex1, complex2); // Add the I2 and Q2 values

      _mm256_storeu_ps(magnitudeVectorPtr, result);
      magnitudeVectorPtr += 8;
    }

    number = eighthPoints * 8;
    for(; number < num_points; number++){
      float val1Real = *complexVectorPtr++;
      float val1Imag = *complexVectorPtr++;
      *magnitudeVectorPtr++ = (val1Real * val1Real) + (val1Imag * val1Imag);
    }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE3
#include <pmmintrin.h>
  /*!
    \brief Calculates the magnitude squared of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_squared_32f_u_sse3(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    const float* complexVectorPtr = (float*)complexVector;
    float* magnitudeVectorPtr = magnitudeVector;

    __m128 cplxValue1, cplxValue2, result;
    for(;number < quarterPoints; number++){
      cplxValue1 = _mm_loadu_ps(complexVectorPtr);
      complexVectorPtr += 4;

      cplxValue2 = _mm_loadu_ps(complexVectorPtr);
      complexVectorPtr += 4;

      cplxValue1 = _mm_mul_ps(cplxValue1, cplxValue1); // Square the values
      cplxValue2 = _mm_mul_ps(cplxValue2, cplxValue2); // Square the Values

      result = _mm_hadd_ps(cplxValue1, cplxValue2); // Add the I2 and Q2 values

      _mm_storeu_ps(magnitudeVectorPtr, result);
      magnitudeVectorPtr += 4;
    }

    number = quarterPoints * 4;
    for(; number < num_points; number++){
      float val1Real = *complexVectorPtr++;
      float val1Imag = *complexVectorPtr++;
      *magnitudeVectorPtr++ = (val1Real * val1Real) + (val1Imag * val1Imag);
    }
}
#endif /* LV_HAVE_SSE3 */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
  /*!
    \brief Calculates the magnitude squared of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_squared_32f_u_sse(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    const float* complexVectorPtr = (float*)complexVector;
    float* magnitudeVectorPtr = magnitudeVector;

    __m128 cplxValue1, cplxValue2, iValue, qValue, result;
    for(;number < quarterPoints; number++){
      cplxValue1 = _mm_loadu_ps(complexVectorPtr);
      complexVectorPtr += 4;

      cplxValue2 = _mm_loadu_ps(complexVectorPtr);
      complexVectorPtr += 4;

      // Arrange in i1i2i3i4 format
      iValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(2,0,2,0));
      // Arrange in q1q2q3q4 format
      qValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(3,1,3,1));

      iValue = _mm_mul_ps(iValue, iValue); // Square the I values
      qValue = _mm_mul_ps(qValue, qValue); // Square the Q Values

      result = _mm_add_ps(iValue, qValue); // Add the I2 and Q2 values

      _mm_storeu_ps(magnitudeVectorPtr, result);
      magnitudeVectorPtr += 4;
    }

    number = quarterPoints * 4;
    for(; number < num_points; number++){
       float val1Real = *complexVectorPtr++;
       float val1Imag = *complexVectorPtr++;
      *magnitudeVectorPtr++ = (val1Real * val1Real) + (val1Imag * val1Imag);
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Calculates the magnitude squared of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_squared_32f_generic(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
  const float* complexVectorPtr = (float*)complexVector;
  float* magnitudeVectorPtr = magnitudeVector;
  unsigned int number = 0;
  for(number = 0; number < num_points; number++){
    const float real = *complexVectorPtr++;
    const float imag = *complexVectorPtr++;
    *magnitudeVectorPtr++ = (real*real) + (imag*imag);
  }
}
#endif /* LV_HAVE_GENERIC */

#endif /* INCLUDED_volk_32fc_magnitude_32f_u_H */
#ifndef INCLUDED_volk_32fc_magnitude_squared_32f_a_H
#define INCLUDED_volk_32fc_magnitude_squared_32f_a_H

#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#ifdef LV_HAVE_AVX
#include <immintrin.h>
  /*!
    \brief Calculates the magnitude squared of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_squared_32f_a_avx(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int eighthPoints = num_points / 8;

    const float* complexVectorPtr = (float*)complexVector;
    float* magnitudeVectorPtr = magnitudeVector;

    __m256 cplxValue1, cplxValue2, complex1, complex2, result;
    for(;number < eighthPoints; number++){
      cplxValue1 = _mm256_load_ps(complexVectorPtr);
      complexVectorPtr += 8;

      cplxValue2 = _mm256_load_ps(complexVectorPtr);
      complexVectorPtr += 8;

      cplxValue1 = _mm256_mul_ps(cplxValue1, cplxValue1); // Square the values
      cplxValue2 = _mm256_mul_ps(cplxValue2, cplxValue2); // Square the Values

      complex1 = _mm256_permute2f128_ps(cplxValue1, cplxValue2, 0x20);
      complex2 = _mm256_permute2f128_ps(cplxValue1, cplxValue2, 0x31);

      result = _mm256_hadd_ps(complex1, complex2); // Add the I2 and Q2 values

      _mm256_store_ps(magnitudeVectorPtr, result);
      magnitudeVectorPtr += 8;
    }

    number = eighthPoints * 8;
    for(; number < num_points; number++){
      float val1Real = *complexVectorPtr++;
      float val1Imag = *complexVectorPtr++;
      *magnitudeVectorPtr++ = (val1Real * val1Real) + (val1Imag * val1Imag);
    }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE3
#include <pmmintrin.h>
  /*!
    \brief Calculates the magnitude squared of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_squared_32f_a_sse3(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    const float* complexVectorPtr = (float*)complexVector;
    float* magnitudeVectorPtr = magnitudeVector;

    __m128 cplxValue1, cplxValue2, result;
    for(;number < quarterPoints; number++){
      cplxValue1 = _mm_load_ps(complexVectorPtr);
      complexVectorPtr += 4;

      cplxValue2 = _mm_load_ps(complexVectorPtr);
      complexVectorPtr += 4;

      cplxValue1 = _mm_mul_ps(cplxValue1, cplxValue1); // Square the values
      cplxValue2 = _mm_mul_ps(cplxValue2, cplxValue2); // Square the Values

      result = _mm_hadd_ps(cplxValue1, cplxValue2); // Add the I2 and Q2 values

      _mm_store_ps(magnitudeVectorPtr, result);
      magnitudeVectorPtr += 4;
    }

    number = quarterPoints * 4;
    for(; number < num_points; number++){
      float val1Real = *complexVectorPtr++;
      float val1Imag = *complexVectorPtr++;
      *magnitudeVectorPtr++ = (val1Real * val1Real) + (val1Imag * val1Imag);
    }
}
#endif /* LV_HAVE_SSE3 */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
  /*!
    \brief Calculates the magnitude squared of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_squared_32f_a_sse(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    const float* complexVectorPtr = (float*)complexVector;
    float* magnitudeVectorPtr = magnitudeVector;

    __m128 cplxValue1, cplxValue2, iValue, qValue, result;
    for(;number < quarterPoints; number++){
      cplxValue1 = _mm_load_ps(complexVectorPtr);
      complexVectorPtr += 4;

      cplxValue2 = _mm_load_ps(complexVectorPtr);
      complexVectorPtr += 4;

      // Arrange in i1i2i3i4 format
      iValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(2,0,2,0));
      // Arrange in q1q2q3q4 format
      qValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(3,1,3,1));

      iValue = _mm_mul_ps(iValue, iValue); // Square the I values
      qValue = _mm_mul_ps(qValue, qValue); // Square the Q Values

      result = _mm_add_ps(iValue, qValue); // Add the I2 and Q2 values

      _mm_store_ps(magnitudeVectorPtr, result);
      magnitudeVectorPtr += 4;
    }

    number = quarterPoints * 4;
    for(; number < num_points; number++){
       float val1Real = *complexVectorPtr++;
       float val1Imag = *complexVectorPtr++;
      *magnitudeVectorPtr++ = (val1Real * val1Real) + (val1Imag * val1Imag);
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
//

  /*!
    \brief Calculates the magnitude squared of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_squared_32f_neon(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    const float* complexVectorPtr = (float*)complexVector;
    float* magnitudeVectorPtr = magnitudeVector;

    float32x4x2_t cmplx_val;
    float32x4_t result;
    for(;number < quarterPoints; number++){
      cmplx_val = vld2q_f32(complexVectorPtr);
      complexVectorPtr += 8;

      cmplx_val.val[0] = vmulq_f32(cmplx_val.val[0], cmplx_val.val[0]); // Square the values
      cmplx_val.val[1] = vmulq_f32(cmplx_val.val[1], cmplx_val.val[1]); // Square the values

      result = vaddq_f32(cmplx_val.val[0], cmplx_val.val[1]); // Add the I2 and Q2 values

      vst1q_f32(magnitudeVectorPtr, result);
      magnitudeVectorPtr += 4;
    }

    number = quarterPoints * 4;
    for(; number < num_points; number++){
      float val1Real = *complexVectorPtr++;
      float val1Imag = *complexVectorPtr++;
      *magnitudeVectorPtr++ = (val1Real * val1Real) + (val1Imag * val1Imag);
    }
}
#endif /* LV_HAVE_NEON */


#ifdef LV_HAVE_GENERIC
  /*!
    \brief Calculates the magnitude squared of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_squared_32f_a_generic(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
  const float* complexVectorPtr = (float*)complexVector;
  float* magnitudeVectorPtr = magnitudeVector;
  unsigned int number = 0;
  for(number = 0; number < num_points; number++){
    const float real = *complexVectorPtr++;
    const float imag = *complexVectorPtr++;
    *magnitudeVectorPtr++ = (real*real) + (imag*imag);
  }
}
#endif /* LV_HAVE_GENERIC */

#endif /* INCLUDED_volk_32fc_magnitude_32f_a_H */

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

#ifndef INCLUDED_volk_32fc_magnitude_32f_u_H
#define INCLUDED_volk_32fc_magnitude_32f_u_H

#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#ifdef LV_HAVE_AVX
#include <immintrin.h>
  /*!
    \brief Calculates the magnitude of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_32f_u_avx(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
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

      result = _mm256_sqrt_ps(result);

      _mm256_storeu_ps(magnitudeVectorPtr, result);
      magnitudeVectorPtr += 8;
    }

    number = eighthPoints * 8;
    for(; number < num_points; number++){
      float val1Real = *complexVectorPtr++;
      float val1Imag = *complexVectorPtr++;
      *magnitudeVectorPtr++ = sqrtf((val1Real * val1Real) + (val1Imag * val1Imag));
    }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE3
#include <pmmintrin.h>
  /*!
    \brief Calculates the magnitude of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_32f_u_sse3(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
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

      result = _mm_sqrt_ps(result);

      _mm_storeu_ps(magnitudeVectorPtr, result);
      magnitudeVectorPtr += 4;
    }

    number = quarterPoints * 4;
    for(; number < num_points; number++){
      float val1Real = *complexVectorPtr++;
      float val1Imag = *complexVectorPtr++;
      *magnitudeVectorPtr++ = sqrtf((val1Real * val1Real) + (val1Imag * val1Imag));
    }
}
#endif /* LV_HAVE_SSE3 */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
  /*!
    \brief Calculates the magnitude of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_32f_u_sse(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
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

      result = _mm_sqrt_ps(result);

      _mm_storeu_ps(magnitudeVectorPtr, result);
      magnitudeVectorPtr += 4;
    }

    number = quarterPoints * 4;
    for(; number < num_points; number++){
       float val1Real = *complexVectorPtr++;
       float val1Imag = *complexVectorPtr++;
      *magnitudeVectorPtr++ = sqrtf((val1Real * val1Real) + (val1Imag * val1Imag));
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Calculates the magnitude of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_32f_generic(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
  const float* complexVectorPtr = (float*)complexVector;
  float* magnitudeVectorPtr = magnitudeVector;
  unsigned int number = 0;
  for(number = 0; number < num_points; number++){
    const float real = *complexVectorPtr++;
    const float imag = *complexVectorPtr++;
    *magnitudeVectorPtr++ = sqrtf((real*real) + (imag*imag));
  }
}
#endif /* LV_HAVE_GENERIC */

#endif /* INCLUDED_volk_32fc_magnitude_32f_u_H */
#ifndef INCLUDED_volk_32fc_magnitude_32f_a_H
#define INCLUDED_volk_32fc_magnitude_32f_a_H

#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#ifdef LV_HAVE_AVX
#include <immintrin.h>
  /*!
    \brief Calculates the magnitude of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_32f_a_avx(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
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

      result = _mm256_sqrt_ps(result);

      _mm256_store_ps(magnitudeVectorPtr, result);
      magnitudeVectorPtr += 8;
    }

    number = eighthPoints * 8;
    for(; number < num_points; number++){
      float val1Real = *complexVectorPtr++;
      float val1Imag = *complexVectorPtr++;
      *magnitudeVectorPtr++ = sqrtf((val1Real * val1Real) + (val1Imag * val1Imag));
    }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE3
#include <pmmintrin.h>
  /*!
    \brief Calculates the magnitude of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_32f_a_sse3(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
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

      result = _mm_sqrt_ps(result);

      _mm_store_ps(magnitudeVectorPtr, result);
      magnitudeVectorPtr += 4;
    }

    number = quarterPoints * 4;
    for(; number < num_points; number++){
      float val1Real = *complexVectorPtr++;
      float val1Imag = *complexVectorPtr++;
      *magnitudeVectorPtr++ = sqrtf((val1Real * val1Real) + (val1Imag * val1Imag));
    }
}
#endif /* LV_HAVE_SSE3 */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
  /*!
    \brief Calculates the magnitude of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_32f_a_sse(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
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

      result = _mm_sqrt_ps(result);

      _mm_store_ps(magnitudeVectorPtr, result);
      magnitudeVectorPtr += 4;
    }

    number = quarterPoints * 4;
    for(; number < num_points; number++){
       float val1Real = *complexVectorPtr++;
       float val1Imag = *complexVectorPtr++;
      *magnitudeVectorPtr++ = sqrtf((val1Real * val1Real) + (val1Imag * val1Imag));
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Calculates the magnitude of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_32f_a_generic(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
  const float* complexVectorPtr = (float*)complexVector;
  float* magnitudeVectorPtr = magnitudeVector;
  unsigned int number = 0;
  for(number = 0; number < num_points; number++){
    const float real = *complexVectorPtr++;
    const float imag = *complexVectorPtr++;
    *magnitudeVectorPtr++ = sqrtf((real*real) + (imag*imag));
  }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_NEON
#include <arm_neon.h>

  /*!
    \brief Calculates the magnitude of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_32f_neon(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
    unsigned int number;
    unsigned int quarter_points = num_points / 4;
    const float* complexVectorPtr = (float*)complexVector;
    float* magnitudeVectorPtr = magnitudeVector;

    float32x4x2_t complex_vec;
    float32x4_t magnitude_vec;
    for(number = 0; number < quarter_points; number++){
        complex_vec = vld2q_f32(complexVectorPtr);
        complex_vec.val[0] = vmulq_f32(complex_vec.val[0], complex_vec.val[0]);
        magnitude_vec = vmlaq_f32(complex_vec.val[0], complex_vec.val[1], complex_vec.val[1]);
    magnitude_vec = vrsqrteq_f32(magnitude_vec);
    magnitude_vec = vrecpeq_f32( magnitude_vec ); // no plain ol' sqrt
        vst1q_f32(magnitudeVectorPtr, magnitude_vec);

        complexVectorPtr += 8;
        magnitudeVectorPtr += 4;
    }

    for(number = quarter_points*4; number < num_points; number++){
      const float real = *complexVectorPtr++;
      const float imag = *complexVectorPtr++;
      *magnitudeVectorPtr++ = sqrtf((real*real) + (imag*imag));
    }
}
#endif /* LV_HAVE_NEON */

#ifdef LV_HAVE_NEON
  /*!
    \brief Calculates the magnitude of the complexVector and stores the results in the magnitudeVector

    This is an approximation from "Streamlining Digital Signal Processing" by
    Richard Lyons. Apparently max error is about 1% and mean error is about 0.6%.
    The basic idea is to do a weighted sum of the abs. value of imag and real parts
    where weight A is always assigned to max(imag, real) and B is always min(imag,real).
    There are two pairs of cofficients chosen based on whether min <= 0.4142 max.
    This method is called equiripple-error magnitude estimation proposed by Filip in '73

    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
static inline void volk_32fc_magnitude_32f_neon_fancy_sweet(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
    unsigned int number;
    unsigned int quarter_points = num_points / 4;
    const float* complexVectorPtr = (float*)complexVector;
    float* magnitudeVectorPtr = magnitudeVector;

    const float threshold = 0.4142135;

    float32x4_t a_vec, b_vec, a_high, a_low, b_high, b_low;
    a_high = vdupq_n_f32( 0.84 );
    b_high = vdupq_n_f32( 0.561);
    a_low  = vdupq_n_f32( 0.99 );
    b_low  = vdupq_n_f32( 0.197);

    uint32x4_t comp0, comp1;

    float32x4x2_t complex_vec;
    float32x4_t min_vec, max_vec, magnitude_vec;
    float32x4_t real_abs, imag_abs;
    for(number = 0; number < quarter_points; number++){
        complex_vec = vld2q_f32(complexVectorPtr);

        real_abs = vabsq_f32(complex_vec.val[0]);
        imag_abs = vabsq_f32(complex_vec.val[1]);

        min_vec = vminq_f32(real_abs, imag_abs);
        max_vec = vmaxq_f32(real_abs, imag_abs);

        // effective branch to choose coefficient pair.
        comp0 = vcgtq_f32(min_vec, vmulq_n_f32(max_vec, threshold));
        comp1 = vcleq_f32(min_vec, vmulq_n_f32(max_vec, threshold));

        // and 0s or 1s with coefficients from previous effective branch
        a_vec = (float32x4_t)vaddq_s32(vandq_s32((int32x4_t)comp0, (int32x4_t)a_high), vandq_s32((int32x4_t)comp1, (int32x4_t)a_low));
        b_vec = (float32x4_t)vaddq_s32(vandq_s32((int32x4_t)comp0, (int32x4_t)b_high), vandq_s32((int32x4_t)comp1, (int32x4_t)b_low));

        // coefficients chosen, do the weighted sum
        min_vec = vmulq_f32(min_vec, b_vec);
        max_vec = vmulq_f32(max_vec, a_vec);

        magnitude_vec = vaddq_f32(min_vec, max_vec);
        vst1q_f32(magnitudeVectorPtr, magnitude_vec);

        complexVectorPtr += 8;
        magnitudeVectorPtr += 4;
    }

    for(number = quarter_points*4; number < num_points; number++){
      const float real = *complexVectorPtr++;
      const float imag = *complexVectorPtr++;
      *magnitudeVectorPtr++ = sqrtf((real*real) + (imag*imag));
    }
}
#endif /* LV_HAVE_NEON */


#ifdef LV_HAVE_ORC
  /*!
    \brief Calculates the magnitude of the complexVector and stores the results in the magnitudeVector
    \param complexVector The vector containing the complex input values
    \param magnitudeVector The vector containing the real output values
    \param num_points The number of complex values in complexVector to be calculated and stored into cVector
  */
extern void volk_32fc_magnitude_32f_a_orc_impl(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points);
static inline void volk_32fc_magnitude_32f_u_orc(float* magnitudeVector, const lv_32fc_t* complexVector, unsigned int num_points){
    volk_32fc_magnitude_32f_a_orc_impl(magnitudeVector, complexVector, num_points);
}
#endif /* LV_HAVE_ORC */


#endif /* INCLUDED_volk_32fc_magnitude_32f_a_H */

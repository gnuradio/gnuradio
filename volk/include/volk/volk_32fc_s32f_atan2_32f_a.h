#ifndef INCLUDED_volk_32fc_s32f_atan2_32f_a_H
#define INCLUDED_volk_32fc_s32f_atan2_32f_a_H

#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>

#ifdef LV_HAVE_LIB_SIMDMATH
#include <simdmath.h>
#endif /* LV_HAVE_LIB_SIMDMATH */

/*!
  \brief performs the atan2 on the input vector and stores the results in the output vector.
  \param outputVector The byte-aligned vector where the results will be stored.
  \param inputVector The byte-aligned input vector containing interleaved IQ data (I = cos, Q = sin).
  \param normalizeFactor The atan2 results will be divided by this normalization factor.
  \param num_points The number of complex values in the input vector.
*/
static inline void volk_32fc_s32f_atan2_32f_a_sse4_1(float* outputVector,  const lv_32fc_t* complexVector, const float normalizeFactor, unsigned int num_points){
  const float* complexVectorPtr = (float*)complexVector;
  float* outPtr = outputVector;

  unsigned int number = 0;
  const float invNormalizeFactor = 1.0 / normalizeFactor;

#ifdef LV_HAVE_LIB_SIMDMATH
  const unsigned int quarterPoints = num_points / 4;    
  __m128 testVector = _mm_set_ps1(2*M_PI);
  __m128 correctVector = _mm_set_ps1(M_PI);
  __m128 vNormalizeFactor = _mm_set_ps1(invNormalizeFactor);
  __m128 phase;
  __m128 complex1, complex2, iValue, qValue;
  __m128 keepMask;
    
  for (; number < quarterPoints; number++) {
    // Load IQ data:
    complex1 = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;
    complex2 = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;
    // Deinterleave IQ data:
    iValue = _mm_shuffle_ps(complex1, complex2, _MM_SHUFFLE(2,0,2,0)); 
    qValue = _mm_shuffle_ps(complex1, complex2, _MM_SHUFFLE(3,1,3,1)); 
    // Arctan to get phase:
    phase = atan2f4(qValue, iValue);
    // When Q = 0 and I < 0, atan2f4 sucks and returns 2pi vice pi.
    // Compare to 2pi:
    keepMask = _mm_cmpneq_ps(phase,testVector);
    phase = _mm_blendv_ps(correctVector, phase, keepMask);
    // done with above correction.      
    phase = _mm_mul_ps(phase, vNormalizeFactor);
    _mm_store_ps((float*)outPtr, phase);
    outPtr += 4;
  }
  number = quarterPoints * 4;
#endif /* LV_HAVE_SIMDMATH_H */

  for (; number < num_points; number++) {
    const float real = *complexVectorPtr++;
    const float imag = *complexVectorPtr++;
    *outPtr++ = atan2f(imag, real) * invNormalizeFactor;
  }
}
#endif /* LV_HAVE_SSE4_1 */


#ifdef LV_HAVE_SSE
#include <xmmintrin.h>

#ifdef LV_HAVE_LIB_SIMDMATH
#include <simdmath.h>
#endif /* LV_HAVE_LIB_SIMDMATH */

/*!
  \brief performs the atan2 on the input vector and stores the results in the output vector.
  \param outputVector The byte-aligned vector where the results will be stored.
  \param inputVector The byte-aligned input vector containing interleaved IQ data (I = cos, Q = sin).
  \param normalizeFactor The atan2 results will be divided by this normalization factor.
  \param num_points The number of complex values in the input vector.
*/
static inline void volk_32fc_s32f_atan2_32f_a_sse(float* outputVector,  const lv_32fc_t* complexVector, const float normalizeFactor, unsigned int num_points){
  const float* complexVectorPtr = (float*)complexVector;
  float* outPtr = outputVector;

  unsigned int number = 0;
  const float invNormalizeFactor = 1.0 / normalizeFactor;

#ifdef LV_HAVE_LIB_SIMDMATH
  const unsigned int quarterPoints = num_points / 4;    
  __m128 testVector = _mm_set_ps1(2*M_PI);
  __m128 correctVector = _mm_set_ps1(M_PI);
  __m128 vNormalizeFactor = _mm_set_ps1(invNormalizeFactor);
  __m128 phase;
  __m128 complex1, complex2, iValue, qValue;
  __m128 mask;
  __m128 keepMask;
    
  for (; number < quarterPoints; number++) {
    // Load IQ data:
    complex1 = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;
    complex2 = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;
    // Deinterleave IQ data:
    iValue = _mm_shuffle_ps(complex1, complex2, _MM_SHUFFLE(2,0,2,0)); 
    qValue = _mm_shuffle_ps(complex1, complex2, _MM_SHUFFLE(3,1,3,1)); 
    // Arctan to get phase:
    phase = atan2f4(qValue, iValue);
    // When Q = 0 and I < 0, atan2f4 sucks and returns 2pi vice pi.
    // Compare to 2pi:
    keepMask = _mm_cmpneq_ps(phase,testVector);
    phase = _mm_and_ps(phase, keepMask);
    mask = _mm_andnot_ps(keepMask, correctVector);
    phase = _mm_or_ps(phase, mask);
    // done with above correction.      
    phase = _mm_mul_ps(phase, vNormalizeFactor);
    _mm_store_ps((float*)outPtr, phase);
    outPtr += 4;
  }
  number = quarterPoints * 4;
#endif /* LV_HAVE_SIMDMATH_H */

  for (; number < num_points; number++) {
    const float real = *complexVectorPtr++;
    const float imag = *complexVectorPtr++;
    *outPtr++ = atan2f(imag, real) * invNormalizeFactor;
  }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief performs the atan2 on the input vector and stores the results in the output vector.
  \param outputVector The vector where the results will be stored.
  \param inputVector Input vector containing interleaved IQ data (I = cos, Q = sin).
  \param normalizeFactor The atan2 results will be divided by this normalization factor.
  \param num_points The number of complex values in the input vector.
*/
static inline void volk_32fc_s32f_atan2_32f_a_generic(float* outputVector, const lv_32fc_t* inputVector, const float normalizeFactor, unsigned int num_points){
  float* outPtr = outputVector;
  const float* inPtr = (float*)inputVector;
  const float invNormalizeFactor = 1.0 / normalizeFactor;
  unsigned int number;
  for ( number = 0; number < num_points; number++) {
    const float real = *inPtr++;
    const float imag = *inPtr++;
    *outPtr++ = atan2f(imag, real) * invNormalizeFactor;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32fc_s32f_atan2_32f_a_H */

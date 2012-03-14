#ifndef INCLUDED_volk_32fc_deinterleave_imag_32f_a_H
#define INCLUDED_volk_32fc_deinterleave_imag_32f_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Deinterleaves the complex vector into Q vector data
  \param complexVector The complex input vector
  \param qBuffer The Q buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_32fc_deinterleave_imag_32f_a_sse(float* qBuffer, const lv_32fc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;

  const float* complexVectorPtr = (const float*)complexVector;
  float* qBufferPtr = qBuffer;

  __m128 cplxValue1, cplxValue2, iValue;
  for(;number < quarterPoints; number++){
      
    cplxValue1 = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;

    cplxValue2 = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;

    // Arrange in q1q2q3q4 format
    iValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(3,1,3,1));

    _mm_store_ps(qBufferPtr, iValue);

    qBufferPtr += 4;
  }

  number = quarterPoints * 4;
  for(; number < num_points; number++){
    complexVectorPtr++;
    *qBufferPtr++ = *complexVectorPtr++;
  }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Deinterleaves the complex vector into Q vector data
  \param complexVector The complex input vector
  \param qBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_32fc_deinterleave_imag_32f_a_generic(float* qBuffer, const lv_32fc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const float* complexVectorPtr = (float*)complexVector;
  float* qBufferPtr = qBuffer;
  for(number = 0; number < num_points; number++){
    complexVectorPtr++;
    *qBufferPtr++ = *complexVectorPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32fc_deinterleave_imag_32f_a_H */

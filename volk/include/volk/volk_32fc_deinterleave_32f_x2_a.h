#ifndef INCLUDED_volk_32fc_deinterleave_32f_x2_a_H
#define INCLUDED_volk_32fc_deinterleave_32f_x2_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Deinterleaves the complex vector into I & Q vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param qBuffer The Q buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_32fc_deinterleave_32f_x2_a_sse(float* iBuffer, float* qBuffer, const lv_32fc_t* complexVector, unsigned int num_points){
  const float* complexVectorPtr = (float*)complexVector;
  float* iBufferPtr = iBuffer;
  float* qBufferPtr = qBuffer;

  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;    
  __m128 cplxValue1, cplxValue2, iValue, qValue;
  for(;number < quarterPoints; number++){
      
    cplxValue1 = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;

    cplxValue2 = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;

    // Arrange in i1i2i3i4 format
    iValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(2,0,2,0));
    // Arrange in q1q2q3q4 format
    qValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(3,1,3,1));

    _mm_store_ps(iBufferPtr, iValue);
    _mm_store_ps(qBufferPtr, qValue);

    iBufferPtr += 4;
    qBufferPtr += 4;
  }

  number = quarterPoints * 4;
  for(; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
    *qBufferPtr++ = *complexVectorPtr++;
  }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Deinterleaves the complex vector into I & Q vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param qBuffer The Q buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_32fc_deinterleave_32f_x2_a_generic(float* iBuffer, float* qBuffer, const lv_32fc_t* complexVector, unsigned int num_points){
  const float* complexVectorPtr = (float*)complexVector;
  float* iBufferPtr = iBuffer;
  float* qBufferPtr = qBuffer;
  unsigned int number;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
    *qBufferPtr++ = *complexVectorPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32fc_deinterleave_32f_x2_a_H */

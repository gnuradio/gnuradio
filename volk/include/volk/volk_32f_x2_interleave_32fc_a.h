#ifndef INCLUDED_volk_32f_x2_interleave_32fc_a_H
#define INCLUDED_volk_32f_x2_interleave_32fc_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Interleaves the I & Q vector data into the complex vector
  \param iBuffer The I buffer data to be interleaved
  \param qBuffer The Q buffer data to be interleaved
  \param complexVector The complex output vector
  \param num_points The number of complex data values to be interleaved
*/
static inline void volk_32f_x2_interleave_32fc_a_sse(lv_32fc_t* complexVector, const float* iBuffer, const float* qBuffer, unsigned int num_points){
  unsigned int number = 0;
  float* complexVectorPtr = (float*)complexVector;
  const float* iBufferPtr = iBuffer;
  const float* qBufferPtr = qBuffer;

  const uint64_t quarterPoints = num_points / 4;
    
  __m128 iValue, qValue, cplxValue;
  for(;number < quarterPoints; number++){
    iValue = _mm_load_ps(iBufferPtr);
    qValue = _mm_load_ps(qBufferPtr);

    // Interleaves the lower two values in the i and q variables into one buffer
    cplxValue = _mm_unpacklo_ps(iValue, qValue);
    _mm_store_ps(complexVectorPtr, cplxValue);
    complexVectorPtr += 4;

    // Interleaves the upper two values in the i and q variables into one buffer
    cplxValue = _mm_unpackhi_ps(iValue, qValue);
    _mm_store_ps(complexVectorPtr, cplxValue);
    complexVectorPtr += 4;

    iBufferPtr += 4;
    qBufferPtr += 4;
  }

  number = quarterPoints * 4;
  for(; number < num_points; number++){
    *complexVectorPtr++ = *iBufferPtr++;
    *complexVectorPtr++ = *qBufferPtr++;
  }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Interleaves the I & Q vector data into the complex vector.
  \param iBuffer The I buffer data to be interleaved
  \param qBuffer The Q buffer data to be interleaved
  \param complexVector The complex output vector
  \param num_points The number of complex data values to be interleaved
*/
static inline void volk_32f_x2_interleave_32fc_a_generic(lv_32fc_t* complexVector, const float* iBuffer, const float* qBuffer, unsigned int num_points){
  float* complexVectorPtr = (float*)complexVector;
  const float* iBufferPtr = iBuffer;
  const float* qBufferPtr = qBuffer;
  unsigned int number;

  for(number = 0; number < num_points; number++){
    *complexVectorPtr++ = *iBufferPtr++;
    *complexVectorPtr++ = *qBufferPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32f_x2_interleave_32fc_a_H */

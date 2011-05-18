#ifndef INCLUDED_volk_32fc_deinterleave_real_32f_a_H
#define INCLUDED_volk_32fc_deinterleave_real_32f_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Deinterleaves the complex vector into I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_32fc_deinterleave_real_32f_a_sse(float* iBuffer, const lv_32fc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;

  const float* complexVectorPtr = (const float*)complexVector;
  float* iBufferPtr = iBuffer;

  __m128 cplxValue1, cplxValue2, iValue;
  for(;number < quarterPoints; number++){
      
    cplxValue1 = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;

    cplxValue2 = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;

    // Arrange in i1i2i3i4 format
    iValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(2,0,2,0));

    _mm_store_ps(iBufferPtr, iValue);

    iBufferPtr += 4;
  }

  number = quarterPoints * 4;
  for(; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
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
static inline void volk_32fc_deinterleave_real_32f_a_generic(float* iBuffer, const lv_32fc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const float* complexVectorPtr = (float*)complexVector;
  float* iBufferPtr = iBuffer;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32fc_deinterleave_real_32f_a_H */

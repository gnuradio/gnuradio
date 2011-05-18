#ifndef INCLUDED_volk_32fc_deinterleave_real_64f_a_H
#define INCLUDED_volk_32fc_deinterleave_real_64f_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
/*!
  \brief Deinterleaves the complex vector into I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_32fc_deinterleave_real_64f_a_sse2(double* iBuffer, const lv_32fc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;

  const float* complexVectorPtr = (float*)complexVector;
  double* iBufferPtr = iBuffer;

  const unsigned int halfPoints = num_points / 2;    
  __m128 cplxValue, fVal;
  __m128d dVal;
  for(;number < halfPoints; number++){
      
    cplxValue = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;

    // Arrange in i1i2i1i2 format
    fVal = _mm_shuffle_ps(cplxValue, cplxValue, _MM_SHUFFLE(2,0,2,0));
    dVal = _mm_cvtps_pd(fVal); 
    _mm_store_pd(iBufferPtr, dVal);

    iBufferPtr += 2;
  }

  number = halfPoints * 2;
  for(; number < num_points; number++){
    *iBufferPtr++ = (double)*complexVectorPtr++;
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
static inline void volk_32fc_deinterleave_real_64f_a_generic(double* iBuffer, const lv_32fc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const float* complexVectorPtr = (float*)complexVector;
  double* iBufferPtr = iBuffer;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = (double)*complexVectorPtr++;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32fc_deinterleave_real_64f_a_H */

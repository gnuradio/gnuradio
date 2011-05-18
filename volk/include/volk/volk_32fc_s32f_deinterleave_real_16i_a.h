#ifndef INCLUDED_volk_32fc_s32f_deinterleave_real_16i_a_H
#define INCLUDED_volk_32fc_s32f_deinterleave_real_16i_a_H

#include <volk/volk_common.h>
#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Deinterleaves the complex vector, multiply the value by the scalar, convert to 16t, and in I vector data
  \param complexVector The complex input vector
  \param scalar The value to be multiply against each of the input values
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_32fc_s32f_deinterleave_real_16i_a_sse(int16_t* iBuffer, const lv_32fc_t* complexVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;

  const float* complexVectorPtr = (float*)complexVector;
  int16_t* iBufferPtr = iBuffer;

  __m128 vScalar = _mm_set_ps1(scalar);

  __m128 cplxValue1, cplxValue2, iValue;

  __VOLK_ATTR_ALIGNED(16) float floatBuffer[4];

  for(;number < quarterPoints; number++){
    cplxValue1 = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;

    cplxValue2 = _mm_load_ps(complexVectorPtr);
    complexVectorPtr += 4;

    // Arrange in i1i2i3i4 format
    iValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(2,0,2,0));

    iValue = _mm_mul_ps(iValue, vScalar);

    _mm_store_ps(floatBuffer, iValue);
    *iBufferPtr++ = (int16_t)(floatBuffer[0]);
    *iBufferPtr++ = (int16_t)(floatBuffer[1]);
    *iBufferPtr++ = (int16_t)(floatBuffer[2]);
    *iBufferPtr++ = (int16_t)(floatBuffer[3]);
  }

  number = quarterPoints * 4;
  iBufferPtr = &iBuffer[number];
  for(; number < num_points; number++){
    *iBufferPtr++ = (int16_t)(*complexVectorPtr++ * scalar);
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Deinterleaves the complex vector, multiply the value by the scalar, convert to 16t, and in I vector data
  \param complexVector The complex input vector
  \param scalar The value to be multiply against each of the input values
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_32fc_s32f_deinterleave_real_16i_a_generic(int16_t* iBuffer, const lv_32fc_t* complexVector, const float scalar, unsigned int num_points){
  const float* complexVectorPtr = (float*)complexVector;
  int16_t* iBufferPtr = iBuffer;
  unsigned int number = 0;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = (int16_t)(*complexVectorPtr++ * scalar);
    complexVectorPtr++;
  }

}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32fc_s32f_deinterleave_real_16i_a_H */

#ifndef INCLUDED_volk_32f_x2_s32f_interleave_16ic_a_H
#define INCLUDED_volk_32f_x2_s32f_interleave_16ic_a_H

#include <volk/volk_common.h>
#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
  /*!
    \brief Interleaves the I & Q vector data into the complex vector, scales the output values by the scalar, and converts to 16 bit data.
    \param iBuffer The I buffer data to be interleaved
    \param qBuffer The Q buffer data to be interleaved
    \param complexVector The complex output vector
    \param scalar The scaling value being multiplied against each data point
    \param num_points The number of complex data values to be interleaved
  */
static inline void volk_32f_x2_s32f_interleave_16ic_a_sse2(lv_16sc_t* complexVector, const float* iBuffer, const float* qBuffer, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const float* iBufferPtr = iBuffer;
    const float* qBufferPtr = qBuffer;

    __m128 vScalar = _mm_set_ps1(scalar);

    const unsigned int quarterPoints = num_points / 4;
    
    __m128 iValue, qValue, cplxValue1, cplxValue2;
    __m128i intValue1, intValue2;

    int16_t* complexVectorPtr = (int16_t*)complexVector;

    for(;number < quarterPoints; number++){
      iValue = _mm_load_ps(iBufferPtr);
      qValue = _mm_load_ps(qBufferPtr);

      // Interleaves the lower two values in the i and q variables into one buffer
      cplxValue1 = _mm_unpacklo_ps(iValue, qValue);
      cplxValue1 = _mm_mul_ps(cplxValue1, vScalar);

      // Interleaves the upper two values in the i and q variables into one buffer
      cplxValue2 = _mm_unpackhi_ps(iValue, qValue);
      cplxValue2 = _mm_mul_ps(cplxValue2, vScalar);

      intValue1 = _mm_cvtps_epi32(cplxValue1);
      intValue2 = _mm_cvtps_epi32(cplxValue2);

      intValue1 = _mm_packs_epi32(intValue1, intValue2);

      _mm_store_si128((__m128i*)complexVectorPtr, intValue1);
      complexVectorPtr += 8;

      iBufferPtr += 4;
      qBufferPtr += 4;
    }

    number = quarterPoints * 4;
    complexVectorPtr = (int16_t*)(&complexVector[number]);
    for(; number < num_points; number++){
      *complexVectorPtr++ = (int16_t)(*iBufferPtr++ * scalar);
      *complexVectorPtr++ = (int16_t)(*qBufferPtr++ * scalar);
    }
    
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
  /*!
    \brief Interleaves the I & Q vector data into the complex vector, scales the output values by the scalar, and converts to 16 bit data.
    \param iBuffer The I buffer data to be interleaved
    \param qBuffer The Q buffer data to be interleaved
    \param complexVector The complex output vector
    \param scalar The scaling value being multiplied against each data point
    \param num_points The number of complex data values to be interleaved
  */
static inline void volk_32f_x2_s32f_interleave_16ic_a_sse(lv_16sc_t* complexVector, const float* iBuffer, const float* qBuffer, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const float* iBufferPtr = iBuffer;
    const float* qBufferPtr = qBuffer;

    __m128 vScalar = _mm_set_ps1(scalar);

    const unsigned int quarterPoints = num_points / 4;
    
    __m128 iValue, qValue, cplxValue;

    int16_t* complexVectorPtr = (int16_t*)complexVector;

    __VOLK_ATTR_ALIGNED(16) float floatBuffer[4];

    for(;number < quarterPoints; number++){
      iValue = _mm_load_ps(iBufferPtr);
      qValue = _mm_load_ps(qBufferPtr);

      // Interleaves the lower two values in the i and q variables into one buffer
      cplxValue = _mm_unpacklo_ps(iValue, qValue);
      cplxValue = _mm_mul_ps(cplxValue, vScalar);

      _mm_store_ps(floatBuffer, cplxValue);

      *complexVectorPtr++ = (int16_t)(floatBuffer[0]);
      *complexVectorPtr++ = (int16_t)(floatBuffer[1]);
      *complexVectorPtr++ = (int16_t)(floatBuffer[2]);
      *complexVectorPtr++ = (int16_t)(floatBuffer[3]);

      // Interleaves the upper two values in the i and q variables into one buffer
      cplxValue = _mm_unpackhi_ps(iValue, qValue);
      cplxValue = _mm_mul_ps(cplxValue, vScalar);
 
      _mm_store_ps(floatBuffer, cplxValue);
      
      *complexVectorPtr++ = (int16_t)(floatBuffer[0]);
      *complexVectorPtr++ = (int16_t)(floatBuffer[1]);
      *complexVectorPtr++ = (int16_t)(floatBuffer[2]);
      *complexVectorPtr++ = (int16_t)(floatBuffer[3]);

      iBufferPtr += 4;
      qBufferPtr += 4;
    }

    number = quarterPoints * 4;
    complexVectorPtr = (int16_t*)(&complexVector[number]);
    for(; number < num_points; number++){
      *complexVectorPtr++ = (int16_t)(*iBufferPtr++ * scalar);
      *complexVectorPtr++ = (int16_t)(*qBufferPtr++ * scalar);
    }
    
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Interleaves the I & Q vector data into the complex vector, scales the output values by the scalar, and converts to 16 bit data.
    \param iBuffer The I buffer data to be interleaved
    \param qBuffer The Q buffer data to be interleaved
    \param complexVector The complex output vector
    \param scalar The scaling value being multiplied against each data point
    \param num_points The number of complex data values to be interleaved
  */
static inline void volk_32f_x2_s32f_interleave_16ic_a_generic(lv_16sc_t* complexVector, const float* iBuffer, const float* qBuffer, const float scalar, unsigned int num_points){
  int16_t* complexVectorPtr = (int16_t*)complexVector;
  const float* iBufferPtr = iBuffer;
  const float* qBufferPtr = qBuffer;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *complexVectorPtr++ = (int16_t)(*iBufferPtr++ * scalar);
    *complexVectorPtr++ = (int16_t)(*qBufferPtr++ * scalar);
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32f_x2_s32f_interleave_16ic_a_H */

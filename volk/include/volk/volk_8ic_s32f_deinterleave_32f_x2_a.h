#ifndef INCLUDED_volk_8ic_s32f_deinterleave_32f_x2_a_H
#define INCLUDED_volk_8ic_s32f_deinterleave_32f_x2_a_H

#include <volk/volk_common.h>
#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Deinterleaves the complex 8 bit vector into I & Q floating point vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param qBuffer The Q buffer output data
  \param scalar The scaling value being multiplied against each data point
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_s32f_deinterleave_32f_x2_a_sse4_1(float* iBuffer, float* qBuffer, const lv_8sc_t* complexVector, const float scalar, unsigned int num_points){
  float* iBufferPtr = iBuffer;
  float* qBufferPtr = qBuffer;

  unsigned int number = 0;
  const unsigned int eighthPoints = num_points / 8;    
  __m128 iFloatValue, qFloatValue;

  const float iScalar= 1.0 / scalar;
  __m128 invScalar = _mm_set_ps1(iScalar);
  __m128i complexVal, iIntVal, qIntVal, iComplexVal, qComplexVal;
  int8_t* complexVectorPtr = (int8_t*)complexVector;

  __m128i iMoveMask = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0);
  __m128i qMoveMask = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 15, 13, 11, 9, 7, 5, 3, 1);

  for(;number < eighthPoints; number++){
    complexVal = _mm_load_si128((__m128i*)complexVectorPtr); complexVectorPtr += 16;
    iComplexVal = _mm_shuffle_epi8(complexVal, iMoveMask);
    qComplexVal = _mm_shuffle_epi8(complexVal, qMoveMask);

    iIntVal = _mm_cvtepi8_epi32(iComplexVal);
    iFloatValue = _mm_cvtepi32_ps(iIntVal);
    iFloatValue = _mm_mul_ps(iFloatValue, invScalar);
    _mm_store_ps(iBufferPtr, iFloatValue);
    iBufferPtr += 4;

    iComplexVal = _mm_srli_si128(iComplexVal, 4);

    iIntVal = _mm_cvtepi8_epi32(iComplexVal);
    iFloatValue = _mm_cvtepi32_ps(iIntVal);
    iFloatValue = _mm_mul_ps(iFloatValue, invScalar);
    _mm_store_ps(iBufferPtr, iFloatValue);
    iBufferPtr += 4;

    qIntVal = _mm_cvtepi8_epi32(qComplexVal);
    qFloatValue = _mm_cvtepi32_ps(qIntVal);
    qFloatValue = _mm_mul_ps(qFloatValue, invScalar);
    _mm_store_ps(qBufferPtr, qFloatValue);
    qBufferPtr += 4;

    qComplexVal = _mm_srli_si128(qComplexVal, 4);

    qIntVal = _mm_cvtepi8_epi32(qComplexVal);
    qFloatValue = _mm_cvtepi32_ps(qIntVal);
    qFloatValue = _mm_mul_ps(qFloatValue, invScalar);
    _mm_store_ps(qBufferPtr, qFloatValue);

    qBufferPtr += 4;
  }

  number = eighthPoints * 8;
  for(; number < num_points; number++){
    *iBufferPtr++ = (float)(*complexVectorPtr++) * iScalar;
    *qBufferPtr++ = (float)(*complexVectorPtr++) * iScalar;
  }
    
}
#endif /* LV_HAVE_SSE4_1 */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Deinterleaves the complex 8 bit vector into I & Q floating point vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param qBuffer The Q buffer output data
  \param scalar The scaling value being multiplied against each data point
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_s32f_deinterleave_32f_x2_a_sse(float* iBuffer, float* qBuffer, const lv_8sc_t* complexVector, const float scalar, unsigned int num_points){
  float* iBufferPtr = iBuffer;
  float* qBufferPtr = qBuffer;

  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;    
  __m128 cplxValue1, cplxValue2, iValue, qValue;

  __m128 invScalar = _mm_set_ps1(1.0/scalar);
  int8_t* complexVectorPtr = (int8_t*)complexVector;

  __VOLK_ATTR_ALIGNED(16) float floatBuffer[8];

  for(;number < quarterPoints; number++){
    floatBuffer[0] = (float)(complexVectorPtr[0]);
    floatBuffer[1] = (float)(complexVectorPtr[1]);
    floatBuffer[2] = (float)(complexVectorPtr[2]);
    floatBuffer[3] = (float)(complexVectorPtr[3]);
      
    floatBuffer[4] = (float)(complexVectorPtr[4]);
    floatBuffer[5] = (float)(complexVectorPtr[5]);
    floatBuffer[6] = (float)(complexVectorPtr[6]);
    floatBuffer[7] = (float)(complexVectorPtr[7]);

    cplxValue1 = _mm_load_ps(&floatBuffer[0]);
    cplxValue2 = _mm_load_ps(&floatBuffer[4]);

    complexVectorPtr += 8;

    cplxValue1 = _mm_mul_ps(cplxValue1, invScalar);
    cplxValue2 = _mm_mul_ps(cplxValue2, invScalar);

    // Arrange in i1i2i3i4 format
    iValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(2,0,2,0));
    qValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(3,1,3,1));

    _mm_store_ps(iBufferPtr, iValue);
    _mm_store_ps(qBufferPtr, qValue);

    iBufferPtr += 4;
    qBufferPtr += 4;
  }

  number = quarterPoints * 4;
  complexVectorPtr = (int8_t*)&complexVector[number];
  for(; number < num_points; number++){
    *iBufferPtr++ = (float)(*complexVectorPtr++) / scalar;
    *qBufferPtr++ = (float)(*complexVectorPtr++) / scalar;
  }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Deinterleaves the complex 8 bit vector into I & Q floating point vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param qBuffer The Q buffer output data
  \param scalar The scaling value being multiplied against each data point
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_s32f_deinterleave_32f_x2_a_generic(float* iBuffer, float* qBuffer, const lv_8sc_t* complexVector, const float scalar, unsigned int num_points){
  const int8_t* complexVectorPtr = (const int8_t*)complexVector;
  float* iBufferPtr = iBuffer;
  float* qBufferPtr = qBuffer;
  unsigned int number;
  const float invScalar = 1.0 / scalar;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = (float)(*complexVectorPtr++)*invScalar;
    *qBufferPtr++ = (float)(*complexVectorPtr++)*invScalar;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_8ic_s32f_deinterleave_32f_x2_a_H */

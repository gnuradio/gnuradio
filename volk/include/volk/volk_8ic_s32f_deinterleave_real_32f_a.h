#ifndef INCLUDED_volk_8ic_s32f_deinterleave_real_32f_a_H
#define INCLUDED_volk_8ic_s32f_deinterleave_real_32f_a_H

#include <volk/volk_common.h>
#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Deinterleaves the complex 8 bit vector into I float vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param scalar The scaling value being multiplied against each data point
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_s32f_deinterleave_real_32f_a_sse4_1(float* iBuffer, const lv_8sc_t* complexVector, const float scalar, unsigned int num_points){
  float* iBufferPtr = iBuffer;

  unsigned int number = 0;
  const unsigned int eighthPoints = num_points / 8;    
  __m128 iFloatValue;

  const float iScalar= 1.0 / scalar;
  __m128 invScalar = _mm_set_ps1(iScalar);
  __m128i complexVal, iIntVal;
  int8_t* complexVectorPtr = (int8_t*)complexVector;

  __m128i moveMask = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0);

  for(;number < eighthPoints; number++){
    complexVal = _mm_load_si128((__m128i*)complexVectorPtr); complexVectorPtr += 16;
    complexVal = _mm_shuffle_epi8(complexVal, moveMask);

    iIntVal = _mm_cvtepi8_epi32(complexVal);
    iFloatValue = _mm_cvtepi32_ps(iIntVal);

    iFloatValue = _mm_mul_ps(iFloatValue, invScalar);

    _mm_store_ps(iBufferPtr, iFloatValue);

    iBufferPtr += 4;

    complexVal = _mm_srli_si128(complexVal, 4);
    iIntVal = _mm_cvtepi8_epi32(complexVal);
    iFloatValue = _mm_cvtepi32_ps(iIntVal);

    iFloatValue = _mm_mul_ps(iFloatValue, invScalar);

    _mm_store_ps(iBufferPtr, iFloatValue);

    iBufferPtr += 4;
  }

  number = eighthPoints * 8;
  for(; number < num_points; number++){
    *iBufferPtr++ = (float)(*complexVectorPtr++) * iScalar;
    complexVectorPtr++;
  }
    
}
#endif /* LV_HAVE_SSE4_1 */


#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Deinterleaves the complex 8 bit vector into I float vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param scalar The scaling value being multiplied against each data point
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_s32f_deinterleave_real_32f_a_sse(float* iBuffer, const lv_8sc_t* complexVector, const float scalar, unsigned int num_points){
  float* iBufferPtr = iBuffer;

  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;    
  __m128 iValue;

  const float iScalar= 1.0 / scalar;
  __m128 invScalar = _mm_set_ps1(iScalar);
  int8_t* complexVectorPtr = (int8_t*)complexVector;

  __VOLK_ATTR_ALIGNED(16) float floatBuffer[4];

  for(;number < quarterPoints; number++){
    floatBuffer[0] = (float)(*complexVectorPtr); complexVectorPtr += 2;
    floatBuffer[1] = (float)(*complexVectorPtr); complexVectorPtr += 2;
    floatBuffer[2] = (float)(*complexVectorPtr); complexVectorPtr += 2;
    floatBuffer[3] = (float)(*complexVectorPtr); complexVectorPtr += 2; 

    iValue = _mm_load_ps(floatBuffer);

    iValue = _mm_mul_ps(iValue, invScalar);

    _mm_store_ps(iBufferPtr, iValue);

    iBufferPtr += 4;
  }

  number = quarterPoints * 4;
  for(; number < num_points; number++){
    *iBufferPtr++ = (float)(*complexVectorPtr++) * iScalar;
    complexVectorPtr++;
  }
    
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Deinterleaves the complex 8 bit vector into I float vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param scalar The scaling value being multiplied against each data point
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_s32f_deinterleave_real_32f_a_generic(float* iBuffer, const lv_8sc_t* complexVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;
  const int8_t* complexVectorPtr = (const int8_t*)complexVector;
  float* iBufferPtr = iBuffer;
  const float invScalar = 1.0 / scalar;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = ((float)(*complexVectorPtr++)) * invScalar;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_8ic_s32f_deinterleave_real_32f_a_H */

#ifndef INCLUDED_volk_16ic_s32f_deinterleave_32f_x2_a_H
#define INCLUDED_volk_16ic_s32f_deinterleave_32f_x2_a_H

#include <volk/volk_common.h>
#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
  /*!
    \brief Converts the complex 16 bit vector into floats,scales each data point, and deinterleaves into I & Q vector data
    \param complexVector The complex input vector
    \param iBuffer The I buffer output data
    \param qBuffer The Q buffer output data
    \param scalar The data value to be divided against each input data value of the input complex vector
    \param num_points The number of complex data values to be deinterleaved
  */
static inline void volk_16ic_s32f_deinterleave_32f_x2_a_sse(float* iBuffer, float* qBuffer, const lv_16sc_t* complexVector, const float scalar, unsigned int num_points){
    float* iBufferPtr = iBuffer;
    float* qBufferPtr = qBuffer;

    uint64_t number = 0;
    const uint64_t quarterPoints = num_points / 4;    
    __m128 cplxValue1, cplxValue2, iValue, qValue;

    __m128 invScalar = _mm_set_ps1(1.0/scalar);
    int16_t* complexVectorPtr = (int16_t*)complexVector;

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
      // Arrange in q1q2q3q4 format
      qValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(3,1,3,1));

      _mm_store_ps(iBufferPtr, iValue);
      _mm_store_ps(qBufferPtr, qValue);

      iBufferPtr += 4;
      qBufferPtr += 4;
    }

    number = quarterPoints * 4;
    complexVectorPtr = (int16_t*)&complexVector[number];
    for(; number < num_points; number++){
      *iBufferPtr++ = (float)(*complexVectorPtr++) / scalar;
      *qBufferPtr++ = (float)(*complexVectorPtr++) / scalar;
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Converts the complex 16 bit vector into floats,scales each data point, and deinterleaves into I & Q vector data
    \param complexVector The complex input vector
    \param iBuffer The I buffer output data
    \param qBuffer The Q buffer output data
    \param scalar The data value to be divided against each input data value of the input complex vector
    \param num_points The number of complex data values to be deinterleaved
  */
static inline void volk_16ic_s32f_deinterleave_32f_x2_a_generic(float* iBuffer, float* qBuffer, const lv_16sc_t* complexVector, const float scalar, unsigned int num_points){
  const int16_t* complexVectorPtr = (const int16_t*)complexVector;
  float* iBufferPtr = iBuffer;
  float* qBufferPtr = qBuffer;
  unsigned int number;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = (float)(*complexVectorPtr++) / scalar;
    *qBufferPtr++ = (float)(*complexVectorPtr++) / scalar;
  }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_ORC
  /*!
    \brief Converts the complex 16 bit vector into floats,scales each data point, and deinterleaves into I & Q vector data
    \param complexVector The complex input vector
    \param iBuffer The I buffer output data
    \param qBuffer The Q buffer output data
    \param scalar The data value to be divided against each input data value of the input complex vector
    \param num_points The number of complex data values to be deinterleaved
  */
extern void volk_16ic_s32f_deinterleave_32f_x2_a_orc_impl(float* iBuffer, float* qBuffer, const lv_16sc_t* complexVector, const float scalar, unsigned int num_points);
static inline void volk_16ic_s32f_deinterleave_32f_x2_a_orc(float* iBuffer, float* qBuffer, const lv_16sc_t* complexVector, const float scalar, unsigned int num_points){
    volk_16ic_s32f_deinterleave_32f_x2_a_orc_impl(iBuffer, qBuffer, complexVector, scalar, num_points);
}
#endif /* LV_HAVE_ORC */


#endif /* INCLUDED_volk_16ic_s32f_deinterleave_32f_x2_a_H */

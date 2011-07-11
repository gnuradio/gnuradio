#ifndef INCLUDED_volk_8ic_deinterleave_16i_x2_a_H
#define INCLUDED_volk_8ic_deinterleave_16i_x2_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Deinterleaves the complex 8 bit vector into I & Q 16 bit vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param qBuffer The Q buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_deinterleave_16i_x2_a_sse4_1(int16_t* iBuffer, int16_t* qBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int8_t* complexVectorPtr = (int8_t*)complexVector;
  int16_t* iBufferPtr = iBuffer;
  int16_t* qBufferPtr = qBuffer;
  __m128i iMoveMask = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0);
  __m128i qMoveMask = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 15, 13, 11, 9, 7, 5, 3, 1);
  __m128i complexVal, iOutputVal, qOutputVal;

  unsigned int eighthPoints = num_points / 8;

  for(number = 0; number < eighthPoints; number++){
    complexVal = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 16;

    iOutputVal = _mm_shuffle_epi8(complexVal, iMoveMask);
    qOutputVal = _mm_shuffle_epi8(complexVal, qMoveMask);

    iOutputVal = _mm_cvtepi8_epi16(iOutputVal);
    iOutputVal = _mm_slli_epi16(iOutputVal, 8);

    qOutputVal = _mm_cvtepi8_epi16(qOutputVal);
    qOutputVal = _mm_slli_epi16(qOutputVal, 8);

    _mm_store_si128((__m128i*)iBufferPtr, iOutputVal);
    _mm_store_si128((__m128i*)qBufferPtr, qOutputVal);

    iBufferPtr += 8;
    qBufferPtr += 8;
  }

  number = eighthPoints * 8;
  for(; number < num_points; number++){
    *iBufferPtr++ = ((int16_t)*complexVectorPtr++) * 256;
    *qBufferPtr++ = ((int16_t)*complexVectorPtr++) * 256;
  }
}
#endif /* LV_HAVE_SSE4_1 */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Deinterleaves the complex 8 bit vector into I & Q 16 bit vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param qBuffer The Q buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_deinterleave_16i_x2_a_generic(int16_t* iBuffer, int16_t* qBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
  const int8_t* complexVectorPtr = (const int8_t*)complexVector;
  int16_t* iBufferPtr = iBuffer;
  int16_t* qBufferPtr = qBuffer;
  unsigned int number;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = (int16_t)(*complexVectorPtr++)*256;
    *qBufferPtr++ = (int16_t)(*complexVectorPtr++)*256;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_8ic_deinterleave_16i_x2_a_H */

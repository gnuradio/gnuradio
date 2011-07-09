#ifndef INCLUDED_VOLK_8sc_DEINTERLEAVE_REAL_8s_ALIGNED8_H
#define INCLUDED_VOLK_8sc_DEINTERLEAVE_REAL_8s_ALIGNED8_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSSE3
#include <tmmintrin.h>
/*!
  \brief Deinterleaves the complex 8 bit vector into I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_deinterleave_real_8i_a_ssse3(int8_t* iBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int8_t* complexVectorPtr = (int8_t*)complexVector;
  int8_t* iBufferPtr = iBuffer;
  __m128i moveMask1 = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0);
  __m128i moveMask2 = _mm_set_epi8(14, 12, 10, 8, 6, 4, 2, 0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
  __m128i complexVal1, complexVal2, outputVal;

  unsigned int sixteenthPoints = num_points / 16;

  for(number = 0; number < sixteenthPoints; number++){
    complexVal1 = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 16;
    complexVal2 = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 16;

    complexVal1 = _mm_shuffle_epi8(complexVal1, moveMask1);
    complexVal2 = _mm_shuffle_epi8(complexVal2, moveMask2);

    outputVal = _mm_or_si128(complexVal1, complexVal2);

    _mm_store_si128((__m128i*)iBufferPtr, outputVal);
    iBufferPtr += 16;
  }

  number = sixteenthPoints * 16;
  for(; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_SSSE3 */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Deinterleaves the complex 8 bit vector into I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_deinterleave_real_8i_a_generic(int8_t* iBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int8_t* complexVectorPtr = (int8_t*)complexVector;
  int8_t* iBufferPtr = iBuffer;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_VOLK_8sc_DEINTERLEAVE_REAL_8s_ALIGNED8_H */

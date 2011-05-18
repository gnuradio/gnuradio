#ifndef INCLUDED_volk_16ic_deinterleave_real_16i_a_H
#define INCLUDED_volk_16ic_deinterleave_real_16i_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSSE3
#include <tmmintrin.h>
/*!
  \brief Deinterleaves the complex 16 bit vector into I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_16ic_deinterleave_real_16i_a_ssse3(int16_t* iBuffer, const lv_16sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int16_t* complexVectorPtr = (int16_t*)complexVector;
  int16_t* iBufferPtr = iBuffer;

  __m128i iMoveMask1 = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 13, 12, 9, 8, 5, 4, 1, 0);
  __m128i iMoveMask2 = _mm_set_epi8(13, 12, 9, 8, 5, 4, 1, 0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80);

  __m128i complexVal1, complexVal2, iOutputVal;

  unsigned int eighthPoints = num_points / 8;

  for(number = 0; number < eighthPoints; number++){
    complexVal1 = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 8;
    complexVal2 = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 8;

    complexVal1 = _mm_shuffle_epi8(complexVal1, iMoveMask1);
    complexVal2 = _mm_shuffle_epi8(complexVal2, iMoveMask2);

    iOutputVal = _mm_or_si128(complexVal1, complexVal2);

    _mm_store_si128((__m128i*)iBufferPtr, iOutputVal);

    iBufferPtr += 8;
  }

  number = eighthPoints * 8;
  for(; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_SSSE3 */


#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
/*!
  \brief Deinterleaves the complex 16 bit vector into I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_16ic_deinterleave_real_16i_a_sse2(int16_t* iBuffer, const lv_16sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int16_t* complexVectorPtr = (int16_t*)complexVector;
  int16_t* iBufferPtr = iBuffer;
  __m128i complexVal1, complexVal2, iOutputVal;
  __m128i lowMask = _mm_set_epi32(0x0, 0x0, 0xFFFFFFFF, 0xFFFFFFFF);
  __m128i highMask = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0x0, 0x0);

  unsigned int eighthPoints = num_points / 8;
 
  for(number = 0; number < eighthPoints; number++){
    complexVal1 = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 8;
    complexVal2 = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 8;

    complexVal1 = _mm_shufflelo_epi16(complexVal1, _MM_SHUFFLE(3,1,2,0));

    complexVal1 = _mm_shufflehi_epi16(complexVal1, _MM_SHUFFLE(3,1,2,0));

    complexVal1 = _mm_shuffle_epi32(complexVal1, _MM_SHUFFLE(3,1,2,0));

    complexVal2 = _mm_shufflelo_epi16(complexVal2, _MM_SHUFFLE(3,1,2,0));

    complexVal2 = _mm_shufflehi_epi16(complexVal2, _MM_SHUFFLE(3,1,2,0));

    complexVal2 = _mm_shuffle_epi32(complexVal2, _MM_SHUFFLE(2,0,3,1));

    iOutputVal = _mm_or_si128(_mm_and_si128(complexVal1, lowMask), _mm_and_si128(complexVal2, highMask));

    _mm_store_si128((__m128i*)iBufferPtr, iOutputVal);

    iBufferPtr += 8;
  }

  number = eighthPoints * 8;
  for(; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Deinterleaves the complex 16 bit vector into I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_16ic_deinterleave_real_16i_a_generic(int16_t* iBuffer, const lv_16sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int16_t* complexVectorPtr = (int16_t*)complexVector;
  int16_t* iBufferPtr = iBuffer;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = *complexVectorPtr++;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_16ic_deinterleave_real_16i_a_H */

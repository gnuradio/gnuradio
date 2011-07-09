#ifndef INCLUDED_volk_16ic_deinterleave_real_8i_a_H
#define INCLUDED_volk_16ic_deinterleave_real_8i_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSSE3
#include <tmmintrin.h>
/*!
  \brief Deinterleaves the complex 16 bit vector into 8 bit I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_16ic_deinterleave_real_8i_a_ssse3(int8_t* iBuffer, const lv_16sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int8_t* complexVectorPtr = (int8_t*)complexVector;
  int8_t* iBufferPtr = iBuffer;
  __m128i iMoveMask1 = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 13, 12, 9, 8, 5, 4, 1, 0);
  __m128i iMoveMask2 = _mm_set_epi8(13, 12, 9, 8, 5, 4, 1, 0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
  __m128i complexVal1, complexVal2, complexVal3, complexVal4, iOutputVal;

  unsigned int sixteenthPoints = num_points / 16;

  for(number = 0; number < sixteenthPoints; number++){
    complexVal1 = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 16;
    complexVal2 = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 16;

    complexVal3 = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 16;
    complexVal4 = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 16;

    complexVal1 = _mm_shuffle_epi8(complexVal1, iMoveMask1);
    complexVal2 = _mm_shuffle_epi8(complexVal2, iMoveMask2);

    complexVal1 = _mm_or_si128(complexVal1, complexVal2);

    complexVal3 = _mm_shuffle_epi8(complexVal3, iMoveMask1);
    complexVal4 = _mm_shuffle_epi8(complexVal4, iMoveMask2);

    complexVal3 = _mm_or_si128(complexVal3, complexVal4);


    complexVal1 = _mm_srai_epi16(complexVal1, 8);
    complexVal3 = _mm_srai_epi16(complexVal3, 8);

    iOutputVal = _mm_packs_epi16(complexVal1, complexVal3);

    _mm_store_si128((__m128i*)iBufferPtr, iOutputVal);

    iBufferPtr += 16;
  }

  number = sixteenthPoints * 16;
  int16_t* int16ComplexVectorPtr = (int16_t*)complexVectorPtr;
  for(; number < num_points; number++){
    *iBufferPtr++ = ((int8_t)(*int16ComplexVectorPtr++ >> 8));
    int16ComplexVectorPtr++;
  }
}
#endif /* LV_HAVE_SSSE3 */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Deinterleaves the complex 16 bit vector into 8 bit I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_16ic_deinterleave_real_8i_a_generic(int8_t* iBuffer, const lv_16sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  int16_t* complexVectorPtr = (int16_t*)complexVector;
  int8_t* iBufferPtr = iBuffer;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = ((int8_t)(*complexVectorPtr++ >> 8));
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_ORC
/*!
  \brief Deinterleaves the complex 16 bit vector into 8 bit I vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
extern void volk_16ic_deinterleave_real_8i_a_orc_impl(int8_t* iBuffer, const lv_16sc_t* complexVector, unsigned int num_points);
static inline void volk_16ic_deinterleave_real_8i_a_orc(int8_t* iBuffer, const lv_16sc_t* complexVector, unsigned int num_points){
    volk_16ic_deinterleave_real_8i_a_orc_impl(iBuffer, complexVector, num_points);
}
#endif /* LV_HAVE_ORC */


#endif /* INCLUDED_volk_16ic_deinterleave_real_8i_a_H */

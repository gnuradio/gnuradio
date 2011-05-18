#ifndef INCLUDED_volk_8ic_deinterleave_real_16i_a_H
#define INCLUDED_volk_8ic_deinterleave_real_16i_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Deinterleaves the complex 8 bit vector into I 16 bit vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_deinterleave_real_16i_a_sse4_1(int16_t* iBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int8_t* complexVectorPtr = (int8_t*)complexVector;
  int16_t* iBufferPtr = iBuffer;
  __m128i moveMask = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0);
  __m128i complexVal, outputVal;

  unsigned int eighthPoints = num_points / 8;

  for(number = 0; number < eighthPoints; number++){
    complexVal = _mm_load_si128((__m128i*)complexVectorPtr);  complexVectorPtr += 16;

    complexVal = _mm_shuffle_epi8(complexVal, moveMask);

    outputVal = _mm_cvtepi8_epi16(complexVal);
    outputVal = _mm_slli_epi16(outputVal, 7);

    _mm_store_si128((__m128i*)iBufferPtr, outputVal);
    iBufferPtr += 8;
  }

  number = eighthPoints * 8;
  for(; number < num_points; number++){
    *iBufferPtr++ = ((int16_t)*complexVectorPtr++) * 128;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_SSE4_1 */


#ifdef LV_HAVE_GENERIC
/*!
  \brief Deinterleaves the complex 8 bit vector into I 16 bit vector data
  \param complexVector The complex input vector
  \param iBuffer The I buffer output data
  \param num_points The number of complex data values to be deinterleaved
*/
static inline void volk_8ic_deinterleave_real_16i_a_generic(int16_t* iBuffer, const lv_8sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const int8_t* complexVectorPtr = (const int8_t*)complexVector;
  int16_t* iBufferPtr = iBuffer;
  for(number = 0; number < num_points; number++){
    *iBufferPtr++ = ((int16_t)(*complexVectorPtr++)) * 128;
    complexVectorPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_8ic_deinterleave_real_16i_a_H */

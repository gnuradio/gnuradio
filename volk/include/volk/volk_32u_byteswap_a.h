#ifndef INCLUDED_volk_32u_byteswap_a_H
#define INCLUDED_volk_32u_byteswap_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>

/*!
  \brief Byteswaps (in-place) an aligned vector of int32_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_32u_byteswap_a_sse2(uint32_t* intsToSwap, unsigned int num_points){
  unsigned int number = 0;

  uint32_t* inputPtr = intsToSwap;
  __m128i input, byte1, byte2, byte3, byte4, output;
  __m128i byte2mask = _mm_set1_epi32(0x00FF0000);
  __m128i byte3mask = _mm_set1_epi32(0x0000FF00);

  const uint64_t quarterPoints = num_points / 4;
  for(;number < quarterPoints; number++){
    // Load the 32t values, increment inputPtr later since we're doing it in-place.
    input = _mm_load_si128((__m128i*)inputPtr);
    // Do the four shifts
    byte1 = _mm_slli_epi32(input, 24);
    byte2 = _mm_slli_epi32(input, 8);
    byte3 = _mm_srli_epi32(input, 8);
    byte4 = _mm_srli_epi32(input, 24);
    // Or bytes together
    output = _mm_or_si128(byte1, byte4);
    byte2 = _mm_and_si128(byte2, byte2mask);
    output = _mm_or_si128(output, byte2);
    byte3 = _mm_and_si128(byte3, byte3mask);
    output = _mm_or_si128(output, byte3);
    // Store the results
    _mm_store_si128((__m128i*)inputPtr, output);
    inputPtr += 4;
  }
  
  // Byteswap any remaining points:
  number = quarterPoints*4;  
  for(; number < num_points; number++){
    uint32_t outputVal = *inputPtr;
    outputVal = (((outputVal >> 24) & 0xff) | ((outputVal >> 8) & 0x0000ff00) | ((outputVal << 8) & 0x00ff0000) | ((outputVal << 24) & 0xff000000));
    *inputPtr = outputVal;
    inputPtr++;
  }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Byteswaps (in-place) an aligned vector of int32_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_32u_byteswap_a_generic(uint32_t* intsToSwap, unsigned int num_points){
  uint32_t* inputPtr = intsToSwap;

  unsigned int point;
  for(point = 0; point < num_points; point++){
    uint32_t output = *inputPtr;
    output = (((output >> 24) & 0xff) | ((output >> 8) & 0x0000ff00) | ((output << 8) & 0x00ff0000) | ((output << 24) & 0xff000000));
    
    *inputPtr = output;
    inputPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32u_byteswap_a_H */

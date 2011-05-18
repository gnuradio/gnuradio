#ifndef INCLUDED_volk_64u_byteswap_a_H
#define INCLUDED_volk_64u_byteswap_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>

/*!
  \brief Byteswaps (in-place) an aligned vector of int64_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_64u_byteswap_a_sse2(uint64_t* intsToSwap, unsigned int num_points){
    uint32_t* inputPtr = (uint32_t*)intsToSwap;
    __m128i input, byte1, byte2, byte3, byte4, output;
    __m128i byte2mask = _mm_set1_epi32(0x00FF0000);
    __m128i byte3mask = _mm_set1_epi32(0x0000FF00);
    uint64_t number = 0;
    const unsigned int halfPoints = num_points / 2;
    for(;number < halfPoints; number++){
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
      
      // Reorder the two words
      output = _mm_shuffle_epi32(output, _MM_SHUFFLE(2, 3, 0, 1));

      // Store the results
      _mm_store_si128((__m128i*)inputPtr, output);
      inputPtr += 4;
    }
  
    // Byteswap any remaining points:
    number = halfPoints*2;  
    for(; number < num_points; number++){
      uint32_t output1 = *inputPtr;
      uint32_t output2 = inputPtr[1];
      
      output1 = (((output1 >> 24) & 0xff) | ((output1 >> 8) & 0x0000ff00) | ((output1 << 8) & 0x00ff0000) | ((output1 << 24) & 0xff000000));
      
      output2 = (((output2 >> 24) & 0xff) | ((output2 >> 8) & 0x0000ff00) | ((output2 << 8) & 0x00ff0000) | ((output2 << 24) & 0xff000000));
      
      *inputPtr++ = output2;
      *inputPtr++ = output1;
    }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Byteswaps (in-place) an aligned vector of int64_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_64u_byteswap_a_generic(uint64_t* intsToSwap, unsigned int num_points){
  uint32_t* inputPtr = (uint32_t*)intsToSwap;
  unsigned int point;
  for(point = 0; point < num_points; point++){
    uint32_t output1 = *inputPtr;
    uint32_t output2 = inputPtr[1];
    
    output1 = (((output1 >> 24) & 0xff) | ((output1 >> 8) & 0x0000ff00) | ((output1 << 8) & 0x00ff0000) | ((output1 << 24) & 0xff000000));
    
    output2 = (((output2 >> 24) & 0xff) | ((output2 >> 8) & 0x0000ff00) | ((output2 << 8) & 0x00ff0000) | ((output2 << 24) & 0xff000000));
    
    *inputPtr++ = output2;
    *inputPtr++ = output1;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_64u_byteswap_a_H */

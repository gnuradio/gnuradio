#ifndef INCLUDED_volk_16u_byteswap_a_H
#define INCLUDED_volk_16u_byteswap_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>

/*!
  \brief Byteswaps (in-place) an aligned vector of int16_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_16u_byteswap_a_sse2(uint16_t* intsToSwap, unsigned int num_points){
  unsigned int number = 0;
  uint16_t* inputPtr = intsToSwap;
  __m128i input, left, right, output;

  const unsigned int eighthPoints = num_points / 8;
  for(;number < eighthPoints; number++){
    // Load the 16t values, increment inputPtr later since we're doing it in-place.
    input = _mm_load_si128((__m128i*)inputPtr);
    // Do the two shifts
    left = _mm_slli_epi16(input, 8);
    right = _mm_srli_epi16(input, 8);
    // Or the left and right halves together
    output = _mm_or_si128(left, right);
    // Store the results
    _mm_store_si128((__m128i*)inputPtr, output);
    inputPtr += 8;
  }

  
  // Byteswap any remaining points:
  number = eighthPoints*8;  
  for(; number < num_points; number++){
    uint16_t outputVal = *inputPtr;
    outputVal = (((outputVal >> 8) & 0xff) | ((outputVal << 8) & 0xff00));
    *inputPtr = outputVal;
    inputPtr++;
  }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Byteswaps (in-place) an aligned vector of int16_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
static inline void volk_16u_byteswap_a_generic(uint16_t* intsToSwap, unsigned int num_points){
  unsigned int point;
  uint16_t* inputPtr = intsToSwap;
  for(point = 0; point < num_points; point++){
    uint16_t output = *inputPtr;
    output = (((output >> 8) & 0xff) | ((output << 8) & 0xff00));
    *inputPtr = output;
    inputPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_ORC
/*!
  \brief Byteswaps (in-place) an aligned vector of int16_t's.
  \param intsToSwap The vector of data to byte swap
  \param numDataPoints The number of data points
*/
extern void volk_16u_byteswap_a_orc_impl(uint16_t* intsToSwap, unsigned int num_points);
static inline void volk_16u_byteswap_a_orc(uint16_t* intsToSwap, unsigned int num_points){
    volk_16u_byteswap_a_orc_impl(intsToSwap, num_points);
}
#endif /* LV_HAVE_ORC */


#endif /* INCLUDED_volk_16u_byteswap_a_H */

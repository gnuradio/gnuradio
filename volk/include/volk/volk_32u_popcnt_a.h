#ifndef INCLUDED_VOLK_32u_POPCNT_A16_H
#define INCLUDED_VOLK_32u_POPCNT_A16_H

#include <stdio.h>
#include <inttypes.h>


#ifdef LV_HAVE_GENERIC

static inline void volk_32u_popcnt_a_generic(uint32_t* ret, const uint32_t value) {

  // This is faster than a lookup table
  uint32_t retVal = value;

  retVal = (retVal & 0x55555555) + (retVal >> 1 & 0x55555555);
  retVal = (retVal & 0x33333333) + (retVal >> 2 & 0x33333333);
  retVal = (retVal + (retVal >> 4)) & 0x0F0F0F0F;
  retVal = (retVal + (retVal >> 8));
  retVal = (retVal + (retVal >> 16)) & 0x0000003F;

  *ret = retVal;
}

#endif /*LV_HAVE_GENERIC*/

#ifdef LV_HAVE_SSE4_2

#include <nmmintrin.h>

static inline void volk_32u_popcnt_a_sse4_2(uint32_t* ret, const uint32_t value) {
  *ret = _mm_popcnt_u32(value);
}

#endif /*LV_HAVE_SSE4_2*/

#endif /*INCLUDED_VOLK_32u_POPCNT_A16_H*/

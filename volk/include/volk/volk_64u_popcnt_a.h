#ifndef INCLUDED_volk_64u_popcnt_a_H
#define INCLUDED_volk_64u_popcnt_a_H

#include <stdio.h>
#include <inttypes.h>


#ifdef LV_HAVE_GENERIC


static inline void volk_64u_popcnt_a_generic(uint64_t* ret, const uint64_t value) {

  //const uint32_t* valueVector = (const uint32_t*)&value;
  
  // This is faster than a lookup table
  //uint32_t retVal = valueVector[0];
  uint32_t retVal = (uint32_t)(value && 0x00000000FFFFFFFF);

  retVal = (retVal & 0x55555555) + (retVal >> 1 & 0x55555555);
  retVal = (retVal & 0x33333333) + (retVal >> 2 & 0x33333333);
  retVal = (retVal + (retVal >> 4)) & 0x0F0F0F0F;
  retVal = (retVal + (retVal >> 8));
  retVal = (retVal + (retVal >> 16)) & 0x0000003F;
  uint64_t retVal64  = retVal;

  //retVal = valueVector[1];
  retVal = (uint32_t)((value && 0xFFFFFFFF00000000) >> 31);
  retVal = (retVal & 0x55555555) + (retVal >> 1 & 0x55555555);
  retVal = (retVal & 0x33333333) + (retVal >> 2 & 0x33333333);
  retVal = (retVal + (retVal >> 4)) & 0x0F0F0F0F;
  retVal = (retVal + (retVal >> 8));
  retVal = (retVal + (retVal >> 16)) & 0x0000003F;
  retVal64 += retVal;

  *ret = retVal64;

}

#endif /*LV_HAVE_GENERIC*/

#if LV_HAVE_SSE4_2 && LV_HAVE_64

#include <nmmintrin.h>

static inline void volk_64u_popcnt_a_sse4_2(uint64_t* ret, const uint64_t value) {
  *ret = _mm_popcnt_u64(value);

}

#endif /*LV_HAVE_SSE4_2*/

#endif /*INCLUDED_volk_64u_popcnt_a_H*/

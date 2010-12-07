#ifndef INCLUDED_LIBVECTOR_COMMON_H
#define INCLUDED_LIBVECTOR_COMMON_H

#include<inttypes.h>
#if LV_HAVE_MMX
#include<xmmintrin.h>
union bit128{
  uint16_t i16[8];
  uint32_t i[4];
  float f[4];
  double d[2];
  __m128i int_vec;
  __m128 float_vec;
  __m128d double_vec;
};
#endif /*LV_HAVE_MMX*/

#endif /*INCLUDED_LIBVECTOR_COMMON_H*/

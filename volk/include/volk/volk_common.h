#ifndef INCLUDED_LIBVOLK_COMMON_H
#define INCLUDED_LIBVOLK_COMMON_H

#include<inttypes.h>
#ifdef LV_HAVE_MMX
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

struct volk_func_desc {
     const char **indices;
     const int *arch_defs;
     const int n_archs;
};

#endif /*INCLUDED_LIBVOLK_COMMON_H*/

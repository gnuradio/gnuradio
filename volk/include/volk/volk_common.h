#ifndef INCLUDED_LIBVOLK_COMMON_H
#define INCLUDED_LIBVOLK_COMMON_H

////////////////////////////////////////////////////////////////////////
// Cross-platform attribute macros
////////////////////////////////////////////////////////////////////////
#if defined __GNUC__
#  define __VOLK_ATTR_ALIGNED(x) __attribute__((aligned(x)))
#  define __VOLK_ATTR_UNUSED     __attribute__((unused))
#  define __VOLK_ATTR_INLINE     __attribute__((always_inline))
#  define __VOLK_ATTR_DEPRECATED __attribute__((deprecated))
#  if __GNUC__ >= 4
#    define __VOLK_ATTR_EXPORT   __attribute__((visibility("default")))
#    define __VOLK_ATTR_IMPORT   __attribute__((visibility("default")))
#  else
#    define __VOLK_ATTR_EXPORT
#    define __VOLK_ATTR_IMPORT
#  endif
#elif _MSC_VER
#  define __VOLK_ATTR_ALIGNED(x) __declspec(align(x))
#  define __VOLK_ATTR_UNUSED
#  define __VOLK_ATTR_INLINE     __forceinline
#  define __VOLK_ATTR_DEPRECATED __declspec(deprecated)
#  define __VOLK_ATTR_EXPORT     __declspec(dllexport)
#  define __VOLK_ATTR_IMPORT     __declspec(dllimport)
#else
#  define __VOLK_ATTR_ALIGNED(x)
#  define __VOLK_ATTR_UNUSED
#  define __VOLK_ATTR_INLINE
#  define __VOLK_ATTR_DEPRECATED
#  define __VOLK_ATTR_EXPORT
#  define __VOLK_ATTR_IMPORT
#endif

////////////////////////////////////////////////////////////////////////
// Ignore annoying warnings in MSVC
////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#  pragma warning(disable: 4244) //'conversion' conversion from 'type1' to 'type2', possible loss of data
#  pragma warning(disable: 4305) //'identifier' : truncation from 'type1' to 'type2'
#endif

////////////////////////////////////////////////////////////////////////
// C-linkage declaration macros
// FIXME: due to the usage of complex.h, require gcc for c-linkage
////////////////////////////////////////////////////////////////////////
#if defined(__cplusplus) && (__GNUC__)
#  define __VOLK_DECL_BEGIN extern "C" {
#  define __VOLK_DECL_END }
#else
#  define __VOLK_DECL_BEGIN
#  define __VOLK_DECL_END
#endif

////////////////////////////////////////////////////////////////////////
// Define VOLK_API for library symbols
// http://gcc.gnu.org/wiki/Visibility
////////////////////////////////////////////////////////////////////////
#ifdef volk_EXPORTS
#  define VOLK_API __VOLK_ATTR_EXPORT
#else
#  define VOLK_API __VOLK_ATTR_IMPORT
#endif

////////////////////////////////////////////////////////////////////////
// The bit128 union used by some
////////////////////////////////////////////////////////////////////////
#include <inttypes.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
#endif

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
#endif

union bit128{
  uint16_t i16[8];
  uint32_t i[4];
  float f[4];
  double d[2];

  #ifdef LV_HAVE_SSE
  __m128 float_vec;
  #endif

  #ifdef LV_HAVE_SSE2
  __m128i int_vec;
  __m128d double_vec;
  #endif
};

#define bit128_p(x) ((union bit128 *)(x))

#endif /*INCLUDED_LIBVOLK_COMMON_H*/

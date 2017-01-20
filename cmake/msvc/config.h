#ifndef _MSC_VER // [
#error "Use this header only with Microsoft Visual C++ compilers!"
#endif // _MSC_VER ]

#ifndef _MSC_CONFIG_H_ // [
#define _MSC_CONFIG_H_

////////////////////////////////////////////////////////////////////////
// enable inline functions for C code
////////////////////////////////////////////////////////////////////////
#ifndef __cplusplus
#  define inline __inline
#endif

////////////////////////////////////////////////////////////////////////
// signed size_t
////////////////////////////////////////////////////////////////////////
#include <stddef.h>
typedef ptrdiff_t ssize_t;

////////////////////////////////////////////////////////////////////////
// rint functions
////////////////////////////////////////////////////////////////////////
#if _MSC_VER < 1800
#include <math.h>
static inline long lrint(double x){return (long)(x > 0.0 ? x + 0.5 : x - 0.5);}
static inline long lrintf(float x){return (long)(x > 0.0f ? x + 0.5f : x - 0.5f);}
static inline long long llrint(double x){return (long long)(x > 0.0 ? x + 0.5 : x - 0.5);}
static inline long long llrintf(float x){return (long long)(x > 0.0f ? x + 0.5f : x - 0.5f);}
static inline double rint(double x){return (x > 0.0)? floor(x + 0.5) : ceil(x - 0.5);}
static inline float rintf(float x){return (x > 0.0f)? floorf(x + 0.5f) : ceilf(x - 0.5f);}
#endif

////////////////////////////////////////////////////////////////////////
// math constants
////////////////////////////////////////////////////////////////////////
#if _MSC_VER < 1800
#include <math.h>
#define INFINITY HUGE_VAL
#endif

////////////////////////////////////////////////////////////////////////
// random and srandom
////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
static inline long int random (void) { return rand(); }
static inline void srandom (unsigned int seed) { srand(seed); }

#endif // _MSC_CONFIG_H_ ]

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
#include <math.h>
static inline long lrint(double x){return (long)(x > 0.0 ? x + 0.5 : x - 0.5);}
static inline long lrintf(float x){return (long)(x > 0.0f ? x + 0.5f : x - 0.5f);}
static inline long long llrint(double x){return (long long)(x > 0.0 ? x + 0.5 : x - 0.5);}
static inline long long llrintf(float x){return (long long)(x > 0.0f ? x + 0.5f : x - 0.5f);}
static inline double rint(double x){return (x > 0.0)? floor(x + 0.5) : ceil(x - 0.5);}
static inline float rintf(float x){return (x > 0.0f)? floorf(x + 0.5f) : ceilf(x - 0.5f);}

////////////////////////////////////////////////////////////////////////
// math constants
////////////////////////////////////////////////////////////////////////
#define INFINITY HUGE_VAL

# define M_E		2.7182818284590452354	/* e */
# define M_LOG2E	1.4426950408889634074	/* log_2 e */
# define M_LOG10E	0.43429448190325182765	/* log_10 e */
# define M_LN2		0.69314718055994530942	/* log_e 2 */
# define M_LN10		2.30258509299404568402	/* log_e 10 */
# define M_PI		3.14159265358979323846	/* pi */
# define M_PI_2		1.57079632679489661923	/* pi/2 */
# define M_PI_4		0.78539816339744830962	/* pi/4 */
# define M_1_PI		0.31830988618379067154	/* 1/pi */
# define M_2_PI		0.63661977236758134308	/* 2/pi */
# define M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
# define M_SQRT2	1.41421356237309504880	/* sqrt(2) */
# define M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */

////////////////////////////////////////////////////////////////////////
// random and srandom
////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
static inline long int random (void) { return rand(); }
static inline void srandom (unsigned int seed) { srand(seed); }

#endif // _MSC_CONFIG_H_ ]

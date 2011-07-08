#ifndef INCLUDE_VOLK_COMPLEX_H
#define INCLUDE_VOLK_COMPLEX_H

/*!
 * \brief Provide typedefs and operators for all complex types in C and C++.
 *
 * The typedefs encompass all signed integer and floating point types.
 * Each operator function is intended to work across all data types.
 * Under C++, these operators are defined as inline templates.
 * Under C, these operators are defined as preprocessor macros.
 * The use of macros makes the operators agnostic to the type.
 *
 * The following operator functions are defined:
 * - lv_cmake - make a complex type from components
 * - lv_creal - get the real part of the complex number
 * - lv_cimag - get the imaginary part of the complex number
 * - lv_conj - take the conjugate of the complex number
 */

#ifdef __cplusplus

#include <complex>
#include <stdint.h>

typedef std::complex<int8_t>  lv_8sc_t;
typedef std::complex<int16_t> lv_16sc_t;
typedef std::complex<int32_t> lv_32sc_t;
typedef std::complex<int64_t> lv_64sc_t;
typedef std::complex<float>   lv_32fc_t;
typedef std::complex<double>  lv_64fc_t;

template <typename T> inline std::complex<T> lv_cmake(const T &r, const T &i){
    return std::complex<T>(r, i);
}

template <typename T> inline typename T::value_type lv_creal(const T &x){
    return x.real();
}

template <typename T> inline typename T::value_type lv_cimag(const T &x){
    return x.imag();
}

template <typename T> inline T lv_conj(const T &x){
    return std::conj(x);
}

#else /* __cplusplus */

#include <complex.h>

typedef char complex         lv_8sc_t;
typedef short complex        lv_16sc_t;
typedef long complex         lv_32sc_t;
typedef long long complex    lv_64sc_t;
typedef float complex        lv_32fc_t;
typedef double complex       lv_64fc_t;

#define lv_cmake(r, i) ((r) + _Complex_I*(i))

// When GNUC is available, use the complex extensions.
// The extensions always return the correct value type.
// http://gcc.gnu.org/onlinedocs/gcc/Complex.html
#ifdef __GNUC__

#define lv_creal(x) (__real__(x))

#define lv_cimag(x) (__imag__(x))

#define lv_conj(x) (~(x))

// When not available, use the c99 complex function family,
// which always returns double regardless of the input type.
#else /* __GNUC__ */

#define lv_creal(x) (creal(x))

#define lv_cimag(x) (cimag(x))

#define lv_conj(x) (conj(x))

#endif /* __GNUC__ */

#endif /* __cplusplus */

#endif /* INCLUDE_VOLK_COMPLEX_H */

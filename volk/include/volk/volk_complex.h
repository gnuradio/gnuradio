#ifndef INCLUDE_VOLK_COMPLEX_H
#define INCLUDE_VOLK_COMPLEX_H

/*!
  \brief This header file is to prevent issues with having <complex> and <complex.h> variables in the same code as the gcc compiler does not allow that
*/
#ifdef __cplusplus

#include <complex>
#include <stdint.h>

typedef std::complex<int8_t>  lv_8sc_t;
typedef std::complex<int16_t> lv_16sc_t;
typedef std::complex<int32_t> lv_32sc_t;
typedef std::complex<float>   lv_32fc_t;
typedef std::complex<double>  lv_64fc_t;

static inline float lv_creal(const lv_32fc_t x){
  return x.real();
}

static inline float lv_cimag(const lv_32fc_t x){
  return x.imag();
}

static inline lv_32fc_t lv_conj(const lv_32fc_t x){
  return std::conj(x);
}

static inline lv_32fc_t lv_cpow(const lv_32fc_t x, const lv_32fc_t y){
  return std::pow(x, y);
}

static inline lv_32fc_t lv_32fc_init(const float x, const float y){
  return std::complex<float>(x,y);
}

#else

#include <complex.h>

typedef char complex         lv_8sc_t;
typedef short complex        lv_16sc_t;
typedef int complex          lv_32sc_t;
typedef float complex        lv_32fc_t;
typedef double complex       lv_64fc_t;

static inline float lv_creal(const lv_32fc_t x){
  return creal(x);
}

static inline float lv_cimag(const lv_32fc_t x){
  return cimag(x);
}

static inline lv_32fc_t lv_conj(const lv_32fc_t x){
  return conj(x);
}

static inline lv_32fc_t lv_cpow(const lv_32fc_t x, const lv_32fc_t y){
  return cpow(x, y);
}

static inline lv_32fc_t lv_32fc_init(const float x, const float y){
  return x + I*y;
}

#endif


#endif /* INCLUDE_VOLK_COMPLEX_H */

#include <gr_core_api.h>
#include <vector>
#include <complex>

GR_CORE_API std::vector<std::complex<float> >
complex_vec_test0();

GR_CORE_API std::vector<std::complex<float> >
complex_vec_test1(const std::vector<std::complex<float> > &input);

GR_CORE_API std::complex<float>
complex_scalar_test0();

GR_CORE_API std::complex<float>
complex_scalar_test1(std::complex<float> input);

GR_CORE_API std::vector<int>
int_vec_test0();

GR_CORE_API std::vector<int>
int_vec_test1(const std::vector<int> &input);

GR_CORE_API std::vector<float>
float_vec_test0();

GR_CORE_API std::vector<float>
float_vec_test1(const std::vector<float> &input);


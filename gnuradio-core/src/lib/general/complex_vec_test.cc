#include <complex_vec_test.h>
#include <stddef.h>

std::vector<std::complex<float> >
complex_vec_test0()
{
  std::vector<std::complex<float> > r(5);
  
  for (size_t i = 0; i < r.size(); i++)
    r[i] = std::complex<float>(i, i);

  return r;
}

std::vector<std::complex<float> > 
complex_vec_test1(const std::vector<std::complex<float> > &input)
{
  std::vector<std::complex<float> > r(input.size());
  
  for (size_t i = 0; i < input.size(); i++)
    r[i] = std::complex<float>(input[i].real()+0.5, input[i].imag()-0.5);

  return r;
}

std::complex<float>
complex_scalar_test0()
{
  return std::complex<float>(5, 5);
}

std::complex<float>
complex_scalar_test1(std::complex<float> input)
{
  return std::complex<float>(input.real()+0.5, input.imag()-0.5);
}


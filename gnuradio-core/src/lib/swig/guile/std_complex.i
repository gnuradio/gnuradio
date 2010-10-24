%{
#include <complex> 
%}

// To the target language, complex number conversion
%typemap(out) complex, complex<double>, std::complex<double> {
  $result = scm_make_rectangular( gh_double2scm ($1.real ()),
           gh_double2scm ($1.imag ()) );
}

// To the target language, complex number conversion
%typemap(out) complex, complex<float>, std::complex<float> {
  $result = scm_make_rectangular( gh_double2scm ($1.real ()),
           gh_double2scm ($1.imag ()) );
}

// From the target language, complex number conversion
%typemap(in) complex, complex<double>, std::complex<double> {
  $1 = std::complex<double>( gh_scm2double (scm_real_part ($input)),
           gh_scm2double (scm_imag_part ($input)) );
}

// From the target language, complex number conversion
%typemap(in) complex, complex<float>, std::complex<float> {
  $1 = std::complex<float>( gh_scm2double (scm_real_part ($input)),
           gh_scm2double (scm_imag_part ($input)) );
}

%typemaps_primitive(%checkcode(CPLXDBL), std::complex<double>);
%typemaps_primitive(%checkcode(CPLXFLT), std::complex<float>);

%typecheck(SWIG_TYPECHECK_COMPLEX)
   std::complex<float>, std::complex<double>,
   const std::complex<float> &, const std::complex<double> &
{
  $1 = scm_is_complex($input) ? 1 : 0;
}

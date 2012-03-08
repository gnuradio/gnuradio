/* -*- c++ -*- */

%include "gnuradio.i"			// the common stuff

%{
#include "fcd_source_c.h"
%}

%include "fcd_source_c.i"

#if SWIGGUILE
%scheme %{
(load-extension-global "libguile-gnuradio-fcd_swig" "scm_init_gnuradio_fcd_swig_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif

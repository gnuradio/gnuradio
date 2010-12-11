/* -*- c++ -*- */

%include "gnuradio.i"			// the common stuff

%{
#include "howto_square_ff.h"
#include "howto_square2_ff.h"
%}

%include "howto_square_ff.i"
%include "howto_square2_ff.i"

#if SWIGGUILE
%scheme %{
(load-extension-global "libguile-gnuradio-howto_swig" "scm_init_gnuradio_howto_swig_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif

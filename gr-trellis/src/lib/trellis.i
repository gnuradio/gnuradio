/* -*- c++ -*- */

%feature("autodoc", "1");		// generate python docstrings

%include "exception.i"
%import "gnuradio.i"			// the common stuff

%{
#include "gnuradio_swig_bug_workaround.h"	// mandatory bug fix
#include "fsm.h"
#include "interleaver.h"
#include "trellis_permutation.h"
#include <stdexcept>
%}

// ----------------------------------------------------------------

%include "fsm.i"
%include "interleaver.i"
%include "trellis_permutation.i"
%include "trellis_metric_type.h"


%include "trellis_generated.i"


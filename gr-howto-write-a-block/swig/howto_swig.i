/* -*- c++ -*- */

#define HOWTO_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "howto_swig_doc.i"


%{
#include "howto_square_ff.h"
#include "howto_square2_ff.h"
%}

GR_SWIG_BLOCK_MAGIC(howto,square_ff);
%include "howto_square_ff.h"

GR_SWIG_BLOCK_MAGIC(howto,square2_ff);
%include "howto_square2_ff.h"

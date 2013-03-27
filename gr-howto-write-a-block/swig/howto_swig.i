/* -*- c++ -*- */

#define HOWTO_API

%include "runtime_swig.i"			// the common stuff

//load generated python docstrings
%include "howto_swig_doc.i"

%{
#include "howto/square_ff.h"
#include "howto/square2_ff.h"
%}

%include "howto/square_ff.h"
%include "howto/square2_ff.h"

GR_SWIG_BLOCK_MAGIC2(howto, square_ff);
GR_SWIG_BLOCK_MAGIC2(howto, square2_ff);

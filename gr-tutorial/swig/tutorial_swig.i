/* -*- c++ -*- */

#define TUTORIAL_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "tutorial_swig_doc.i"

%{
#include "tutorial/my_qpsk_demod_cb.h"
%}


%include "tutorial/my_qpsk_demod_cb.h"
GR_SWIG_BLOCK_MAGIC2(tutorial, my_qpsk_demod_cb);

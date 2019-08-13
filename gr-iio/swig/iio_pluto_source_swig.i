/* -*- c++ -*- */

/* The PlutoSDR block is in a separate file, because Swig chokes on it
 * and produces incorrect Python code if it is present in iio_swig.i... */

#define IIO_API

%include "gnuradio.i"

//load generated python docstrings
%include "iio_swig_doc.i"

%{
#include "gnuradio/iio/pluto_source.h"
%}

%include "gnuradio/iio/pluto_source.h"

GR_SWIG_BLOCK_MAGIC2(iio, pluto_source);

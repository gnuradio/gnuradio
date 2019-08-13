/* -*- c++ -*- */

#define IIO_API

%include "gnuradio.i"

//load generated python docstrings
%include "iio_swig_doc.i"

%{
#include "gnuradio/iio/device_source.h"
#include "gnuradio/iio/device_sink.h"
#include "gnuradio/iio/fmcomms2_source.h"
#include "gnuradio/iio/fmcomms2_sink.h"
#include "gnuradio/iio/fmcomms5_source.h"
#include "gnuradio/iio/fmcomms5_sink.h"
#include "gnuradio/iio/attr_source.h"
#include "gnuradio/iio/attr_sink.h"
#include "gnuradio/iio/dds_control.h"
#include "gnuradio/iio/attr_updater.h"
%}

%include "gnuradio/iio/device_source.h"
%include "gnuradio/iio/device_sink.h"
%include "gnuradio/iio/fmcomms2_source.h"
%include "gnuradio/iio/fmcomms2_sink.h"
%include "gnuradio/iio/fmcomms5_source.h"
%include "gnuradio/iio/fmcomms5_sink.h"
%include "gnuradio/iio/attr_source.h"
%include "gnuradio/iio/attr_sink.h"
%include "gnuradio/iio/dds_control.h"
%include "gnuradio/iio/attr_updater.h"

GR_SWIG_BLOCK_MAGIC2(iio, device_source);
GR_SWIG_BLOCK_MAGIC2(iio, device_sink);
GR_SWIG_BLOCK_MAGIC2(iio, fmcomms2_source);
GR_SWIG_BLOCK_MAGIC2(iio, fmcomms2_source_f32c);
GR_SWIG_BLOCK_MAGIC2(iio, fmcomms2_sink);
GR_SWIG_BLOCK_MAGIC2(iio, fmcomms2_sink_f32c);
GR_SWIG_BLOCK_MAGIC2(iio, fmcomms5_source);
GR_SWIG_BLOCK_MAGIC2(iio, fmcomms5_source_f32c);
GR_SWIG_BLOCK_MAGIC2(iio, fmcomms5_sink);
GR_SWIG_BLOCK_MAGIC2(iio, fmcomms5_sink_f32c);
GR_SWIG_BLOCK_MAGIC2(iio, attr_source);
GR_SWIG_BLOCK_MAGIC2(iio, attr_sink);
GR_SWIG_BLOCK_MAGIC2(iio, dds_control);
GR_SWIG_BLOCK_MAGIC2(iio, attr_updater);

/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#define QTGUI_API
#define FILTER_API

%include "gnuradio.i"

//load generated python docstrings
%include "qtgui_swig_doc.i"

namespace gr {
  namespace qtgui {

    enum graph_t {
      NUM_GRAPH_NONE = 0,
      NUM_GRAPH_HORIZ,
      NUM_GRAPH_VERT,
    };

    enum data_type_t {
      INT = 0,
      FLOAT,
      DOUBLE,
      COMPLEX,
      STRING,
      INT_VEC,
      FLOAT_VEC,
      DOUBLE_VEC,
      COMPLEX_VEC,
    };

  } /* namespace qtgui */
} /* namespace gr */


enum{
  INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR = 0,
  INTENSITY_COLOR_MAP_TYPE_WHITE_HOT = 1,
  INTENSITY_COLOR_MAP_TYPE_BLACK_HOT = 2,
  INTENSITY_COLOR_MAP_TYPE_INCANDESCENT = 3,
  INTENSITY_COLOR_MAP_TYPE_USER_DEFINED = 4,
  INTENSITY_COLOR_MAP_TYPE_SUNSET = 5,
  INTENSITY_COLOR_MAP_TYPE_COOL = 6,
};

%include "gnuradio/qtgui/trigger_mode.h"

// So we understand the firdes window types
%import "gnuradio/filter/firdes.h"

%{
#include "gnuradio/qtgui/form_menus.h"
#include "gnuradio/qtgui/DisplayPlot.h"
#include "gnuradio/qtgui/displayform.h"
#include "gnuradio/qtgui/eyedisplaysform.h"
#include "gnuradio/qtgui/sink_c.h"
#include "gnuradio/qtgui/sink_f.h"
#include "gnuradio/qtgui/eye_sink_c.h"
#include "gnuradio/qtgui/eye_sink_f.h"
#include "gnuradio/qtgui/time_sink_c.h"
#include "gnuradio/qtgui/time_sink_f.h"
#include "gnuradio/qtgui/time_raster_sink_b.h"
#include "gnuradio/qtgui/time_raster_sink_f.h"
#include "gnuradio/qtgui/freq_sink_c.h"
#include "gnuradio/qtgui/freq_sink_f.h"
#include "gnuradio/qtgui/const_sink_c.h"
#include "gnuradio/qtgui/waterfall_sink_c.h"
#include "gnuradio/qtgui/waterfall_sink_f.h"
#include "gnuradio/qtgui/histogram_sink_f.h"
#include "gnuradio/qtgui/number_sink.h"
#include "gnuradio/qtgui/ber_sink_b.h"
#include "gnuradio/qtgui/vector_sink_f.h"
#include "gnuradio/qtgui/edit_box_msg.h"
%}

%include "gnuradio/qtgui/sink_c.h"
%include "gnuradio/qtgui/sink_f.h"
%include "gnuradio/qtgui/eye_sink_c.h"
%include "gnuradio/qtgui/eye_sink_f.h"
%include "gnuradio/qtgui/time_sink_c.h"
%include "gnuradio/qtgui/time_sink_f.h"
%include "gnuradio/qtgui/time_raster_sink_b.h"
%include "gnuradio/qtgui/time_raster_sink_f.h"
%include "gnuradio/qtgui/freq_sink_c.h"
%include "gnuradio/qtgui/freq_sink_f.h"
%include "gnuradio/qtgui/const_sink_c.h"
%include "gnuradio/qtgui/waterfall_sink_c.h"
%include "gnuradio/qtgui/waterfall_sink_f.h"
%include "gnuradio/qtgui/histogram_sink_f.h"
%include "gnuradio/qtgui/number_sink.h"
%include "gnuradio/qtgui/ber_sink_b.h"
%include "gnuradio/qtgui/vector_sink_f.h"
%include "gnuradio/qtgui/edit_box_msg.h"

GR_SWIG_BLOCK_MAGIC2(qtgui, sink_c);
GR_SWIG_BLOCK_MAGIC2(qtgui, sink_f);
GR_SWIG_BLOCK_MAGIC2(qtgui, eye_sink_c);
GR_SWIG_BLOCK_MAGIC2(qtgui, eye_sink_f);
GR_SWIG_BLOCK_MAGIC2(qtgui, time_sink_c);
GR_SWIG_BLOCK_MAGIC2(qtgui, time_sink_f);
GR_SWIG_BLOCK_MAGIC2(qtgui, time_raster_sink_b);
GR_SWIG_BLOCK_MAGIC2(qtgui, time_raster_sink_f);
GR_SWIG_BLOCK_MAGIC2(qtgui, freq_sink_c);
GR_SWIG_BLOCK_MAGIC2(qtgui, freq_sink_f);
GR_SWIG_BLOCK_MAGIC2(qtgui, const_sink_c);
GR_SWIG_BLOCK_MAGIC2(qtgui, waterfall_sink_c);
GR_SWIG_BLOCK_MAGIC2(qtgui, waterfall_sink_f);
GR_SWIG_BLOCK_MAGIC2(qtgui, histogram_sink_f);
GR_SWIG_BLOCK_MAGIC2(qtgui, number_sink);
GR_SWIG_BLOCK_MAGIC2(qtgui, ber_sink_b);
GR_SWIG_BLOCK_MAGIC2(qtgui, vector_sink_f);
GR_SWIG_BLOCK_MAGIC2(qtgui, edit_box_msg);

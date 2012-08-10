/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#define QTGUI_API

%include "gnuradio.i"

//load generated python docstrings
%include "qtgui_swig_doc.i"

%{
#include "qtgui/sink_c.h"
#include "qtgui/sink_f.h"
#include "qtgui/time_sink_c.h"
#include "qtgui/time_sink_f.h"
#include "qtgui/freq_sink_c.h"
#include "qtgui/freq_sink_f.h"
#include "qtgui/const_sink_c.h"
#include "qtgui/waterfall_sink_c.h"
#include "qtgui/waterfall_sink_f.h"
%}

%include "qtgui/sink_c.h"
%include "qtgui/sink_f.h"
%include "qtgui/time_sink_c.h"
%include "qtgui/time_sink_f.h"
%include "qtgui/freq_sink_c.h"
%include "qtgui/freq_sink_f.h"
%include "qtgui/const_sink_c.h"
%include "qtgui/waterfall_sink_c.h"
%include "qtgui/waterfall_sink_f.h"

GR_SWIG_BLOCK_MAGIC2(qtgui, sink_c);
GR_SWIG_BLOCK_MAGIC2(qtgui, sink_f);
GR_SWIG_BLOCK_MAGIC2(qtgui, time_sink_c);
GR_SWIG_BLOCK_MAGIC2(qtgui, time_sink_f);
GR_SWIG_BLOCK_MAGIC2(qtgui, freq_sink_c);
GR_SWIG_BLOCK_MAGIC2(qtgui, freq_sink_f);
GR_SWIG_BLOCK_MAGIC2(qtgui, const_sink_c);
GR_SWIG_BLOCK_MAGIC2(qtgui, waterfall_sink_c);
GR_SWIG_BLOCK_MAGIC2(qtgui, waterfall_sink_f);

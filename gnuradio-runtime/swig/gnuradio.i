/* -*- c++ -*- */
/*
 * Copyright 2003,2004,2009,2012,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

// Disable warning about base class types
#pragma SWIG nowarn=401

////////////////////////////////////////////////////////////////////////
// gnuradio.i
// SWIG interface definition
////////////////////////////////////////////////////////////////////////

%include "std_string.i"
%include <gr_extras.i>

#ifdef SW_RUNTIME
%include <gr_types.i>
#endif

////////////////////////////////////////////////////////////////////////
// Headers

%feature("autodoc","1");

// Required for a bug in SWIG 2.0.4 with GCC 4.6
// see: http://sourceforge.net/p/swig/bugs/1187/
%{
#include <cstddef>
%}

// local file
%include <gr_shared_ptr.i>
%include <std_except.i>

////////////////////////////////////////////////////////////////////////

#ifndef SW_RUNTIME
// import runtime_swig.i for all but sw_runtime, since it needs to %include
%import <runtime_swig.i>
%import <gr_types.i>
#endif

////////////////////////////////////////////////////////////////////////

%{
#include <gnuradio/block.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/sync_decimator.h>
#include <gnuradio/sync_interpolator.h>
#include <gnuradio/tagged_stream_block.h>
#include <gnuradio/block_gateway.h>
#include <gnuradio/feval.h>
#include <gnuradio/py_feval.h>
#include <gnuradio/high_res_timer.h>
%}

%include <gnuradio/high_res_timer.h>

////////////////////////////////////////////////////////////////////////
// Python 2/3 compatibility

%begin %{
#define SWIG_PYTHON_CAST_MODE
%}

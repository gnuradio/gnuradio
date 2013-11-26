/* -*- c++ -*- */
/*
 * Copyright 2003,2004,2009,2012 Free Software Foundation, Inc.
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

%{
#include "gnuradio_swig_bug_workaround.h"	// mandatory bug fix
%}

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

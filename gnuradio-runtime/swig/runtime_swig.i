/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#define GR_RUNTIME_API

#ifndef SWIGIMPORTED
%include "runtime_swig_doc.i"
%module(directors="1") gnuradio_runtime
#endif

%feature("autodoc", "1");		// generate python docstrings

#define SW_RUNTIME
%include "gnuradio.i"				// the common stuff

%{
#include <gnuradio/runtime_types.h>
%}

%include <gnuradio/runtime_types.h>

%{
#include <gnuradio/block.h>
#include <gnuradio/block_detail.h>
#include <gnuradio/buffer.h>
#include <gnuradio/constants.h>
#include <gnuradio/endianness.h>
#include <gnuradio/feval.h>
#include <gnuradio/hier_block2.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/message.h>
#include <gnuradio/msg_handler.h>
#include <gnuradio/msg_queue.h>
#include <gnuradio/prefs.h>
#include <gnuradio/realtime.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/sync_decimator.h>
#include <gnuradio/sync_interpolator.h>
#include <gnuradio/tags.h>
#include <gnuradio/tagged_stream_block.h>
#include <gnuradio/top_block.h>
#include <gnuradio/logger.h>
#include <gnuradio/math.h>
#include <gnuradio/random.h>
%}

%constant int sizeof_char 	= sizeof(char);
%constant int sizeof_short	= sizeof(short);
%constant int sizeof_int	= sizeof(int);
%constant int sizeof_float	= sizeof(float);
%constant int sizeof_double	= sizeof(double);
%constant int sizeof_gr_complex	= sizeof(gr_complex);

%include <gnuradio/endianness.h>
%include "basic_block.i"
%include "block.i"
%include "block_detail.i"
%include "buffer.i"
%include "constants.i"
%include "feval.i"
%include "hier_block2.i"
%include "io_signature.i"
%include "message.i"
%include "msg_handler.i"
%include "msg_queue.i"
%include "prefs.i"
%include "realtime.i"
%include "sync_block.i"
%include "sync_decimator.i"
%include "sync_interpolator.i"
%include "tagged_stream_block.i"
%include "tags.i"
%include "top_block.i"
%include "block_gateway.i"
%include "gr_logger.i"
%include "gr_swig_block_magic.i"
%include "gr_ctrlport.i"

%include "gnuradio/math.h"
%include "gnuradio/random.h"

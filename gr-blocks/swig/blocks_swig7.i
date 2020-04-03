/* -*- c++ -*- */
/*
 * Copyright 2017 Free Software Foundation, Inc.
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

#define BLOCKS_API
#define GR_RUNTIME_API

%include "gnuradio.i"

//load generated python docstrings
%include "blocks_swig7_doc.i"

%include "feval.i"

%{
#include "gnuradio/blocks/deinterleave.h"
#include "gnuradio/blocks/divide.h"
#include "gnuradio/blocks/exponentiate_const_cci.h"
#include "gnuradio/blocks/selector.h"
#include "gnuradio/blocks/skiphead.h"
#include "gnuradio/blocks/stream_mux.h"
#include "gnuradio/blocks/stream_to_streams.h"
#include "gnuradio/blocks/stream_to_tagged_stream.h"
#include "gnuradio/blocks/stream_to_vector.h"
#include "gnuradio/blocks/streams_to_stream.h"
#include "gnuradio/blocks/streams_to_vector.h"
#include "gnuradio/blocks/tag_debug.h"
#include "gnuradio/blocks/tagged_file_sink.h"
#include "gnuradio/blocks/tsb_vector_sink.h"
#include "gnuradio/blocks/throttle.h"
%}

%include "gnuradio/blocks/deinterleave.h"
%include "gnuradio/blocks/divide.h"
%include "gnuradio/blocks/exponentiate_const_cci.h"
%include "gnuradio/blocks/selector.h"
%include "gnuradio/blocks/skiphead.h"
%include "gnuradio/blocks/stream_mux.h"
%include "gnuradio/blocks/stream_to_streams.h"
%include "gnuradio/blocks/stream_to_tagged_stream.h"
%include "gnuradio/blocks/stream_to_vector.h"
%include "gnuradio/blocks/streams_to_stream.h"
%include "gnuradio/blocks/streams_to_vector.h"
%include "gnuradio/blocks/tag_debug.h"
%include "gnuradio/blocks/tagged_file_sink.h"
%include "gnuradio/blocks/tsb_vector_sink.h"
%include "gnuradio/blocks/throttle.h"

GR_SWIG_BLOCK_MAGIC2(blocks, deinterleave);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, divide_ff, divide<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, divide_ss, divide<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, divide_ii, divide<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, divide_cc, divide<gr_complex>);
GR_SWIG_BLOCK_MAGIC2(blocks, exponentiate_const_cci);
GR_SWIG_BLOCK_MAGIC2(blocks, selector);
GR_SWIG_BLOCK_MAGIC2(blocks, skiphead);
GR_SWIG_BLOCK_MAGIC2(blocks, stream_mux);
GR_SWIG_BLOCK_MAGIC2(blocks, stream_to_streams);
GR_SWIG_BLOCK_MAGIC2(blocks, stream_to_tagged_stream);
GR_SWIG_BLOCK_MAGIC2(blocks, stream_to_vector);
GR_SWIG_BLOCK_MAGIC2(blocks, streams_to_stream);
GR_SWIG_BLOCK_MAGIC2(blocks, streams_to_vector);
GR_SWIG_BLOCK_MAGIC2(blocks, tag_debug);
GR_SWIG_BLOCK_MAGIC2(blocks, tagged_file_sink);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, tsb_vector_sink_b, tsb_vector_sink<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, tsb_vector_sink_c, tsb_vector_sink<gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, tsb_vector_sink_f, tsb_vector_sink<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, tsb_vector_sink_i, tsb_vector_sink<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, tsb_vector_sink_s, tsb_vector_sink<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2(blocks, throttle);

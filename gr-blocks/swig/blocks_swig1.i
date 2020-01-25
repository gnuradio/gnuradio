/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#define BLOCKS_API

%include "gnuradio.i"

//load generated python docstrings
%include "blocks_swig1_doc.i"

%{
#include "gnuradio/blocks/vector_map.h"
#include "gnuradio/blocks/vector_to_stream.h"
#include "gnuradio/blocks/vector_to_streams.h"
#include "gnuradio/blocks/vector_insert.h"
#include "gnuradio/blocks/vector_sink.h"
#include "gnuradio/blocks/vector_source.h"
#include "gnuradio/blocks/wavfile_sink.h"
#include "gnuradio/blocks/wavfile_source.h"
%}

%include "gnuradio/blocks/vector_map.h"
%include "gnuradio/blocks/vector_to_stream.h"
%include "gnuradio/blocks/vector_to_streams.h"
%include "gnuradio/blocks/vector_insert.h"
%include "gnuradio/blocks/vector_sink.h"
%include "gnuradio/blocks/vector_source.h"
%include "gnuradio/blocks/wavfile_sink.h"
%include "gnuradio/blocks/wavfile_source.h"

GR_SWIG_BLOCK_MAGIC2(blocks, vector_map);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_to_stream);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_to_streams);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, vector_insert_b, vector_insert<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, vector_insert_s, vector_insert<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, vector_insert_i, vector_insert<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, vector_insert_f, vector_insert<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, vector_insert_c, vector_insert<gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, vector_sink_b, vector_sink<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, vector_sink_s, vector_sink<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, vector_sink_i, vector_sink<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, vector_sink_f, vector_sink<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, vector_sink_c, vector_sink<gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, vector_source_b, vector_source<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, vector_source_s, vector_source<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, vector_source_i, vector_source<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, vector_source_f, vector_source<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, vector_source_c, vector_source<gr_complex>);
GR_SWIG_BLOCK_MAGIC2(blocks, wavfile_sink);
GR_SWIG_BLOCK_MAGIC2(blocks, wavfile_source);

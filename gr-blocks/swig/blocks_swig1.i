/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
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

%include "gnuradio.i"

//load generated python docstrings
%include "blocks_swig1_doc.i"

%{
#include "gnuradio/blocks/vector_map.h"
#include "gnuradio/blocks/vector_to_stream.h"
#include "gnuradio/blocks/vector_to_streams.h"
#include "gnuradio/blocks/vector_insert_b.h"
#include "gnuradio/blocks/vector_insert_s.h"
#include "gnuradio/blocks/vector_insert_i.h"
#include "gnuradio/blocks/vector_insert_f.h"
#include "gnuradio/blocks/vector_insert_c.h"
#include "gnuradio/blocks/vector_sink_b.h"
#include "gnuradio/blocks/vector_sink_s.h"
#include "gnuradio/blocks/vector_sink_i.h"
#include "gnuradio/blocks/vector_sink_f.h"
#include "gnuradio/blocks/vector_sink_c.h"
#include "gnuradio/blocks/vector_source_b.h"
#include "gnuradio/blocks/vector_source_s.h"
#include "gnuradio/blocks/vector_source_i.h"
#include "gnuradio/blocks/vector_source_f.h"
#include "gnuradio/blocks/vector_source_c.h"
#include "gnuradio/blocks/wavfile_sink.h"
#include "gnuradio/blocks/wavfile_source.h"
%}

%include "gnuradio/blocks/vector_map.h"
%include "gnuradio/blocks/vector_to_stream.h"
%include "gnuradio/blocks/vector_to_streams.h"
%include "gnuradio/blocks/vector_insert_b.h"
%include "gnuradio/blocks/vector_insert_s.h"
%include "gnuradio/blocks/vector_insert_i.h"
%include "gnuradio/blocks/vector_insert_f.h"
%include "gnuradio/blocks/vector_insert_c.h"
%include "gnuradio/blocks/vector_sink_b.h"
%include "gnuradio/blocks/vector_sink_s.h"
%include "gnuradio/blocks/vector_sink_i.h"
%include "gnuradio/blocks/vector_sink_f.h"
%include "gnuradio/blocks/vector_sink_c.h"
%include "gnuradio/blocks/vector_source_b.h"
%include "gnuradio/blocks/vector_source_s.h"
%include "gnuradio/blocks/vector_source_i.h"
%include "gnuradio/blocks/vector_source_f.h"
%include "gnuradio/blocks/vector_source_c.h"
%include "gnuradio/blocks/wavfile_sink.h"
%include "gnuradio/blocks/wavfile_source.h"

GR_SWIG_BLOCK_MAGIC2(blocks, vector_map);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_to_stream);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_to_streams);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_insert_b);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_insert_s);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_insert_i);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_insert_f);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_insert_c);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_sink_b);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_sink_s);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_sink_i);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_sink_f);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_sink_c);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_source_b);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_source_s);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_source_i);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_source_f);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_source_c);
GR_SWIG_BLOCK_MAGIC2(blocks, wavfile_sink);
GR_SWIG_BLOCK_MAGIC2(blocks, wavfile_source);

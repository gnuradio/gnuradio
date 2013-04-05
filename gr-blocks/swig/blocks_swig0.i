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

%include "runtime_swig.i"

%include "blocks_swig0_doc.i"

%template() std::vector<size_t>;
%template() std::vector< std::vector< std::vector<size_t> > >;

%{
#include "blocks/annotator_1to1.h"
#include "blocks/annotator_alltoall.h"
#include "blocks/annotator_raw.h"
#include "blocks/control_loop.h"
#include "blocks/copy.h"
#include "blocks/delay.h"
#include "blocks/endian_swap.h"
#include "blocks/file_descriptor_sink.h"
#include "blocks/file_descriptor_source.h"
#include "blocks/file_sink_base.h"
#include "blocks/file_sink.h"
#include "blocks/file_source.h"
#include "blocks/file_meta_sink.h"
#include "blocks/file_meta_source.h"
#include "blocks/head.h"
#include "blocks/message_debug.h"
#include "blocks/message_sink.h"
#include "blocks/message_source.h"
#include "blocks/message_strobe.h"
#include "blocks/message_burst_source.h"
#include "blocks/nop.h"
#include "blocks/null_sink.h"
#include "blocks/null_source.h"
#include "blocks/skiphead.h"
#include "blocks/stream_mux.h"
#include "blocks/stream_to_streams.h"
#include "blocks/stream_to_vector.h"
#include "blocks/streams_to_stream.h"
#include "blocks/streams_to_vector.h"
#include "blocks/tag_debug.h"
#include "blocks/tagged_file_sink.h"
#include "blocks/throttle.h"
#include "blocks/vector_map.h"
#include "blocks/vector_to_stream.h"
#include "blocks/vector_to_streams.h"
#include "blocks/vector_insert_b.h"
#include "blocks/vector_insert_s.h"
#include "blocks/vector_insert_i.h"
#include "blocks/vector_insert_f.h"
#include "blocks/vector_insert_c.h"
#include "blocks/vector_sink_b.h"
#include "blocks/vector_sink_s.h"
#include "blocks/vector_sink_i.h"
#include "blocks/vector_sink_f.h"
#include "blocks/vector_sink_c.h"
#include "blocks/vector_source_b.h"
#include "blocks/vector_source_s.h"
#include "blocks/vector_source_i.h"
#include "blocks/vector_source_f.h"
#include "blocks/vector_source_c.h"
#include "blocks/wavfile_sink.h"
#include "blocks/wavfile_source.h"
%}

%include "blocks/annotator_1to1.h"
%include "blocks/annotator_alltoall.h"
%include "blocks/annotator_raw.h"
%include "blocks/control_loop.h"
%include "blocks/copy.h"
%include "blocks/delay.h"
%include "blocks/endian_swap.h"
%include "blocks/file_descriptor_sink.h"
%include "blocks/file_descriptor_source.h"
%include "blocks/file_sink_base.h"
%include "blocks/file_sink.h"
%include "blocks/file_source.h"
%include "blocks/file_meta_sink.h"
%include "blocks/file_meta_source.h"
%include "blocks/head.h"
%include "blocks/message_debug.h"
%include "blocks/message_sink.h"
%include "blocks/message_source.h"
%include "blocks/message_strobe.h"
%include "blocks/message_burst_source.h"
%include "blocks/nop.h"
%include "blocks/null_sink.h"
%include "blocks/null_source.h"
%include "blocks/skiphead.h"
%include "blocks/stream_mux.h"
%include "blocks/stream_to_streams.h"
%include "blocks/stream_to_vector.h"
%include "blocks/streams_to_stream.h"
%include "blocks/streams_to_vector.h"
%include "blocks/tag_debug.h"
%include "blocks/tagged_file_sink.h"
%include "blocks/throttle.h"
%include "blocks/vector_map.h"
%include "blocks/vector_to_stream.h"
%include "blocks/vector_to_streams.h"
%include "blocks/vector_insert_b.h"
%include "blocks/vector_insert_s.h"
%include "blocks/vector_insert_i.h"
%include "blocks/vector_insert_f.h"
%include "blocks/vector_insert_c.h"
%include "blocks/vector_sink_b.h"
%include "blocks/vector_sink_s.h"
%include "blocks/vector_sink_i.h"
%include "blocks/vector_sink_f.h"
%include "blocks/vector_sink_c.h"
%include "blocks/vector_source_b.h"
%include "blocks/vector_source_s.h"
%include "blocks/vector_source_i.h"
%include "blocks/vector_source_f.h"
%include "blocks/vector_source_c.h"
%include "blocks/wavfile_sink.h"
%include "blocks/wavfile_source.h"

GR_SWIG_BLOCK_MAGIC2(blocks, annotator_1to1);
GR_SWIG_BLOCK_MAGIC2(blocks, annotator_alltoall);
GR_SWIG_BLOCK_MAGIC2(blocks, annotator_raw);
GR_SWIG_BLOCK_MAGIC2(blocks, copy);
GR_SWIG_BLOCK_MAGIC2(blocks, delay);
GR_SWIG_BLOCK_MAGIC2(blocks, endian_swap);
GR_SWIG_BLOCK_MAGIC2(blocks, file_descriptor_sink);
GR_SWIG_BLOCK_MAGIC2(blocks, file_descriptor_source);
GR_SWIG_BLOCK_MAGIC2(blocks, file_sink);
GR_SWIG_BLOCK_MAGIC2(blocks, file_source);
GR_SWIG_BLOCK_MAGIC2(blocks, file_meta_sink);
GR_SWIG_BLOCK_MAGIC2(blocks, file_meta_source);
GR_SWIG_BLOCK_MAGIC2(blocks, head);
GR_SWIG_BLOCK_MAGIC2(blocks, message_debug);
GR_SWIG_BLOCK_MAGIC2(blocks, message_sink);
GR_SWIG_BLOCK_MAGIC2(blocks, message_source);
GR_SWIG_BLOCK_MAGIC2(blocks, message_strobe);
GR_SWIG_BLOCK_MAGIC2(blocks, message_burst_source);
GR_SWIG_BLOCK_MAGIC2(blocks, nop);
GR_SWIG_BLOCK_MAGIC2(blocks, null_sink);
GR_SWIG_BLOCK_MAGIC2(blocks, null_source);
GR_SWIG_BLOCK_MAGIC2(blocks, skiphead);
GR_SWIG_BLOCK_MAGIC2(blocks, stream_mux);
GR_SWIG_BLOCK_MAGIC2(blocks, stream_to_streams);
GR_SWIG_BLOCK_MAGIC2(blocks, stream_to_vector);
GR_SWIG_BLOCK_MAGIC2(blocks, streams_to_stream);
GR_SWIG_BLOCK_MAGIC2(blocks, streams_to_vector);
GR_SWIG_BLOCK_MAGIC2(blocks, tag_debug);
GR_SWIG_BLOCK_MAGIC2(blocks, tagged_file_sink);
GR_SWIG_BLOCK_MAGIC2(blocks, throttle);
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

#ifdef GR_CTRLPORT

%{
#include "blocks/ctrlport_probe_c.h"
#include "blocks/ctrlport_probe2_c.h"
%}

%include "blocks/ctrlport_probe_c.h"
%include "blocks/ctrlport_probe2_c.h"

GR_SWIG_BLOCK_MAGIC2(blocks, ctrlport_probe_c);
GR_SWIG_BLOCK_MAGIC2(blocks, ctrlport_probe2_c);

#endif /* GR_CTRLPORT */

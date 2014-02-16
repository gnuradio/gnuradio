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

%include "blocks_swig0_doc.i"

%{
#include "gnuradio/blocks/annotator_1to1.h"
#include "gnuradio/blocks/annotator_alltoall.h"
#include "gnuradio/blocks/annotator_raw.h"
#include "gnuradio/blocks/control_loop.h"
#include "gnuradio/blocks/copy.h"
#include "gnuradio/blocks/delay.h"
#include "gnuradio/blocks/endian_swap.h"
#include "gnuradio/blocks/file_descriptor_sink.h"
#include "gnuradio/blocks/file_descriptor_source.h"
#include "gnuradio/blocks/file_sink_base.h"
#include "gnuradio/blocks/file_sink.h"
#include "gnuradio/blocks/file_source.h"
#include "gnuradio/blocks/file_meta_sink.h"
#include "gnuradio/blocks/file_meta_source.h"
#include "gnuradio/blocks/head.h"
#include "gnuradio/blocks/message_debug.h"
#include "gnuradio/blocks/message_sink.h"
#include "gnuradio/blocks/message_source.h"
#include "gnuradio/blocks/message_strobe.h"
#include "gnuradio/blocks/message_strobe_random.h"
#include "gnuradio/blocks/message_burst_source.h"
#include "gnuradio/blocks/nop.h"
#include "gnuradio/blocks/null_sink.h"
#include "gnuradio/blocks/null_source.h"
%}

%include "gnuradio/blocks/annotator_1to1.h"
%include "gnuradio/blocks/annotator_alltoall.h"
%include "gnuradio/blocks/annotator_raw.h"
%include "gnuradio/blocks/control_loop.h"
%include "gnuradio/blocks/copy.h"
%include "gnuradio/blocks/delay.h"
%include "gnuradio/blocks/endian_swap.h"
%include "gnuradio/blocks/file_descriptor_sink.h"
%include "gnuradio/blocks/file_descriptor_source.h"
%include "gnuradio/blocks/file_sink_base.h"
%include "gnuradio/blocks/file_sink.h"
%include "gnuradio/blocks/file_source.h"
%include "gnuradio/blocks/file_meta_sink.h"
%include "gnuradio/blocks/file_meta_source.h"
%include "gnuradio/blocks/head.h"
%include "gnuradio/blocks/message_debug.h"
%include "gnuradio/blocks/message_sink.h"
%include "gnuradio/blocks/message_source.h"
%include "gnuradio/blocks/message_strobe.h"
%include "gnuradio/blocks/message_strobe_random.h"
%include "gnuradio/blocks/message_burst_source.h"
%include "gnuradio/blocks/nop.h"
%include "gnuradio/blocks/null_sink.h"
%include "gnuradio/blocks/null_source.h"

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
GR_SWIG_BLOCK_MAGIC2(blocks, message_strobe_random);
GR_SWIG_BLOCK_MAGIC2(blocks, message_burst_source);
GR_SWIG_BLOCK_MAGIC2(blocks, nop);
GR_SWIG_BLOCK_MAGIC2(blocks, null_sink);
GR_SWIG_BLOCK_MAGIC2(blocks, null_source);

#ifdef GR_CTRLPORT

%{
#include "gnuradio/blocks/ctrlport_probe_c.h"
#include "gnuradio/blocks/ctrlport_probe2_c.h"
#include "gnuradio/blocks/ctrlport_probe2_f.h"
#include "gnuradio/blocks/ctrlport_probe2_s.h"
#include "gnuradio/blocks/ctrlport_probe2_i.h"
#include "gnuradio/blocks/ctrlport_probe2_b.h"
%}

%include "gnuradio/blocks/ctrlport_probe_c.h"
%include "gnuradio/blocks/ctrlport_probe2_c.h"
%include "gnuradio/blocks/ctrlport_probe2_f.h"
%include "gnuradio/blocks/ctrlport_probe2_s.h"
%include "gnuradio/blocks/ctrlport_probe2_i.h"
%include "gnuradio/blocks/ctrlport_probe2_b.h"

GR_SWIG_BLOCK_MAGIC2(blocks, ctrlport_probe_c);
GR_SWIG_BLOCK_MAGIC2(blocks, ctrlport_probe2_c);
GR_SWIG_BLOCK_MAGIC2(blocks, ctrlport_probe2_f);
GR_SWIG_BLOCK_MAGIC2(blocks, ctrlport_probe2_s);
GR_SWIG_BLOCK_MAGIC2(blocks, ctrlport_probe2_i);
GR_SWIG_BLOCK_MAGIC2(blocks, ctrlport_probe2_b);

#endif /* GR_CTRLPORT */

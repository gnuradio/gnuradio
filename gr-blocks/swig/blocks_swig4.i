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
%include "gnuradio/blocks/pdu.h"

%include <gnuradio/endianness.h>

//load generated python docstrings
%include "blocks_swig4_doc.i"

%{
#include "gnuradio/blocks/pdu_filter.h"
#include "gnuradio/blocks/pdu_set.h"
#include "gnuradio/blocks/pdu_remove.h"
#include "gnuradio/blocks/pdu_to_tagged_stream.h"
#include "gnuradio/blocks/peak_detector.h"
#include "gnuradio/blocks/peak_detector2_fb.h"
#include "gnuradio/blocks/plateau_detector_fb.h"
#include "gnuradio/blocks/probe_rate.h"
#include "gnuradio/blocks/probe_signal.h"
#include "gnuradio/blocks/probe_signal_v.h"
%}

%include "gnuradio/blocks/probe_signal.h"
%include "gnuradio/blocks/probe_signal_v.h"
%include "gnuradio/blocks/pdu_filter.h"
%include "gnuradio/blocks/pdu_set.h"
%include "gnuradio/blocks/pdu_remove.h"
%include "gnuradio/blocks/pdu_to_tagged_stream.h"
%include "gnuradio/blocks/peak_detector.h"
%include "gnuradio/blocks/peak_detector2_fb.h"
%include "gnuradio/blocks/plateau_detector_fb.h"
%include "gnuradio/blocks/probe_rate.h"

GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, peak_detector_fb, peak_detector<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, peak_detector_ib, peak_detector<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, peak_detector_sb, peak_detector<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2(blocks, peak_detector2_fb);
GR_SWIG_BLOCK_MAGIC2(blocks, plateau_detector_fb);
GR_SWIG_BLOCK_MAGIC2(blocks, pdu_filter);
GR_SWIG_BLOCK_MAGIC2(blocks, pdu_set);
GR_SWIG_BLOCK_MAGIC2(blocks, pdu_remove);
GR_SWIG_BLOCK_MAGIC2(blocks, pdu_to_tagged_stream);
GR_SWIG_BLOCK_MAGIC2(blocks, probe_rate);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, probe_signal_b, probe_signal<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, probe_signal_s, probe_signal<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, probe_signal_i, probe_signal<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, probe_signal_f, probe_signal<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, probe_signal_c, probe_signal<gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, probe_signal_vb, probe_signal_v<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, probe_signal_vs, probe_signal_v<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, probe_signal_vi, probe_signal_v<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, probe_signal_vf, probe_signal_v<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, probe_signal_vc, probe_signal_v<gr_complex>);

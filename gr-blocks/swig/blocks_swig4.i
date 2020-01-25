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

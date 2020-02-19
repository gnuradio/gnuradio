/* -*- c++ -*- */
/*
 * Copyright 2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#define BLOCKS_API

%include "gnuradio.i"

//load generated python docstrings
%include "blocks_swig9_doc.i"

%{
#include "gnuradio/blocks/float_to_char.h"
#include "gnuradio/blocks/float_to_complex.h"
#include "gnuradio/blocks/magphase_to_complex.h"
#include "gnuradio/blocks/float_to_int.h"
#include "gnuradio/blocks/float_to_short.h"
#include "gnuradio/blocks/float_to_uchar.h"
#include "gnuradio/blocks/int_to_float.h"
#include "gnuradio/blocks/integrate.h"
#include "gnuradio/blocks/interleave.h"
#include "gnuradio/blocks/interleaved_short_to_complex.h"
#include "gnuradio/blocks/interleaved_char_to_complex.h"
#include "gnuradio/blocks/keep_m_in_n.h"
#include "gnuradio/blocks/keep_one_in_n.h"
#include "gnuradio/blocks/lfsr_32k_source_s.h"
#include "gnuradio/blocks/max_blk.h"
%}

%include "gnuradio/blocks/float_to_char.h"
%include "gnuradio/blocks/float_to_complex.h"
%include "gnuradio/blocks/magphase_to_complex.h"
%include "gnuradio/blocks/float_to_int.h"
%include "gnuradio/blocks/float_to_short.h"
%include "gnuradio/blocks/float_to_uchar.h"
%include "gnuradio/blocks/int_to_float.h"
%include "gnuradio/blocks/integrate.h"
%include "gnuradio/blocks/interleave.h"
%include "gnuradio/blocks/interleaved_short_to_complex.h"
%include "gnuradio/blocks/interleaved_char_to_complex.h"
%include "gnuradio/blocks/keep_m_in_n.h"
%include "gnuradio/blocks/keep_one_in_n.h"
%include "gnuradio/blocks/lfsr_32k_source_s.h"
%include "gnuradio/blocks/max_blk.h"

GR_SWIG_BLOCK_MAGIC2(blocks, float_to_char);
GR_SWIG_BLOCK_MAGIC2(blocks, float_to_complex);
GR_SWIG_BLOCK_MAGIC2(blocks, magphase_to_complex);
GR_SWIG_BLOCK_MAGIC2(blocks, float_to_int);
GR_SWIG_BLOCK_MAGIC2(blocks, float_to_short);
GR_SWIG_BLOCK_MAGIC2(blocks, float_to_uchar);
GR_SWIG_BLOCK_MAGIC2(blocks, int_to_float);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, integrate_ss, integrate<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, integrate_ii, integrate<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, integrate_ff, integrate<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, integrate_cc, integrate<gr_complex>);
GR_SWIG_BLOCK_MAGIC2(blocks, interleave);
GR_SWIG_BLOCK_MAGIC2(blocks, interleaved_short_to_complex);
GR_SWIG_BLOCK_MAGIC2(blocks, interleaved_char_to_complex);
GR_SWIG_BLOCK_MAGIC2(blocks, keep_m_in_n);
GR_SWIG_BLOCK_MAGIC2(blocks, keep_one_in_n);
GR_SWIG_BLOCK_MAGIC2(blocks, lfsr_32k_source_s);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, max_ff, max_blk<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, max_ii, max_blk<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, max_ss, max_blk<std::int16_t>);

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
%include "blocks_swig3_doc.i"

%{
#include "gnuradio/blocks/float_to_char.h"
#include "gnuradio/blocks/float_to_complex.h"
#include "gnuradio/blocks/float_to_int.h"
#include "gnuradio/blocks/float_to_short.h"
#include "gnuradio/blocks/float_to_uchar.h"
#include "gnuradio/blocks/int_to_float.h"
#include "gnuradio/blocks/integrate_ss.h"
#include "gnuradio/blocks/integrate_ii.h"
#include "gnuradio/blocks/integrate_ff.h"
#include "gnuradio/blocks/integrate_cc.h"
#include "gnuradio/blocks/interleave.h"
#include "gnuradio/blocks/interleaved_short_to_complex.h"
#include "gnuradio/blocks/keep_m_in_n.h"
#include "gnuradio/blocks/keep_one_in_n.h"
#include "gnuradio/blocks/lfsr_32k_source_s.h"
#include "gnuradio/blocks/max_ff.h"
#include "gnuradio/blocks/max_ii.h"
#include "gnuradio/blocks/max_ss.h"
#include "gnuradio/blocks/moving_average_cc.h"
#include "gnuradio/blocks/moving_average_ff.h"
#include "gnuradio/blocks/moving_average_ii.h"
#include "gnuradio/blocks/moving_average_ss.h"
#include "gnuradio/blocks/multiply_ss.h"
#include "gnuradio/blocks/multiply_ii.h"
#include "gnuradio/blocks/multiply_ff.h"
#include "gnuradio/blocks/multiply_cc.h"
#include "gnuradio/blocks/multiply_conjugate_cc.h"
#include "gnuradio/blocks/multiply_const_ss.h"
#include "gnuradio/blocks/multiply_const_ii.h"
#include "gnuradio/blocks/multiply_const_ff.h"
#include "gnuradio/blocks/multiply_const_cc.h"
#include "gnuradio/blocks/multiply_const_vss.h"
#include "gnuradio/blocks/multiply_const_vii.h"
#include "gnuradio/blocks/multiply_const_vff.h"
#include "gnuradio/blocks/multiply_const_vcc.h"
#include "gnuradio/blocks/mute_ss.h"
#include "gnuradio/blocks/mute_ii.h"
#include "gnuradio/blocks/mute_ff.h"
#include "gnuradio/blocks/mute_cc.h"
%}

%include "gnuradio/blocks/float_to_char.h"
%include "gnuradio/blocks/float_to_complex.h"
%include "gnuradio/blocks/float_to_int.h"
%include "gnuradio/blocks/float_to_short.h"
%include "gnuradio/blocks/float_to_uchar.h"
%include "gnuradio/blocks/int_to_float.h"
%include "gnuradio/blocks/integrate_ss.h"
%include "gnuradio/blocks/integrate_ii.h"
%include "gnuradio/blocks/integrate_ff.h"
%include "gnuradio/blocks/integrate_cc.h"
%include "gnuradio/blocks/interleave.h"
%include "gnuradio/blocks/interleaved_short_to_complex.h"
%include "gnuradio/blocks/keep_m_in_n.h"
%include "gnuradio/blocks/keep_one_in_n.h"
%include "gnuradio/blocks/lfsr_32k_source_s.h"
%include "gnuradio/blocks/max_ff.h"
%include "gnuradio/blocks/max_ii.h"
%include "gnuradio/blocks/max_ss.h"
%include "gnuradio/blocks/moving_average_cc.h"
%include "gnuradio/blocks/moving_average_ff.h"
%include "gnuradio/blocks/moving_average_ii.h"
%include "gnuradio/blocks/moving_average_ss.h"
%include "gnuradio/blocks/multiply_ss.h"
%include "gnuradio/blocks/multiply_ii.h"
%include "gnuradio/blocks/multiply_ff.h"
%include "gnuradio/blocks/multiply_cc.h"
%include "gnuradio/blocks/multiply_conjugate_cc.h"
%include "gnuradio/blocks/multiply_const_ss.h"
%include "gnuradio/blocks/multiply_const_ii.h"
%include "gnuradio/blocks/multiply_const_ff.h"
%include "gnuradio/blocks/multiply_const_cc.h"
%include "gnuradio/blocks/multiply_const_vss.h"
%include "gnuradio/blocks/multiply_const_vii.h"
%include "gnuradio/blocks/multiply_const_vff.h"
%include "gnuradio/blocks/multiply_const_vcc.h"
%include "gnuradio/blocks/mute_ss.h"
%include "gnuradio/blocks/mute_ii.h"
%include "gnuradio/blocks/mute_ff.h"
%include "gnuradio/blocks/mute_cc.h"

GR_SWIG_BLOCK_MAGIC2(blocks, float_to_char);
GR_SWIG_BLOCK_MAGIC2(blocks, float_to_complex);
GR_SWIG_BLOCK_MAGIC2(blocks, float_to_int);
GR_SWIG_BLOCK_MAGIC2(blocks, float_to_short);
GR_SWIG_BLOCK_MAGIC2(blocks, float_to_uchar);
GR_SWIG_BLOCK_MAGIC2(blocks, int_to_float);
GR_SWIG_BLOCK_MAGIC2(blocks, integrate_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, integrate_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, integrate_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, integrate_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, interleave);
GR_SWIG_BLOCK_MAGIC2(blocks, interleaved_short_to_complex);
GR_SWIG_BLOCK_MAGIC2(blocks, keep_m_in_n);
GR_SWIG_BLOCK_MAGIC2(blocks, keep_one_in_n);
GR_SWIG_BLOCK_MAGIC2(blocks, lfsr_32k_source_s);
GR_SWIG_BLOCK_MAGIC2(blocks, max_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, max_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, max_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, moving_average_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, moving_average_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, moving_average_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, moving_average_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_conjugate_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_vss);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_vii);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_vff);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_vcc);
GR_SWIG_BLOCK_MAGIC2(blocks, mute_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, mute_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, mute_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, mute_cc);

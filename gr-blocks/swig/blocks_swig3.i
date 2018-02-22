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
#include "gnuradio/blocks/multiply.h"
#include "gnuradio/blocks/multiply_conjugate_cc.h"
#include "gnuradio/blocks/multiply_const.h"
#include "gnuradio/blocks/multiply_const_v.h"
#include "gnuradio/blocks/multiply_matrix.h"
#include "gnuradio/blocks/multiply_by_tag_value_cc.h"
#include "gnuradio/blocks/mute.h"
%}

%include "gnuradio/blocks/multiply.h"
%include "gnuradio/blocks/multiply_conjugate_cc.h"
%include "gnuradio/blocks/multiply_const.h"
%include "gnuradio/blocks/multiply_const_v.h"
%include "gnuradio/blocks/multiply_matrix.h"
%include "gnuradio/blocks/multiply_by_tag_value_cc.h"
%include "gnuradio/blocks/mute.h"

GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, multiply_ss, multiply<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, multiply_ii, multiply<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, multiply_ff, multiply<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, multiply_cc, multiply<gr_complex>);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_conjugate_cc);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, multiply_const_ss, multiply_const<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, multiply_const_ii, multiply_const<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, multiply_const_ff, multiply_const<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, multiply_const_cc, multiply_const<gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, multiply_const_vss, multiply_const_v<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, multiply_const_vii, multiply_const_v<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, multiply_const_vff, multiply_const_v<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, multiply_const_vcc, multiply_const_v<gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, multiply_matrix_cc, multiply_matrix<gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, multiply_matrix_ff, multiply_matrix<float>);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_by_tag_value_cc);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, mute_ss, mute_blk<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, mute_ii, mute_blk<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, mute_ff, mute_blk<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, mute_cc, mute_blk<gr_complex>);

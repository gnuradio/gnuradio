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
#include "gnuradio/blocks/multiply_matrix_cc.h"
#include "gnuradio/blocks/multiply_matrix_ff.h"
#include "gnuradio/blocks/multiply_by_tag_value_cc.h"
#include "gnuradio/blocks/mute_ss.h"
#include "gnuradio/blocks/mute_ii.h"
#include "gnuradio/blocks/mute_ff.h"
#include "gnuradio/blocks/mute_cc.h"
%}

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
%include "gnuradio/blocks/multiply_matrix_cc.h"
%include "gnuradio/blocks/multiply_matrix_ff.h"
%include "gnuradio/blocks/multiply_by_tag_value_cc.h"
%include "gnuradio/blocks/mute_ss.h"
%include "gnuradio/blocks/mute_ii.h"
%include "gnuradio/blocks/mute_ff.h"
%include "gnuradio/blocks/mute_cc.h"

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
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_matrix_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_matrix_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_by_tag_value_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, mute_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, mute_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, mute_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, mute_cc);

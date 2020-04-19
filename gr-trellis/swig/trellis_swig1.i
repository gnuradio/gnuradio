/*
 * Copyright 2016,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#define TRELLIS_API
#define DIGITAL_API

%include "gnuradio.i"

//load generated python docstrings
%include "trellis_swig1_doc.i"

%include "gnuradio/digital/metric_type.h"
%include "gnuradio/trellis/siso_type.h"
%include "gnuradio/trellis/fsm.h"
%include "gnuradio/trellis/interleaver.h"

%{
#include "gnuradio/trellis/pccc_decoder_blk.h"
#include "gnuradio/trellis/viterbi.h"
#include "gnuradio/trellis/viterbi_combined.h"
%}

%include "gnuradio/trellis/pccc_decoder_blk.h"
%include "gnuradio/trellis/viterbi.h"
%include "gnuradio/trellis/viterbi_combined.h"

GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, pccc_decoder_b, pccc_decoder_blk<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, pccc_decoder_s, pccc_decoder_blk<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, pccc_decoder_i, pccc_decoder_blk<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, viterbi_b, viterbi<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, viterbi_s, viterbi<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, viterbi_i, viterbi<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, viterbi_combined_sb, viterbi_combined<std::int16_t,std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, viterbi_combined_ss, viterbi_combined<std::int16_t,std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, viterbi_combined_si, viterbi_combined<std::int16_t,std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, viterbi_combined_ib, viterbi_combined<std::int32_t,std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, viterbi_combined_is, viterbi_combined<std::int32_t,std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, viterbi_combined_ii, viterbi_combined<std::int32_t,std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, viterbi_combined_fb, viterbi_combined<float,std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, viterbi_combined_fs, viterbi_combined<float,std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, viterbi_combined_fi, viterbi_combined<float,std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, viterbi_combined_cb, viterbi_combined<gr_complex,std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, viterbi_combined_cs, viterbi_combined<gr_complex,std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, viterbi_combined_ci, viterbi_combined<gr_complex,std::int32_t>);

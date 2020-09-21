/*
 * Copyright 2016,2020 Free Software Foundation, Inc.
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
 *
 */

#define TRELLIS_API
#define DIGITAL_API

%include "gnuradio.i"

//load generated python docstrings
%include "trellis_swig3_doc.i"

%include "gnuradio/digital/metric_type.h"
%include "gnuradio/trellis/siso_type.h"
%include "gnuradio/trellis/fsm.h"
%include "gnuradio/trellis/interleaver.h"

%{
#include "gnuradio/trellis/sccc_decoder_blk.h"
#include "gnuradio/trellis/sccc_decoder_combined_blk.h"
#include "gnuradio/trellis/pccc_decoder_combined_blk.h"
%}

%include "gnuradio/trellis/sccc_decoder_blk.h"
%include "gnuradio/trellis/sccc_decoder_combined_blk.h"
%include "gnuradio/trellis/pccc_decoder_combined_blk.h"

GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, sccc_decoder_b, sccc_decoder_blk<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, sccc_decoder_s, sccc_decoder_blk<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, sccc_decoder_i, sccc_decoder_blk<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, sccc_decoder_combined_fb, sccc_decoder_combined_blk<float, std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, sccc_decoder_combined_fs, sccc_decoder_combined_blk<float, std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, sccc_decoder_combined_fi, sccc_decoder_combined_blk<float, std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, sccc_decoder_combined_cb, sccc_decoder_combined_blk<gr_complex, std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, sccc_decoder_combined_cs, sccc_decoder_combined_blk<gr_complex, std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, sccc_decoder_combined_ci, sccc_decoder_combined_blk<gr_complex, std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, pccc_decoder_combined_fb, pccc_decoder_combined_blk<float, std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, pccc_decoder_combined_fs, pccc_decoder_combined_blk<float, std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, pccc_decoder_combined_fi, pccc_decoder_combined_blk<float, std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, pccc_decoder_combined_cb, pccc_decoder_combined_blk<gr_complex, std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, pccc_decoder_combined_cs, pccc_decoder_combined_blk<gr_complex, std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, pccc_decoder_combined_ci, pccc_decoder_combined_blk<gr_complex, std::int32_t>);

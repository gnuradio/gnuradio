/*
 * Copyright 2012,2016,2020 Free Software Foundation, Inc.
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
%include "trellis_swig2_doc.i"

%include "gnuradio/digital/metric_type.h"
%include "gnuradio/trellis/siso_type.h"
%include "gnuradio/trellis/fsm.h"
%include "gnuradio/trellis/interleaver.h"

%{
#include "gnuradio/trellis/pccc_encoder.h"
#include "gnuradio/trellis/metrics.h"
#include "gnuradio/trellis/siso_f.h"
%}

%include "gnuradio/trellis/pccc_encoder.h"
%include "gnuradio/trellis/metrics.h"
%include "gnuradio/trellis/siso_f.h"

GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, pccc_encoder_bb, pccc_encoder<std::uint8_t,std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, pccc_encoder_bs, pccc_encoder<std::uint8_t,std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, pccc_encoder_bi, pccc_encoder<std::uint8_t,std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, pccc_encoder_ss, pccc_encoder<std::int16_t,std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, pccc_encoder_si, pccc_encoder<std::int16_t,std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, pccc_encoder_ii, pccc_encoder<std::int32_t,std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, metrics_s, metrics<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, metrics_i, metrics<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, metrics_f, metrics<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, metrics_c, metrics<gr_complex>);

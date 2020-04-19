/*
 * Copyright 2012,2016,2020 Free Software Foundation, Inc.
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

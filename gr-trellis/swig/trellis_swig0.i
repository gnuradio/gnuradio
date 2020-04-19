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
%include "trellis_swig0_doc.i"

%include "gnuradio/digital/metric_type.h"
%include "gnuradio/digital/constellation.h"
%include "gnuradio/trellis/siso_type.h"
%include "gnuradio/trellis/fsm.h"
%include "gnuradio/trellis/interleaver.h"

%{
#include "gnuradio/trellis/constellation_metrics_cf.h"
#include "gnuradio/trellis/permutation.h"
#include "gnuradio/trellis/siso_combined_f.h"
#include "gnuradio/trellis/siso_f.h"
#include "gnuradio/trellis/encoder.h"
#include "gnuradio/trellis/sccc_encoder.h"
%}

%include "gnuradio/trellis/constellation_metrics_cf.h"
%include "gnuradio/trellis/permutation.h"
%include "gnuradio/trellis/siso_combined_f.h"
%include "gnuradio/trellis/siso_f.h"
%include "gnuradio/trellis/encoder.h"
%include "gnuradio/trellis/sccc_encoder.h"

GR_SWIG_BLOCK_MAGIC2(trellis, constellation_metrics_cf);
GR_SWIG_BLOCK_MAGIC2(trellis, permutation);
GR_SWIG_BLOCK_MAGIC2(trellis, siso_combined_f);
GR_SWIG_BLOCK_MAGIC2(trellis, siso_f);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, encoder_bb, encoder<std::uint8_t,std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, encoder_bs, encoder<std::uint8_t,std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, encoder_bi, encoder<std::uint8_t,std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, encoder_ss, encoder<std::int16_t,std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, encoder_si, encoder<std::int16_t,std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, encoder_ii, encoder<std::int32_t,std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, sccc_encoder_bb, sccc_encoder<std::uint8_t,std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, sccc_encoder_bs, sccc_encoder<std::uint8_t,std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, sccc_encoder_bi, sccc_encoder<std::uint8_t,std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, sccc_encoder_ss, sccc_encoder<std::int16_t,std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, sccc_encoder_si, sccc_encoder<std::int16_t,std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(trellis, sccc_encoder_ii, sccc_encoder<std::int32_t,std::int32_t>);

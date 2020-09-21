/* -*- c++ -*- */
/*
 * Copyright 2014,2015,2016,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#define DTV_API

%include "gnuradio.i"

//load generated python docstrings
%include "dtv_swig0_doc.i"

%{
#include "gnuradio/dtv/atsc_deinterleaver.h"
#include "gnuradio/dtv/atsc_depad.h"
#include "gnuradio/dtv/atsc_derandomizer.h"
#include "gnuradio/dtv/atsc_equalizer.h"
#include "gnuradio/dtv/atsc_field_sync_mux.h"
#include "gnuradio/dtv/atsc_fpll.h"
#include "gnuradio/dtv/atsc_fs_checker.h"
#include "gnuradio/dtv/atsc_interleaver.h"
#include "gnuradio/dtv/atsc_pad.h"
#include "gnuradio/dtv/atsc_randomizer.h"
#include "gnuradio/dtv/atsc_rs_decoder.h"
#include "gnuradio/dtv/atsc_rs_encoder.h"
#include "gnuradio/dtv/atsc_sync.h"
#include "gnuradio/dtv/atsc_trellis_encoder.h"
#include "gnuradio/dtv/atsc_viterbi_decoder.h"
#include "gnuradio/dtv/catv_config.h"
#include "gnuradio/dtv/catv_transport_framing_enc_bb.h"
#include "gnuradio/dtv/catv_reed_solomon_enc_bb.h"
#include "gnuradio/dtv/catv_randomizer_bb.h"
#include "gnuradio/dtv/catv_frame_sync_enc_bb.h"
#include "gnuradio/dtv/catv_trellis_enc_bb.h"
%}

%include "gnuradio/dtv/atsc_deinterleaver.h"
%include "gnuradio/dtv/atsc_depad.h"
%include "gnuradio/dtv/atsc_derandomizer.h"
%include "gnuradio/dtv/atsc_equalizer.h"
%include "gnuradio/dtv/atsc_field_sync_mux.h"
%include "gnuradio/dtv/atsc_fpll.h"
%include "gnuradio/dtv/atsc_fs_checker.h"
%include "gnuradio/dtv/atsc_interleaver.h"
%include "gnuradio/dtv/atsc_pad.h"
%include "gnuradio/dtv/atsc_randomizer.h"
%include "gnuradio/dtv/atsc_rs_decoder.h"
%include "gnuradio/dtv/atsc_rs_encoder.h"
%include "gnuradio/dtv/atsc_sync.h"
%include "gnuradio/dtv/atsc_trellis_encoder.h"
%include "gnuradio/dtv/atsc_viterbi_decoder.h"
%include "gnuradio/dtv/catv_config.h"
%include "gnuradio/dtv/catv_transport_framing_enc_bb.h"
%include "gnuradio/dtv/catv_reed_solomon_enc_bb.h"
%include "gnuradio/dtv/catv_randomizer_bb.h"
%include "gnuradio/dtv/catv_frame_sync_enc_bb.h"
%include "gnuradio/dtv/catv_trellis_enc_bb.h"

GR_SWIG_BLOCK_MAGIC2(dtv, atsc_deinterleaver);
GR_SWIG_BLOCK_MAGIC2(dtv, atsc_depad);
GR_SWIG_BLOCK_MAGIC2(dtv, atsc_derandomizer);
GR_SWIG_BLOCK_MAGIC2(dtv, atsc_equalizer)
GR_SWIG_BLOCK_MAGIC2(dtv, atsc_field_sync_mux);
GR_SWIG_BLOCK_MAGIC2(dtv, atsc_fpll);
GR_SWIG_BLOCK_MAGIC2(dtv, atsc_fs_checker);
GR_SWIG_BLOCK_MAGIC2(dtv, atsc_interleaver);
GR_SWIG_BLOCK_MAGIC2(dtv, atsc_pad);
GR_SWIG_BLOCK_MAGIC2(dtv, atsc_randomizer);
GR_SWIG_BLOCK_MAGIC2(dtv, atsc_rs_decoder);
GR_SWIG_BLOCK_MAGIC2(dtv, atsc_rs_encoder);
GR_SWIG_BLOCK_MAGIC2(dtv, atsc_sync);
GR_SWIG_BLOCK_MAGIC2(dtv, atsc_trellis_encoder);
GR_SWIG_BLOCK_MAGIC2(dtv, atsc_viterbi_decoder);
GR_SWIG_BLOCK_MAGIC2(dtv, catv_transport_framing_enc_bb);
GR_SWIG_BLOCK_MAGIC2(dtv, catv_reed_solomon_enc_bb);
GR_SWIG_BLOCK_MAGIC2(dtv, catv_randomizer_bb);
GR_SWIG_BLOCK_MAGIC2(dtv, catv_frame_sync_enc_bb);
GR_SWIG_BLOCK_MAGIC2(dtv, catv_trellis_enc_bb);

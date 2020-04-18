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
%include "dtv_swig2_doc.i"

%include "gnuradio/dtv/dvb_config.h"

%{
#include "gnuradio/dtv/dvbt_config.h"
#include "gnuradio/dtv/dvbt_energy_dispersal.h"
#include "gnuradio/dtv/dvbt_reed_solomon_enc.h"
#include "gnuradio/dtv/dvbt_convolutional_interleaver.h"
#include "gnuradio/dtv/dvbt_inner_coder.h"
#include "gnuradio/dtv/dvbt_bit_inner_interleaver.h"
#include "gnuradio/dtv/dvbt_symbol_inner_interleaver.h"
#include "gnuradio/dtv/dvbt_map.h"
#include "gnuradio/dtv/dvbt_reference_signals.h"
#include "gnuradio/dtv/dvbt_ofdm_sym_acquisition.h"
#include "gnuradio/dtv/dvbt_demod_reference_signals.h"
#include "gnuradio/dtv/dvbt_demap.h"
#include "gnuradio/dtv/dvbt_bit_inner_deinterleaver.h"
#include "gnuradio/dtv/dvbt_viterbi_decoder.h"
#include "gnuradio/dtv/dvbt_convolutional_deinterleaver.h"
#include "gnuradio/dtv/dvbt_reed_solomon_dec.h"
#include "gnuradio/dtv/dvbt_energy_descramble.h"
%}

%include "gnuradio/dtv/dvbt_config.h"
%include "gnuradio/dtv/dvbt_energy_dispersal.h"
%include "gnuradio/dtv/dvbt_reed_solomon_enc.h"
%include "gnuradio/dtv/dvbt_convolutional_interleaver.h"
%include "gnuradio/dtv/dvbt_inner_coder.h"
%include "gnuradio/dtv/dvbt_bit_inner_interleaver.h"
%include "gnuradio/dtv/dvbt_symbol_inner_interleaver.h"
%include "gnuradio/dtv/dvbt_map.h"
%include "gnuradio/dtv/dvbt_reference_signals.h"
%include "gnuradio/dtv/dvbt_ofdm_sym_acquisition.h"
%include "gnuradio/dtv/dvbt_demod_reference_signals.h"
%include "gnuradio/dtv/dvbt_demap.h"
%include "gnuradio/dtv/dvbt_bit_inner_deinterleaver.h"
%include "gnuradio/dtv/dvbt_viterbi_decoder.h"
%include "gnuradio/dtv/dvbt_convolutional_deinterleaver.h"
%include "gnuradio/dtv/dvbt_reed_solomon_dec.h"
%include "gnuradio/dtv/dvbt_energy_descramble.h"

GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_energy_dispersal);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_reed_solomon_enc);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_convolutional_interleaver);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_inner_coder);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_bit_inner_interleaver);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_symbol_inner_interleaver);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_map);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_reference_signals);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_ofdm_sym_acquisition);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_demod_reference_signals);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_demap);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_bit_inner_deinterleaver);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_viterbi_decoder);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_convolutional_deinterleaver);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_reed_solomon_dec);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt_energy_descramble);

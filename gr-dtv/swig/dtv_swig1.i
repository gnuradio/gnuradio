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
%include "dtv_swig1_doc.i"

%{
#include "gnuradio/dtv/dvb_config.h"
#include "gnuradio/dtv/dvbt2_config.h"
#include "gnuradio/dtv/dvbs2_config.h"
#include "gnuradio/dtv/dvb_bbheader_bb.h"
#include "gnuradio/dtv/dvb_bbscrambler_bb.h"
#include "gnuradio/dtv/dvb_bch_bb.h"
#include "gnuradio/dtv/dvb_ldpc_bb.h"
#include "gnuradio/dtv/dvbt2_interleaver_bb.h"
#include "gnuradio/dtv/dvbt2_modulator_bc.h"
#include "gnuradio/dtv/dvbt2_cellinterleaver_cc.h"
#include "gnuradio/dtv/dvbt2_framemapper_cc.h"
#include "gnuradio/dtv/dvbt2_freqinterleaver_cc.h"
#include "gnuradio/dtv/dvbt2_pilotgenerator_cc.h"
#include "gnuradio/dtv/dvbt2_paprtr_cc.h"
#include "gnuradio/dtv/dvbt2_p1insertion_cc.h"
#include "gnuradio/dtv/dvbt2_miso_cc.h"
#include "gnuradio/dtv/dvbs2_interleaver_bb.h"
#include "gnuradio/dtv/dvbs2_modulator_bc.h"
#include "gnuradio/dtv/dvbs2_physical_cc.h"
%}

%include "gnuradio/dtv/dvb_config.h"
%include "gnuradio/dtv/dvbt2_config.h"
%include "gnuradio/dtv/dvbs2_config.h"
%include "gnuradio/dtv/dvb_bbheader_bb.h"
%include "gnuradio/dtv/dvb_bbscrambler_bb.h"
%include "gnuradio/dtv/dvb_bch_bb.h"
%include "gnuradio/dtv/dvb_ldpc_bb.h"
%include "gnuradio/dtv/dvbt2_interleaver_bb.h"
%include "gnuradio/dtv/dvbt2_modulator_bc.h"
%include "gnuradio/dtv/dvbt2_cellinterleaver_cc.h"
%include "gnuradio/dtv/dvbt2_framemapper_cc.h"
%include "gnuradio/dtv/dvbt2_freqinterleaver_cc.h"
%include "gnuradio/dtv/dvbt2_pilotgenerator_cc.h"
%include "gnuradio/dtv/dvbt2_paprtr_cc.h"
%include "gnuradio/dtv/dvbt2_p1insertion_cc.h"
%include "gnuradio/dtv/dvbt2_miso_cc.h"
%include "gnuradio/dtv/dvbs2_interleaver_bb.h"
%include "gnuradio/dtv/dvbs2_modulator_bc.h"
%include "gnuradio/dtv/dvbs2_physical_cc.h"

GR_SWIG_BLOCK_MAGIC2(dtv, dvb_bbheader_bb);
GR_SWIG_BLOCK_MAGIC2(dtv, dvb_bbscrambler_bb);
GR_SWIG_BLOCK_MAGIC2(dtv, dvb_bch_bb);
GR_SWIG_BLOCK_MAGIC2(dtv, dvb_ldpc_bb);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt2_interleaver_bb);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt2_modulator_bc);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt2_cellinterleaver_cc);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt2_framemapper_cc);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt2_freqinterleaver_cc);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt2_pilotgenerator_cc);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt2_paprtr_cc);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt2_p1insertion_cc);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbt2_miso_cc);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbs2_interleaver_bb);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbs2_modulator_bc);
GR_SWIG_BLOCK_MAGIC2(dtv, dvbs2_physical_cc);

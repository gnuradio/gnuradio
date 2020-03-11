/* -*- c++ -*- */
/*
 * Copyright 2011,2013,2016,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#define VOCODER_API

%include "gnuradio.i"

//load generated python docstrings
%include "vocoder_swig_doc.i"

%{
#include "gnuradio/vocoder/alaw_decode_bs.h"
#include "gnuradio/vocoder/alaw_encode_sb.h"
#include "gnuradio/vocoder/cvsd_decode_bs.h"
#include "gnuradio/vocoder/cvsd_encode_sb.h"
#include "gnuradio/vocoder/g721_decode_bs.h"
#include "gnuradio/vocoder/g721_encode_sb.h"
#include "gnuradio/vocoder/g723_24_decode_bs.h"
#include "gnuradio/vocoder/g723_24_encode_sb.h"
#include "gnuradio/vocoder/g723_40_decode_bs.h"
#include "gnuradio/vocoder/g723_40_encode_sb.h"
#include "gnuradio/vocoder/ulaw_decode_bs.h"
#include "gnuradio/vocoder/ulaw_encode_sb.h"
%}

%include "gnuradio/vocoder/alaw_decode_bs.h"
%include "gnuradio/vocoder/alaw_encode_sb.h"
%include "gnuradio/vocoder/cvsd_decode_bs.h"
%include "gnuradio/vocoder/cvsd_encode_sb.h"
%include "gnuradio/vocoder/g721_decode_bs.h"
%include "gnuradio/vocoder/g721_encode_sb.h"
%include "gnuradio/vocoder/g723_24_decode_bs.h"
%include "gnuradio/vocoder/g723_24_encode_sb.h"
%include "gnuradio/vocoder/g723_40_decode_bs.h"
%include "gnuradio/vocoder/g723_40_encode_sb.h"
%include "gnuradio/vocoder/ulaw_decode_bs.h"
%include "gnuradio/vocoder/ulaw_encode_sb.h"

GR_SWIG_BLOCK_MAGIC2(vocoder, alaw_decode_bs);
GR_SWIG_BLOCK_MAGIC2(vocoder, alaw_encode_sb);
GR_SWIG_BLOCK_MAGIC2(vocoder, cvsd_decode_bs);
GR_SWIG_BLOCK_MAGIC2(vocoder, cvsd_encode_sb);
GR_SWIG_BLOCK_MAGIC2(vocoder, g721_decode_bs);
GR_SWIG_BLOCK_MAGIC2(vocoder, g721_encode_sb);
GR_SWIG_BLOCK_MAGIC2(vocoder, g723_24_decode_bs);
GR_SWIG_BLOCK_MAGIC2(vocoder, g723_24_encode_sb);
GR_SWIG_BLOCK_MAGIC2(vocoder, g723_40_decode_bs);
GR_SWIG_BLOCK_MAGIC2(vocoder, g723_40_encode_sb);
GR_SWIG_BLOCK_MAGIC2(vocoder, ulaw_decode_bs);
GR_SWIG_BLOCK_MAGIC2(vocoder, ulaw_encode_sb);

#ifdef LIBCODEC2_FOUND
%{
#include <codec2/codec2.h>
#include "gnuradio/vocoder/codec2.h"
#include "gnuradio/vocoder/codec2_decode_ps.h"
#include "gnuradio/vocoder/codec2_encode_sp.h"
%}

%include <codec2/codec2.h>
%include "gnuradio/vocoder/codec2.h"
%include "gnuradio/vocoder/codec2_decode_ps.h"
%include "gnuradio/vocoder/codec2_encode_sp.h"

GR_SWIG_BLOCK_MAGIC2(vocoder, codec2_decode_ps);
GR_SWIG_BLOCK_MAGIC2(vocoder, codec2_encode_sp);
#endif

#ifdef LIBCODEC2_HAS_FREEDV_API
%{
#include <codec2/freedv_api.h>
#include "gnuradio/vocoder/freedv_api.h"
#include "gnuradio/vocoder/freedv_rx_ss.h"
#include "gnuradio/vocoder/freedv_tx_ss.h"
%}

#ifdef FREEDV_MODE_700D
#else
%ignore freedv_set_smooth_symbols;
%ignore freedv_set_clip;
#endif

%include <codec2/freedv_api.h>
%include "gnuradio/vocoder/freedv_api.h"
%include "gnuradio/vocoder/freedv_rx_ss.h"
%include "gnuradio/vocoder/freedv_tx_ss.h"

GR_SWIG_BLOCK_MAGIC2(vocoder, freedv_rx_ss);
GR_SWIG_BLOCK_MAGIC2(vocoder, freedv_tx_ss);
#endif

#ifdef LIBGSM_FOUND
%{
#include "gnuradio/vocoder/gsm_fr_decode_ps.h"
#include "gnuradio/vocoder/gsm_fr_encode_sp.h"
%}

%include "gnuradio/vocoder/gsm_fr_decode_ps.h"
%include "gnuradio/vocoder/gsm_fr_encode_sp.h"

GR_SWIG_BLOCK_MAGIC2(vocoder, gsm_fr_decode_ps);
GR_SWIG_BLOCK_MAGIC2(vocoder, gsm_fr_encode_sp);
#endif

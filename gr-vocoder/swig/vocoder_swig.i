/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
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

#define VOCODER_API

%include "gnuradio.i"

//load generated python docstrings
%include "vocoder_swig_doc.i"

%{
#include "gnuradio/vocoder/alaw_decode_bs.h"
#include "gnuradio/vocoder/alaw_encode_sb.h"
#include "gnuradio/vocoder/codec2.h"
#include "gnuradio/vocoder/codec2_decode_ps.h"
#include "gnuradio/vocoder/codec2_encode_sp.h"
#include "gnuradio/vocoder/cvsd_decode_bs.h"
#include "gnuradio/vocoder/cvsd_encode_sb.h"
#include "gnuradio/vocoder/g721_decode_bs.h"
#include "gnuradio/vocoder/g721_encode_sb.h"
#include "gnuradio/vocoder/g723_24_decode_bs.h"
#include "gnuradio/vocoder/g723_24_encode_sb.h"
#include "gnuradio/vocoder/g723_40_decode_bs.h"
#include "gnuradio/vocoder/g723_40_encode_sb.h"
#include "gnuradio/vocoder/gsm_fr_decode_ps.h"
#include "gnuradio/vocoder/gsm_fr_encode_sp.h"
#include "gnuradio/vocoder/ulaw_decode_bs.h"
#include "gnuradio/vocoder/ulaw_encode_sb.h"
%}

%include "gnuradio/vocoder/alaw_decode_bs.h"
%include "gnuradio/vocoder/alaw_encode_sb.h"
%include "gnuradio/vocoder/codec2.h"
%include "gnuradio/vocoder/codec2_decode_ps.h"
%include "gnuradio/vocoder/codec2_encode_sp.h"
%include "gnuradio/vocoder/cvsd_decode_bs.h"
%include "gnuradio/vocoder/cvsd_encode_sb.h"
%include "gnuradio/vocoder/g721_decode_bs.h"
%include "gnuradio/vocoder/g721_encode_sb.h"
%include "gnuradio/vocoder/g723_24_decode_bs.h"
%include "gnuradio/vocoder/g723_24_encode_sb.h"
%include "gnuradio/vocoder/g723_40_decode_bs.h"
%include "gnuradio/vocoder/g723_40_encode_sb.h"
%include "gnuradio/vocoder/gsm_fr_decode_ps.h"
%include "gnuradio/vocoder/gsm_fr_encode_sp.h"
%include "gnuradio/vocoder/ulaw_decode_bs.h"
%include "gnuradio/vocoder/ulaw_encode_sb.h"

GR_SWIG_BLOCK_MAGIC2(vocoder, alaw_decode_bs);
GR_SWIG_BLOCK_MAGIC2(vocoder, alaw_encode_sb);
GR_SWIG_BLOCK_MAGIC2(vocoder, codec2_decode_ps);
GR_SWIG_BLOCK_MAGIC2(vocoder, codec2_encode_sp);
GR_SWIG_BLOCK_MAGIC2(vocoder, cvsd_decode_bs);
GR_SWIG_BLOCK_MAGIC2(vocoder, cvsd_encode_sb);
GR_SWIG_BLOCK_MAGIC2(vocoder, g721_decode_bs);
GR_SWIG_BLOCK_MAGIC2(vocoder, g721_encode_sb);
GR_SWIG_BLOCK_MAGIC2(vocoder, g723_24_decode_bs);
GR_SWIG_BLOCK_MAGIC2(vocoder, g723_24_encode_sb);
GR_SWIG_BLOCK_MAGIC2(vocoder, g723_40_decode_bs);
GR_SWIG_BLOCK_MAGIC2(vocoder, g723_40_encode_sb);
GR_SWIG_BLOCK_MAGIC2(vocoder, gsm_fr_decode_ps);
GR_SWIG_BLOCK_MAGIC2(vocoder, gsm_fr_encode_sp);
GR_SWIG_BLOCK_MAGIC2(vocoder, ulaw_decode_bs);
GR_SWIG_BLOCK_MAGIC2(vocoder, ulaw_encode_sb);

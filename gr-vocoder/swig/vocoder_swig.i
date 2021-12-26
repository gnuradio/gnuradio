/* -*- c++ -*- */
/*
 * Copyright 2011,2013,2016,2019 Free Software Foundation, Inc.
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
// == SWIG does not like standards ==
// SWIG (as of SWIG 4.0.2) emulates C99 complex number support: C99 has a keyword
// `_Complex`, and a header <complex.h>, which `#define`s a macro `complex` to be
// replaced by `_Complex`. So, `_Complex` is the C99 keyword, `complex` the macro.
//
// Which means that of course SWIG treats `complex` as keyword and `_Complex` as
// completely unknown token never to be used where a type specifier keyword was to
// be expected.
//
// So we need to be the ones to jump through hoops. This potentially means we're
// breaking future builds, when `_Complex` actually becomes supported and
// `complex` actually a macro, just as standardized. Do we get a choice? Nope.
//
// To top this off, codec2 uses _Complex float, which is not strictly C99
// (http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf); correct would be
// float _Complex, but every serious compiler accepts either.
//
// The solution is to "define away" the whole _Complex.
// This of course breaks the one symbol (as of codec2 1.0.1) that uses _Complex
// in codec2/freedv_api.h, but that symbol ended up there by accident anyway, and
// on codec2 master (git commit 7a554bad) it's moved out of the API.
//
// However, codec2, although it uses three-digit versioning, exports only a
// two-digit API number, so we can't even check for "Codec 2 below 1.0.2"
// (https://github.com/drowe67/codec2/issues/269). I don't have a great solution
// yet; there's no visible #defines after v1.0.1 on master yet that we could abuse
// as feature test macro.

#ifdef CODEC2_HAVE_VERSION
  #ifndef CODEC2_VERSION_PATCH
    #define CODEC2_VERSION_PATCH 0
  #endif
  #define _CODEC2_VERSION_NUMERICAL  (100000 * CODEC2_VERSION_MAJOR + \
                                      1000 * CODEC2_VERSION_MINOR + \
                                      CODEC2_VERSION_PATCH )
#else
  #define _CODEC2_VERSION_NUMERICAL 0
#endif
#if _CODEC2_VERSION_NUMERICAL < 1001002
  #define _Complex
  #define SWIG_COMPLEX_HELL_VOCODER_SWIG
#endif

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

#ifdef SWIG_COMPLEX_HELL_VOCODER_SWIG
  #undef freedv_callback_rx_sym
  #undef _Complex
#endif
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

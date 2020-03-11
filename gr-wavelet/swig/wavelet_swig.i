/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#define WAVELET_API

%include "gnuradio.i"

//load generated python docstrings
%include "wavelet_swig_doc.i"

%{
#include "gnuradio/wavelet/squash_ff.h"
#include "gnuradio/wavelet/wavelet_ff.h"
#include "gnuradio/wavelet/wvps_ff.h"
%}

%include "gnuradio/wavelet/squash_ff.h"
%include "gnuradio/wavelet/wavelet_ff.h"
%include "gnuradio/wavelet/wvps_ff.h"

GR_SWIG_BLOCK_MAGIC2(wavelet, squash_ff);
GR_SWIG_BLOCK_MAGIC2(wavelet, wavelet_ff);
GR_SWIG_BLOCK_MAGIC2(wavelet, wvps_ff);

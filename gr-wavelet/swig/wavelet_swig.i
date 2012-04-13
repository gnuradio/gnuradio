/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#define WAVELET_API

%include "gnuradio.i"

//load generated python docstrings
%include "wavelet_swig_doc.i"

%{
#include "wavelet_squash_ff.h"
#include "wavelet_wavelet_ff.h"
#include "wavelet_wvps_ff.h"
%}

%include "wavelet_squash_ff.h"
%include "wavelet_wavelet_ff.h"
%include "wavelet_wvps_ff.h"

GR_SWIG_BLOCK_MAGIC(wavelet,squash_ff);
wavelet_squash_ff_sptr wavelet_make_squash_ff(const std::vector<float> &igrid,
					      const std::vector<float> &ogrid);

GR_SWIG_BLOCK_MAGIC(wavelet,wavelet_ff);
wavelet_wavelet_ff_sptr wavelet_make_wavelet_ff(int size, int order, bool forward);

GR_SWIG_BLOCK_MAGIC(wavelet,wvps_ff);
wavelet_wvps_ff_sptr wavelet_make_wvps_ff(int ilen);

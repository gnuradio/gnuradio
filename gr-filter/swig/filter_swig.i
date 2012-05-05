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

#define FILTER_API

%include "gnuradio.i"

//load generated python docstrings
%include "gr_filter_swig_doc.i"

%{
#include "filter/firdes.h"
#include "filter/pm_remez.h"
#include "filter/fir_filter_fff.h"
#include "filter/fir_filter_ccf.h"
#include "filter/fir_filter_ccc.h"
#include "filter/fft_filter_ccc.h"
#include "filter/fft_filter_fff.h"
#include "filter/hilbert_fc.h"
%}

%include "filter/firdes.h"
%include "filter/pm_remez.h"
%include "filter/fir_filter_fff.h"
%include "filter/fir_filter_ccf.h"
%include "filter/fir_filter_ccc.h"
%include "filter/fft_filter_ccc.h"
%include "filter/fft_filter_fff.h"
%include "filter/hilbert_fc.h"

GR_SWIG_BLOCK_MAGIC2(filter, fir_filter_fff);
GR_SWIG_BLOCK_MAGIC2(filter, fir_filter_ccf);
GR_SWIG_BLOCK_MAGIC2(filter, fir_filter_ccc);
GR_SWIG_BLOCK_MAGIC2(filter, fft_filter_ccc);
GR_SWIG_BLOCK_MAGIC2(filter, fft_filter_fff);
GR_SWIG_BLOCK_MAGIC2(filter, hilbert_fc);

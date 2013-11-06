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

#define FFT_API

%include "gnuradio.i"

//load generated python docstrings
%include "fft_swig_doc.i"

%{
#include "gnuradio/fft/fft_vcc.h"
#include "gnuradio/fft/fft_vfc.h"
#include "gnuradio/fft/goertzel_fc.h"
#include "gnuradio/fft/window.h"
%}

%include "gnuradio/fft/fft_vcc.h"
%include "gnuradio/fft/fft_vfc.h"
%include "gnuradio/fft/goertzel_fc.h"
%include "gnuradio/fft/window.h"

GR_SWIG_BLOCK_MAGIC2(fft, fft_vcc);
GR_SWIG_BLOCK_MAGIC2(fft, fft_vfc);
GR_SWIG_BLOCK_MAGIC2(fft, goertzel_fc);

#ifdef GR_CTRLPORT

%{
#include "gnuradio/fft/ctrlport_probe_psd.h"
%}

%include "gnuradio/fft/ctrlport_probe_psd.h"

GR_SWIG_BLOCK_MAGIC2(fft, ctrlport_probe_psd);

#endif /* GR_CTRLPORT */


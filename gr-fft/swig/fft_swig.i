/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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


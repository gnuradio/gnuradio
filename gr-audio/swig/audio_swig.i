/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#define GR_AUDIO_API

%include "gnuradio.i"

//load generated python docstrings
%include "audio_swig_doc.i"

%{
#include <gnuradio/audio/source.h>
#include <gnuradio/audio/sink.h>
%}

%include <gnuradio/audio/source.h>
%include <gnuradio/audio/sink.h>

GR_SWIG_BLOCK_MAGIC2(audio, source)
GR_SWIG_BLOCK_MAGIC2(audio, sink)

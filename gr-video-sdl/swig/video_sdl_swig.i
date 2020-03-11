/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#define VIDEO_SDL_API

%include "gnuradio.i"

//load generated python docstrings
%include "video_sdl_swig_doc.i"

%{
#include "gnuradio/video_sdl/sink_uc.h"
#include "gnuradio/video_sdl/sink_s.h"
%}

%include "gnuradio/video_sdl/sink_uc.h"
%include "gnuradio/video_sdl/sink_s.h"

GR_SWIG_BLOCK_MAGIC2(video_sdl, sink_uc);
GR_SWIG_BLOCK_MAGIC2(video_sdl, sink_s);

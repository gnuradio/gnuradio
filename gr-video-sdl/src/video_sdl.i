/* -*- c++ -*- */
/*
 * Copyright 2006,2009 Free Software Foundation, Inc.
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

%include "gnuradio.i"				// the common stuff

//load generated python docstrings
%include "video_sdl_swig_doc.i"

%{
#include "video_sdl_sink_uc.h"
#include "video_sdl_sink_s.h"
%}

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(video_sdl,sink_uc)

video_sdl_sink_uc_sptr
video_sdl_make_sink_uc ( double framerate,int width=640, int height=480,unsigned int format=0,int dst_width=-1,int dst_height=-1
		     ) throw (std::runtime_error);


class video_sdl_sink_uc : public gr_sync_block {
 protected:
  video_sdl_sink_uc (double framerate,int width, int height,gr_uint32 format,int dst_width,int dst_height);

 public:
  ~video_sdl_sink_uc ();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(video_sdl,sink_s)

video_sdl_sink_s_sptr
video_sdl_make_sink_s ( double framerate,int width=640, int height=480,unsigned int format=0,int dst_width=-1,int dst_height=-1
		     ) throw (std::runtime_error);


class video_sdl_sink_s : public gr_sync_block {
 protected:
  video_sdl_sink_s (double framerate,int width, int height,gr_uint32 format,int dst_width,int dst_height);

 public:
  ~video_sdl_sink_s ();
};

#if SWIGGUILE
%scheme %{
(load-extension-global "libguile-gnuradio-video_sdl" "scm_init_gnuradio_video_sdl_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif

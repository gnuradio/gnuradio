/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VIDEO_SDL_SINK_S_H
#define INCLUDED_VIDEO_SDL_SINK_S_H

#include <video_sdl_api.h>
#include <gr_sync_block.h>
#include <string>
#include <SDL.h>

/*  fourcc (four character code) */
#define vid_fourcc(a,b,c,d) (((unsigned)(a)<<0) | ((unsigned)(b)<<8) | ((unsigned)(c)<<16) | ((unsigned)(d)<<24))
#define IMGFMT_YV12  vid_fourcc('Y','V','1','2') /* 12  YVU 4:2:0 */

class video_sdl_sink_s;
typedef boost::shared_ptr<video_sdl_sink_s> video_sdl_sink_s_sptr;

VIDEO_SDL_API video_sdl_sink_s_sptr
video_sdl_make_sink_s (double framerate,int width=640, int height=480,unsigned int format=IMGFMT_YV12,int dst_width=-1,int dst_height=-1);

/*!
 * \brief video sink using SDL
 *
 * input signature is one, two or three streams of signed short.
 * One stream: stream is grey (Y)
 * two streems: first is grey (Y), second is alternating U and V
 * Three streams: first is grey (Y), second is U, third is V
 * Input samples must be in the range [0,255].
 */

class VIDEO_SDL_API video_sdl_sink_s : public gr_sync_block {
  friend VIDEO_SDL_API video_sdl_sink_s_sptr
  video_sdl_make_sink_s (double framerate,int width, int height,unsigned int format,int dst_width,int dst_height);

  int		d_chunk_size;

 protected:
  video_sdl_sink_s (double framerate,int width, int height,unsigned int format,
                      int dst_width,int dst_height);
  void copy_line_pixel_interleaved(unsigned char *dst_pixels_u,unsigned char *dst_pixels_v,
                          const short * src_pixels,int src_width);
  void copy_line_line_interleaved(unsigned char *dst_pixels_u,unsigned char *dst_pixels_v,
                          const short * src_pixels,int src_width);
  void copy_line_single_plane(unsigned char *dst_pixels,const short * src_pixels,int src_width);
  void copy_line_single_plane_dec2(unsigned char *dst_pixels,const short * src_pixels,int src_width);
  int copy_plane_to_surface (int plane,int noutput_items,
		      const short * src_pixels);
  float d_framerate;
  int d_wanted_frametime_ms;
  int d_width;
  int d_height;
  int d_dst_width;
  int d_dst_height;
  int d_format;
  int d_current_line;
  SDL_Surface *d_screen;
  SDL_Overlay *d_image;
  SDL_Rect d_dst_rect;
  float d_avg_delay;
  unsigned int d_wanted_ticks;


 public:
  ~video_sdl_sink_s ();
  
  int work (int noutput_items,
		  gr_vector_const_void_star &input_items,
		  gr_vector_void_star &output_items);
};

#endif /* INCLUDED_VIDEO_SDL_SINK_S_H */

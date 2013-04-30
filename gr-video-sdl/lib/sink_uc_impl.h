/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VIDEO_SDL_SINK_UC_IMPL_H
#define INCLUDED_VIDEO_SDL_SINK_UC_IMPL_H

#include <gnuradio/video_sdl/sink_uc.h>
#include <SDL.h>

namespace gr {
  namespace video_sdl {

/*  fourcc (four character code) */
#define vid_fourcc(a,b,c,d) (((unsigned)(a)<<0) | ((unsigned)(b)<<8) | ((unsigned)(c)<<16) | ((unsigned)(d)<<24))
#define IMGFMT_YV12  vid_fourcc('Y','V','1','2') /* 12  YVU 4:2:0 */

    class sink_uc_impl : public sink_uc
    {
    private:
      int d_chunk_size;

    protected:
      void copy_line_pixel_interleaved(unsigned char *dst_pixels_u,unsigned char *dst_pixels_v,
				       const unsigned char * src_pixels,int src_width);
      void copy_line_line_interleaved(unsigned char *dst_pixels_u,unsigned char *dst_pixels_v,
				      const unsigned char * src_pixels,int src_width);
      void copy_line_single_plane(unsigned char *dst_pixels,const unsigned char * src_pixels,int src_width);
      void copy_line_single_plane_dec2(unsigned char *dst_pixels,const unsigned char * src_pixels,int src_width);
      int copy_plane_to_surface(int plane,int noutput_items,
				const unsigned char * src_pixels);

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
      sink_uc_impl(double framerate,
		   int width, int height,
		   unsigned int format,
		   int dst_width, int dst_height);
      ~sink_uc_impl();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace video_sdl */
} /* namespace gr */

#endif /* INCLUDED_VIDEO_SDL_SINK_UC_IMPL_H */

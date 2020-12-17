/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VIDEO_SDL_SINK_S_IMPL_H
#define INCLUDED_VIDEO_SDL_SINK_S_IMPL_H

#include <gnuradio/video_sdl/sink_s.h>
#include <SDL.h>

namespace gr {
namespace video_sdl {

/*  fourcc (four character code) */
#define vid_fourcc(a, b, c, d)                                             \
    (((unsigned)(a) << 0) | ((unsigned)(b) << 8) | ((unsigned)(c) << 16) | \
     ((unsigned)(d) << 24))
#define IMGFMT_YV12 vid_fourcc('Y', 'V', '1', '2') /* 12  YVU 4:2:0 */

class sink_s_impl : public sink_s
{
private:
    int d_chunk_size;

protected:
    void copy_line_pixel_interleaved(unsigned char* dst_pixels_u,
                                     unsigned char* dst_pixels_v,
                                     const short* src_pixels,
                                     int src_width);
    void copy_line_line_interleaved(unsigned char* dst_pixels_u,
                                    unsigned char* dst_pixels_v,
                                    const short* src_pixels,
                                    int src_width);
    void copy_line_single_plane(unsigned char* dst_pixels,
                                const short* src_pixels,
                                int src_width);
    void copy_line_single_plane_dec2(unsigned char* dst_pixels,
                                     const short* src_pixels,
                                     int src_width);
    int copy_plane_to_surface(int plane, int noutput_items, const short* src_pixels);

    float d_framerate;
    int d_wanted_frametime_ms;
    int d_width;
    int d_height;
    int d_dst_width;
    int d_dst_height;
    int d_format;
    int d_current_line;
    SDL_Surface* d_screen;
    SDL_Overlay* d_image;
    SDL_Rect d_dst_rect;
    float d_avg_delay;
    unsigned int d_wanted_ticks;

public:
    sink_s_impl(double framerate,
                int width,
                int height,
                unsigned int format,
                int dst_width,
                int dst_height);
    ~sink_s_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace video_sdl */
} /* namespace gr */

#endif /* INCLUDED_VIDEO_SDL_SINK_S_IMPL_H */

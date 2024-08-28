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

int render_loop_s(void* data);

class sink_s_impl : public sink_s
{
private:
    int d_chunk_size;
    thread::thread d_render_thread;

protected:
    float d_framerate;
    int d_wanted_frametime_ms;
    int d_width;
    int d_height;
    int d_dst_width;
    int d_dst_height;
    int d_current_line;
    float d_avg_delay;
    unsigned int d_wanted_ticks;
    bool d_quit_requested;

    template <typename F>
    int copy_planes_to_buffers(F copy_func,
                               const short* src_pixels_0,
                               const short* src_pixels_1 = NULL,
                               const short* src_pixels_2 = NULL);

    std::vector<unsigned char> d_buf_y, d_buf_u, d_buf_v;

    std::atomic_bool d_frame_pending;
    SDL_Overlay* d_image;

public:
    sink_s_impl(double framerate, int width, int height, int dst_width, int dst_height);
    ~sink_s_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    friend int render_loop_s(void* data);
};

} /* namespace video_sdl */
} /* namespace gr */

#endif /* INCLUDED_VIDEO_SDL_SINK_S_IMPL_H */

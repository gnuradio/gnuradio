/* -*- c++ -*- */
/*
 * Copyright 2006,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sink_uc_impl.h"
#include <gnuradio/io_signature.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace gr {
namespace video_sdl {

sink_uc::sptr
sink_uc::make(double framerate, int width, int height, int dst_width, int dst_height)
{
    return gnuradio::make_block_sptr<sink_uc_impl>(
        framerate, width, height, dst_width, dst_height);
}

sink_uc_impl::sink_uc_impl(
    double framerate, int width, int height, int dst_width, int dst_height)
    : sync_block("video_sdl_sink_uc",
                 io_signature::make(1, 3, sizeof(unsigned char)),
                 io_signature::make(0, 0, 0)),
      d_chunk_size(width * height),
      d_render_thread(&render_loop_uc, this),
      d_framerate(framerate),
      d_wanted_frametime_ms(0),
      d_width(width),
      d_height(height),
      d_dst_width(dst_width),
      d_dst_height(dst_height),
      d_current_line(0),
      d_avg_delay(0.0),
      d_wanted_ticks(0),
      d_quit_requested(false),
      // clear the surface to grey
      d_buf_y(width * height, 128),
      d_buf_u(width * height / 4, 128),
      d_buf_v(width * height / 4, 128),
      d_frame_pending(false),
      d_image(NULL)
{
    if (framerate <= 0.0)
        d_wanted_frametime_ms = 0; // Go as fast as possible
    else
        d_wanted_frametime_ms = (int)(1000.0 / framerate);

    if (dst_width < 0)
        d_dst_width = d_width;
    if (dst_height < 0)
        d_dst_height = d_height;

    d_chunk_size = std::min(1, 16384 / width); // width*16;
    d_chunk_size = d_chunk_size * width;
    // d_chunk_size = (int)(width);
    set_output_multiple(d_chunk_size);
}

sink_uc_impl::~sink_uc_impl()
{
    SDL_Event quit_event{ SDL_QUIT };
    SDL_PushEvent(&quit_event);
    d_render_thread.detach();
}

template <typename F>
int sink_uc_impl::copy_planes_to_buffers(F copy_func,
                                         const unsigned char* src_pixels_0,
                                         const unsigned char* src_pixels_1,
                                         const unsigned char* src_pixels_2)
{
    int noutput_items_produced = 0;
    auto dst_y = &d_buf_y[d_current_line * d_width],
         dst_u = &d_buf_u[(d_current_line / 2) * (d_width / 2)],
         dst_v = &d_buf_v[(d_current_line / 2) * (d_width / 2)];
    for (int i = 0; i < d_chunk_size; i += d_width) {
        copy_func(src_pixels_0, src_pixels_1, src_pixels_2, dst_y, dst_u, dst_v);
        if (src_pixels_0)
            src_pixels_0 += d_width;
        if (src_pixels_1)
            src_pixels_1 += d_width;
        if (src_pixels_2)
            src_pixels_2 += d_width;
        dst_y += d_width;
        dst_u += d_width / 2;
        dst_v += d_width / 2;
        noutput_items_produced += d_width;
        d_current_line++;
        if (d_current_line >= d_height) {
            d_current_line = 0;
            dst_y = d_buf_y.data();
            dst_u = d_buf_u.data();
            dst_v = d_buf_v.data();

            if (d_image && !d_frame_pending.load()) {
                if (SDL_LockYUVOverlay(d_image) < 0) {
                    continue;
                }
                for (int i = 0; i < d_height; i++) {
                    memcpy(&d_image->pixels[0][i * d_image->pitches[0]],
                           &dst_y[i * d_width],
                           d_width);
                }
                for (int i = 0; i < d_height / 2; i++) {
                    memcpy(&d_image->pixels[1][i * d_image->pitches[1]],
                           &dst_u[i * d_width / 2],
                           d_width / 2);
                    memcpy(&d_image->pixels[2][i * d_image->pitches[2]],
                           &dst_v[i * d_width / 2],
                           d_width / 2);
                }
                SDL_UnlockYUVOverlay(d_image);
                d_frame_pending.store(true);
            }

            unsigned int ticks = SDL_GetTicks(); // milliseconds
            d_wanted_ticks += d_wanted_frametime_ms;
            constexpr float avg_alpha = 0.1;
            int time_diff = d_wanted_ticks - ticks;
            d_avg_delay = time_diff * avg_alpha + d_avg_delay * (1.0 - avg_alpha);
        }
    }
    return noutput_items_produced;
}

int sink_uc_impl::work(int noutput_items,
                       gr_vector_const_void_star& input_items,
                       gr_vector_void_star& output_items)
{
    if (d_quit_requested)
        return WORK_DONE;

    const unsigned char *src_pixels_0, *src_pixels_1, *src_pixels_2;
    int noutput_items_produced = 0;

    switch (input_items.size()) {
    case 3: // first channel=Y, second channel is  U , third channel is V
        src_pixels_0 = (const unsigned char*)input_items[0];
        src_pixels_1 = (const unsigned char*)input_items[1];
        src_pixels_2 = (const unsigned char*)input_items[2];
        for (int i = 0; i < noutput_items; i += d_chunk_size) {
            noutput_items_produced += copy_planes_to_buffers(
                [=](const Uint8* src_y,
                    const Uint8* src_u,
                    const Uint8* src_v,
                    Uint8* dst_y,
                    Uint8* dst_u,
                    Uint8* dst_v) {
                    memcpy(dst_y, src_y, d_width);
                    for (int k = 0; k < d_width; k += 2) {
                        dst_u[k / 2] = src_u[k];
                        dst_v[k / 2] = src_v[k];
                    }
                },
                src_pixels_0,
                src_pixels_1,
                src_pixels_2);
            src_pixels_0 += d_chunk_size;
            src_pixels_1 += d_chunk_size;
            src_pixels_2 += d_chunk_size;
        }
        break;
    case 2: // first channel=Y, second channel is alternating pixels U and V
        src_pixels_0 = (const unsigned char*)input_items[0];
        src_pixels_1 = (const unsigned char*)input_items[1];
        for (int i = 0; i < noutput_items; i += d_chunk_size) {
            noutput_items_produced += copy_planes_to_buffers(
                [=](const Uint8* src_y,
                    const Uint8* src_uv,
                    const Uint8* _sv,
                    Uint8* dst_y,
                    Uint8* dst_u,
                    Uint8* dst_v) {
                    memcpy(dst_y, src_y, d_width);
                    for (int k = 0; k < d_width; k += 2) {
                        dst_u[k / 2] = src_uv[k + 0];
                        dst_v[k / 2] = src_uv[k + 1];
                    }
                },
                src_pixels_0,
                src_pixels_1,
                NULL);
            src_pixels_0 += d_chunk_size;
            src_pixels_1 += d_chunk_size;
        }
        break;
    case 1: // grey (Y) input
        src_pixels_0 = (const unsigned char*)input_items[0];
        for (int i = 0; i < noutput_items; i += d_chunk_size) {
            noutput_items_produced += copy_planes_to_buffers(
                [=](const Uint8* src_y,
                    const Uint8* src_u,
                    const Uint8* src_v,
                    Uint8* dst_y,
                    Uint8* dst_u,
                    Uint8* dst_v) { memcpy(dst_y, src_y, d_width); },
                src_pixels_0,
                NULL,
                NULL);
            src_pixels_0 += d_chunk_size;
        }
        break;
    default: // 0 or more then 3 channels
        d_logger->error("Wrong number of channels: 1, 2 or 3 channels are supported. "
                        "Requested number of channels is {:d}",
                        input_items.size());
        throw std::runtime_error("video_sdl2::sink_uc");
    }

    return noutput_items_produced;
}

int render_loop_uc(void* data)
{
    auto sink = reinterpret_cast<sink_uc_impl*>(data);

    atexit(SDL_Quit);
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        sink->d_logger->error(
            "Couldn't initialize SDL: {:s}; SDL_Init(SDL_INIT_VIDEO) failed",
            SDL_GetError());
        throw std::runtime_error("video_sdl2::sink_uc::render_loop");
    }

    /* accept any depth */
    auto display = SDL_SetVideoMode(sink->d_dst_width,
                                    sink->d_dst_height,
                                    0,
                                    SDL_SWSURFACE | SDL_RESIZABLE | SDL_ANYFORMAT);
    // SDL_DOUBLEBUF |SDL_SWSURFACE| SDL_HWSURFACE||SDL_FULLSCREEN
    if (!display) {
        sink->d_logger->error("Unable to set SDL video mode: {:s}", SDL_GetError());
        throw std::runtime_error("video_sdl2::sink_uc::render_loop");
    }

    if (sink->d_image) {
        SDL_FreeYUVOverlay(sink->d_image);
    }
    sink->d_image =
        SDL_CreateYUVOverlay(sink->d_width, sink->d_height, SDL_IYUV_OVERLAY, display);
    if (!sink->d_image) {
        sink->d_logger->error("Couldn't create a YUV overlay: {:s}", SDL_GetError());
        throw std::runtime_error("video_sdl2::sink_uc::render_loop");
    }

    SDL_Rect dstrect{ 0, 0, (Uint16)sink->d_dst_width, (Uint16)sink->d_dst_height };

    while (!SDL_QuitRequested()) {
        if (sink->d_wanted_ticks == 0)
            sink->d_wanted_ticks = SDL_GetTicks();

        if (sink->d_avg_delay > 0)
            SDL_Delay((unsigned int)sink->d_avg_delay);

        if (!sink->d_frame_pending.load())
            continue;
        SDL_DisplayYUVOverlay(sink->d_image, &dstrect);
        sink->d_frame_pending.store(false);
    }

    sink->d_quit_requested = true;
    SDL_FreeYUVOverlay(sink->d_image);
    SDL_FreeSurface(display);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return 0;
}

} /* namespace video_sdl */
} /* namespace gr */

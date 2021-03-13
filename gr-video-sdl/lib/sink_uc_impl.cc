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
#include <iostream>
#include <stdexcept>


namespace gr {
namespace video_sdl {

sink_uc::sptr sink_uc::make(double framerate,
                            int width,
                            int height,
                            unsigned int format,
                            int dst_width,
                            int dst_height)
{
    return gnuradio::make_block_sptr<sink_uc_impl>(
        framerate, width, height, format, dst_width, dst_height);
}

sink_uc_impl::sink_uc_impl(double framerate,
                           int width,
                           int height,
                           unsigned int format,
                           int dst_width,
                           int dst_height)
    : sync_block("video_sdl_sink_uc",
                 io_signature::make(1, 3, sizeof(unsigned char)),
                 io_signature::make(0, 0, 0)),
      d_chunk_size(width * height),
      d_framerate(framerate),
      d_wanted_frametime_ms(0),
      d_width(width),
      d_height(height),
      d_dst_width(dst_width),
      d_dst_height(dst_height),
      d_format(format),
      d_current_line(0),
      d_screen(NULL),
      d_image(NULL),
      d_avg_delay(0.0),
      d_wanted_ticks(0)
{
    if (framerate <= 0.0)
        d_wanted_frametime_ms = 0; // Go as fast as possible
    else
        d_wanted_frametime_ms = (int)(1000.0 / framerate);

    if (dst_width < 0)
        d_dst_width = d_width;

    if (dst_height < 0)
        d_dst_height = d_height;

    if (0 == format)
        d_format = IMGFMT_YV12;

    atexit(SDL_Quit); // check if this is the way to do this
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::ostringstream msg;
        msg << "Couldn't initialize SDL:" << SDL_GetError()
            << "; SDL_Init(SDL_INIT_VIDEO) failed";
        GR_LOG_ERROR(d_logger, msg.str());
        throw std::runtime_error("video_sdl::sink_uc");
    }

    /* accept any depth */
    d_screen = SDL_SetVideoMode(
        dst_width,
        dst_height,
        0,
        SDL_SWSURFACE | SDL_RESIZABLE |
            SDL_ANYFORMAT); // SDL_DOUBLEBUF |SDL_SWSURFACE| SDL_HWSURFACE||SDL_FULLSCREEN

    if (d_screen == NULL) {
        std::ostringstream msg;
        msg << "Unable to set SDL video mode: " << SDL_GetError()
            << "; SDL_SetVideoMode() Failed";
        GR_LOG_ERROR(d_logger, msg.str());
        exit(1);
    }

    if (d_image) {
        SDL_FreeYUVOverlay(d_image);
    }

    /* Initialize and create the YUV Overlay used for video out */
    if (!(d_image =
              SDL_CreateYUVOverlay(d_width, d_height, SDL_YV12_OVERLAY, d_screen))) {
        std::ostringstream msg;
        msg << "SDL: Couldn't create a YUV overlay: " << SDL_GetError();
        GR_LOG_ERROR(d_logger, msg.str());
        throw std::runtime_error("video_sdl::sink_uc");
    }

    GR_LOG_INFO(d_debug_logger,
                boost::format("SDL screen_mode %d bits-per-pixel") %
                    d_screen->format->BitsPerPixel);
    GR_LOG_INFO(d_debug_logger, boost::format("SDL overlay_mode %i ") % d_image->format);

    d_chunk_size = std::min(1, 16384 / width); // width*16;
    d_chunk_size = d_chunk_size * width;
    // d_chunk_size = (int)(width);
    set_output_multiple(d_chunk_size);

    /* Set the default playback area */
    d_dst_rect.x = 0;
    d_dst_rect.y = 0;
    d_dst_rect.w = d_dst_width;
    d_dst_rect.h = d_dst_height;
    // clear the surface to grey

    if (SDL_LockYUVOverlay(d_image)) {
        std::ostringstream msg;
        msg << "SDL: Couldn't lock a YUV overlay: " << SDL_GetError();
        GR_LOG_ERROR(d_logger, msg.str());
        throw std::runtime_error("video_sdl::sink_uc");
    }

    memset(d_image->pixels[0], 128, d_image->pitches[0] * d_height);
    memset(d_image->pixels[1], 128, d_image->pitches[1] * d_height / 2);
    memset(d_image->pixels[2], 128, d_image->pitches[2] * d_height / 2);
    SDL_UnlockYUVOverlay(d_image);
}

sink_uc_impl::~sink_uc_impl() { SDL_Quit(); }

void sink_uc_impl::copy_line_pixel_interleaved(unsigned char* dst_pixels_u,
                                               unsigned char* dst_pixels_v,
                                               const unsigned char* src_pixels,
                                               int src_width)
{
    for (int i = 0; i < src_width; i++) {
        dst_pixels_u[i] = src_pixels[i * 2];
        dst_pixels_v[i] = src_pixels[i * 2 + 1];
    }
}

void sink_uc_impl::copy_line_line_interleaved(unsigned char* dst_pixels_u,
                                              unsigned char* dst_pixels_v,
                                              const unsigned char* src_pixels,
                                              int src_width)
{
    memcpy(dst_pixels_u, src_pixels, src_width);
    memcpy(dst_pixels_v, src_pixels + src_width, src_width);
}

void sink_uc_impl::copy_line_single_plane(unsigned char* dst_pixels,
                                          const unsigned char* src_pixels,
                                          int src_width)
{
    memcpy(dst_pixels, src_pixels, src_width);
}

void sink_uc_impl::copy_line_single_plane_dec2(unsigned char* dst_pixels,
                                               const unsigned char* src_pixels,
                                               int src_width)
{
    for (int i = 0, j = 0; i < src_width; i += 2, j++) {
        dst_pixels[j] = (unsigned char)src_pixels[i];
    }
}

int sink_uc_impl::copy_plane_to_surface(int plane,
                                        int noutput_items,
                                        const unsigned char* src_pixels)
{
    const int first_dst_plane = (12 == plane || 1122 == plane) ? 1 : plane;
    const int second_dst_plane = (12 == plane || 1122 == plane) ? 2 : plane;
    int current_line = (0 == plane) ? d_current_line : d_current_line / 2;

    unsigned char* dst_pixels = (unsigned char*)d_image->pixels[first_dst_plane];
    dst_pixels = &dst_pixels[current_line * d_image->pitches[first_dst_plane]];

    unsigned char* dst_pixels_2 = (unsigned char*)d_image->pixels[second_dst_plane];
    dst_pixels_2 = &dst_pixels_2[current_line * d_image->pitches[second_dst_plane]];

    int src_width = d_width;
    int noutput_items_produced = 0;
    int max_height = (0 == plane) ? d_height - 1 : d_height / 2 - 1;

    for (int i = 0; i < noutput_items; i += src_width) {
        // output one line at a time
        if (12 == plane) {
            copy_line_pixel_interleaved(dst_pixels, dst_pixels_2, src_pixels, src_width);
            dst_pixels_2 += d_image->pitches[second_dst_plane];
        } else if (1122 == plane) {
            copy_line_line_interleaved(dst_pixels, dst_pixels_2, src_pixels, src_width);
            dst_pixels_2 += d_image->pitches[second_dst_plane];
            src_pixels += src_width;
        } else if (0 == plane)
            copy_line_single_plane(dst_pixels, src_pixels, src_width);
        else /* 1==plane || 2==plane*/
            copy_line_single_plane_dec2(
                dst_pixels, src_pixels, src_width); // decimate by two horizontally

        src_pixels += src_width;
        dst_pixels += d_image->pitches[first_dst_plane];
        noutput_items_produced += src_width;
        current_line++;

        if (current_line > max_height) {
            // Start new frame
            // TODO, do this all in a separate thread
            current_line = 0;
            dst_pixels = d_image->pixels[first_dst_plane];
            dst_pixels_2 = d_image->pixels[second_dst_plane];
            if (0 == plane) {
                SDL_DisplayYUVOverlay(d_image, &d_dst_rect);
                // SDL_Flip(d_screen);
                unsigned int ticks = SDL_GetTicks(); // milliseconds
                d_wanted_ticks += d_wanted_frametime_ms;
                float avg_alpha = 0.1;
                int time_diff = d_wanted_ticks - ticks;
                d_avg_delay = time_diff * avg_alpha + d_avg_delay * (1.0 - avg_alpha);
            }
        }
    }

    if (0 == plane)
        d_current_line = current_line;

    return noutput_items_produced;
}

int sink_uc_impl::work(int noutput_items,
                       gr_vector_const_void_star& input_items,
                       gr_vector_void_star& output_items)
{
    unsigned char *src_pixels_0, *src_pixels_1, *src_pixels_2;
    int noutput_items_produced = 0;
    int plane;
    int delay = (int)d_avg_delay;

    if (0 == d_wanted_ticks)
        d_wanted_ticks = SDL_GetTicks();

    if (delay > 0)
        SDL_Delay((unsigned int)delay); // compensate if running too fast

    if (SDL_LockYUVOverlay(d_image)) {
        return 0;
    }

    switch (input_items.size()) {
    case 3: // first channel=Y, second channel is  U , third channel is V
        src_pixels_0 = (unsigned char*)input_items[0];
        src_pixels_1 = (unsigned char*)input_items[1];
        src_pixels_2 = (unsigned char*)input_items[2];
        for (int i = 0; i < noutput_items; i += d_chunk_size) {
            copy_plane_to_surface(1, d_chunk_size, src_pixels_1);
            copy_plane_to_surface(2, d_chunk_size, src_pixels_2);
            noutput_items_produced +=
                copy_plane_to_surface(0, d_chunk_size, src_pixels_0);
            src_pixels_0 += d_chunk_size;
            src_pixels_1 += d_chunk_size;
            src_pixels_2 += d_chunk_size;
        }
        break;
    case 2:
        if (1) { // if(pixel_interleaved_uv)
            // first channel=Y, second channel is alternating pixels U and V
            src_pixels_0 = (unsigned char*)input_items[0];
            src_pixels_1 = (unsigned char*)input_items[1];
            for (int i = 0; i < noutput_items; i += d_chunk_size) {
                copy_plane_to_surface(12, d_chunk_size / 2, src_pixels_1);
                noutput_items_produced +=
                    copy_plane_to_surface(0, d_chunk_size, src_pixels_0);
                src_pixels_0 += d_chunk_size;
                src_pixels_1 += d_chunk_size;
            }
        } else {
            // first channel=Y, second channel is alternating lines U and V
            src_pixels_0 = (unsigned char*)input_items[0];
            src_pixels_1 = (unsigned char*)input_items[1];
            for (int i = 0; i < noutput_items; i += d_chunk_size) {
                copy_plane_to_surface(1222, d_chunk_size / 2, src_pixels_1);
                noutput_items_produced +=
                    copy_plane_to_surface(0, d_chunk_size, src_pixels_0);
                src_pixels_0 += d_chunk_size;
                src_pixels_1 += d_chunk_size;
            }
        }
        break;
    case 1: // grey (Y) input
        /* Y component */
        plane = 0;
        src_pixels_0 = (unsigned char*)input_items[plane];
        for (int i = 0; i < noutput_items; i += d_chunk_size) {
            noutput_items_produced +=
                copy_plane_to_surface(plane, d_chunk_size, src_pixels_0);
            src_pixels_0 += d_chunk_size;
        }
        break;
    default: // 0 or more then 3 channels
        std::ostringstream msg;
        msg << "Wrong number of channels: 1, 2 or 3 channels are supported. Requested "
               "number of channels is "
            << input_items.size();
        GR_LOG_ERROR(d_logger, msg.str());
        throw std::runtime_error("video_sdl::sink_uc");
    }

    SDL_UnlockYUVOverlay(d_image);
    return noutput_items_produced;
}

} /* namespace video_sdl */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VIDEO_SDL_SINK_UC_H
#define INCLUDED_VIDEO_SDL_SINK_UC_H

#include <gnuradio/sync_block.h>
#include <gnuradio/video_sdl/api.h>

namespace gr {
namespace video_sdl {

/*!
 * \brief video sink using SDL
 *
 * input signature is one, two or three streams of uchar.
 * One stream: stream is grey (Y)
 * two streems: first is grey (Y), second is alternating U and V
 * Three streams: first is grey (Y), second is U, third is V
 * Input samples must be in the range [0,255].
 */
class VIDEO_SDL_API sink_uc : virtual public sync_block
{
public:
    // gr::video_sdl::sink_uc::sptr
    typedef boost::shared_ptr<sink_uc> sptr;

    static sptr make(double framerate,
                     int width,
                     int height,
                     unsigned int format,
                     int dst_width,
                     int dst_height);
};

} /* namespace video_sdl */
} /* namespace gr */

#endif /* INCLUDED_VIDEO_SDL_SINK_UC_H */

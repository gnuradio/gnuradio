/*
 * Copyright 2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VIDEO_SDL_API_H
#define INCLUDED_VIDEO_SDL_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_video_sdl_EXPORTS
#define VIDEO_SDL_API __GR_ATTR_EXPORT
#else
#define VIDEO_SDL_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_VIDEO_SDL_API_H */

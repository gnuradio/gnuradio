/*
 * Copyright 2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_AUDIO_API_H
#define INCLUDED_GR_AUDIO_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_audio_EXPORTS
#define GR_AUDIO_API __GR_ATTR_EXPORT
#else
#define GR_AUDIO_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_GR_AUDIO_API_H */

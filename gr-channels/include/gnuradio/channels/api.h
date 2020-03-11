/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHANNELS_API_H
#define INCLUDED_CHANNELS_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_channels_EXPORTS
#define CHANNELS_API __GR_ATTR_EXPORT
#else
#define CHANNELS_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_CHANNELS_API_H */

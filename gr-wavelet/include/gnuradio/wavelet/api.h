/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_WAVELET_API_H
#define INCLUDED_WAVELET_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_wavelet_EXPORTS
#define WAVELET_API __GR_ATTR_EXPORT
#else
#define WAVELET_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_WAVELET_API_H */

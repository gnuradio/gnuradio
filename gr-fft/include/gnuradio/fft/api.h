/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FFT_API_H
#define INCLUDED_FFT_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_fft_EXPORTS
#define FFT_API __GR_ATTR_EXPORT
#else
#define FFT_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_FFT_API_H */

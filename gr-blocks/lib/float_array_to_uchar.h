/* -*- c++ -*- */
/*
 * Copyright 2002,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FLOAT_ARRAY_TO_UCHAR_H
#define INCLUDED_FLOAT_ARRAY_TO_UCHAR_H

#include <gnuradio/blocks/api.h>

/*!
 * convert array of floats to unsigned chars with rounding and saturation.
 */
BLOCKS_API void float_array_to_uchar(const float* in, unsigned char* out, int nsamples);

#endif /* INCLUDED_FLOAT_ARRAY_TO_UCHAR_H */

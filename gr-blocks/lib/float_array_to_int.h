/* -*- c++ -*- */
/*
 * Copyright 2011-2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_FLOAT_ARRAY_TO_INT_H
#define INCLUDED_BLOCKS_FLOAT_ARRAY_TO_INT_H

#include <gnuradio/blocks/api.h>

/*!
 * convert array of floats to int with rounding and saturation.
 */
BLOCKS_API void float_array_to_int(const float* in, int* out, float scale, int nsamples);

#endif /* INCLUDED_BLOCKS_FLOAT_ARRAY_TO_INT_H */

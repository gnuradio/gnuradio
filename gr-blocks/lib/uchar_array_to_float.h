/* -*- c++ -*- */
/*
 * Copyright 2005, 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_UCHAR_ARRAY_TO_FLOAT_H
#define INCLUDED_UCHAR_ARRAY_TO_FLOAT_H

#include <gnuradio/blocks/api.h>

/*
 * convert array of unsigned chars to floats
 */
BLOCKS_API void uchar_array_to_float(const unsigned char* in, float* out, int nsamples);


#endif /* INCLUDED_UCHAR_ARRAY_TO_FLOAT_H */

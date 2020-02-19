/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_INTERLEAVED_SHORT_ARRAY_TO_COMPLEX_H
#define INCLUDED_INTERLEAVED_SHORT_ARRAY_TO_COMPLEX_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/gr_complex.h>

/*
 * convert array of interleaved shorts to complex.
 * the shorts contains real, imaginary, real, imaginary...
 * nsamples is the number of shorts; it must be even.
 */
BLOCKS_API void
interleaved_short_array_to_complex(const short* in, gr_complex* out, int nsamples);

#endif /* INCLUDED_INTERLEAVED_SHORT_ARRAY_TO_COMPLEX_H */

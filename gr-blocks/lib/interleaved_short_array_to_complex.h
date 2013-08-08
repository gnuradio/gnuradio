/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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
BLOCKS_API void interleaved_short_array_to_complex (const short *in, gr_complex *out, int nsamples);

#endif /* INCLUDED_INTERLEAVED_SHORT_ARRAY_TO_COMPLEX_H */

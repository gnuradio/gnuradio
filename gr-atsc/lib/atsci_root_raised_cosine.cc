/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#include <gnuradio/atsc/consts.h>
#include <gnuradio/atsc/root_raised_cosine_impl.h>
#include <gnuradio/filter/firdes.h>

vector<float>
atsc_root_raised_cosine::taps (double sampling_rate)
{
  static const double symbol_rate = ATSC_SYMBOL_RATE/2;	// 1/2 as wide because we're designing lp filter
  // static const int    NTAPS = 93;			// good number
  // static const int    NTAPS = 745;			// better number
  static const int    NTAPS = 279;			// better number

  return gr::filter::firdes::root_raised_cosine(1.0, sampling_rate, symbol_rate, .115, NTAPS);
}

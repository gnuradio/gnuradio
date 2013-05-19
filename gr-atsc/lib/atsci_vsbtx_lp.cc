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
#include <gnuradio/atsc/vsbtx_lp_impl.h>
#include <stdexcept>
#include <cmath>
#include <iostream>

using std::vector;
using std::cerr;
using std::endl;

// atsc root raised cosine filter, sampling rate = 21.52 MHz
static const float atsc_vsbtx_lp2x[] = {
#include "atsc_vsbtx_lp.dat"
};

#define NELEM(x) (sizeof (x) / sizeof ((x)[0]))

// is a within 5% of target?

static bool
close_enough_p (double a, double target)
{
  double delta = fabs (target * 0.05);	//  5 percent

  return fabs (target - a) <= delta;
}

vector<float>
atsc_vsbtx_lp::taps (double sampling_freq)
{
  if (close_enough_p (sampling_freq, 2 * ATSC_SYMBOL_RATE)){
    return vector<float>(&atsc_vsbtx_lp2x[0], &atsc_vsbtx_lp2x[NELEM(atsc_vsbtx_lp2x)]);
  }
  else
    throw std::out_of_range (
     "atsc_vsbtx_lp: no pre-designed filter close enough");
}

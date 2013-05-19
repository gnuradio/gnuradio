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

#include <gnuradio/atsc/root_raised_cosine_bandpass_impl.h>
#include <iostream>
#include <cmath>

using std::vector;
using std::cerr;
using std::endl;


vector<float>
atsc_root_raised_cosine_bandpass::taps (double sampling_freq)
{

  vector<float> t = atsc_root_raised_cosine::taps (sampling_freq);

  cerr << "atsc_root_raised_cosine_bandpass::taps -- " << t.size () << endl;

  // heterodyne the low pass coefficients up to the specified bandpass
  // center frequency.  Note that when we do this, the filter bandwidth
  // is effectively twice the low pass (2.69 * 2 = 5.38) and hence
  // matches the diagram in the ATSC spec.

  double arg = 2 * M_PI * _center_freq / sampling_freq;
  for (unsigned int i = 0; i < t.size (); i++)
    // the factor of 2 keeps the gain of the passband normalized to 1
    t[i] *= 2 * cos (arg * (double) i);

  return t;
}

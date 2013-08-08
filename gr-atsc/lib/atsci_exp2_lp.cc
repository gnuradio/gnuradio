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

#include <atsc_consts.h>
#include <gnuradio/atsc/exp2_lp_impl.h>
#include <stdexcept>
#include <cmath>
#include <iostream>

using std::vector;
using std::cerr;
using std::endl;

/*
 * FILTER SPECIFICATION FILE
 * FILTER TYPE:LOW PASS                       1H
 * PASSBAND RIPPLE IN -dB            -.0100
 * STOPBAND RIPPLE IN -dB          -66.0000
 * PASSBAND CUTOFF FREQUENCY    5.69000     HERTZ
 * STOPBAND CUTOFF FREQUENCY    6.12000     HERTZ
 * SAMPLING FREQUENCY           21.5200     HERTZ
 */
static const float atsci_exp2_lp2x[] = {
#include "atsci_exp2_lp2x.dat"
};

/*
 * FILTER SPECIFICATION FILE
 * FILTER TYPE:LOW PASS                       1H
 * PASSBAND RIPPLE IN -dB            -.0100
 * STOPBAND RIPPLE IN -dB          -66.0000
 * PASSBAND CUTOFF FREQUENCY    5.69000     HERTZ
 * STOPBAND CUTOFF FREQUENCY    6.12000     HERTZ
 * SAMPLING FREQUENCY           20.0000     HERTZ
 */
static const float atsci_exp2_lp20[] = {
#include "atsci_exp2_lp20.dat"
};


#define NELEM(x) (sizeof (x) / sizeof ((x)[0]))

// is A within 5% of TARGET?

static bool
close_enough_p (double a, double target)
{
  double delta = fabs (target * 0.05);	//  5 percent

  return fabs (target - a) <= delta;
}

vector<float>
atsci_exp2_lp::taps (double sampling_freq)
{
  if (close_enough_p (sampling_freq, 20e6)){
    return vector<float>(&atsci_exp2_lp20[0], &atsci_exp2_lp20[NELEM(atsci_exp2_lp20)]);
  }
  if (close_enough_p (sampling_freq, 2 * ATSC_SYMBOL_RATE)){
    return vector<float>(&atsci_exp2_lp2x[0], &atsci_exp2_lp2x[NELEM(atsci_exp2_lp2x)]);
  }
  else
    throw std::out_of_range (
     "atsci_exp2_lp: no pre-designed filter close enough");
}

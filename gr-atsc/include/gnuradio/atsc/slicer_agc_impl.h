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

#ifndef _ATSC_SLICER_AGC_H_
#define _ATSC_SLICER_AGC_H_

#include <gnuradio/atsc/api.h>
#include <math.h>
#include <gnuradio/filter/single_pole_iir.h>

/*!
 * \brief Automatic Gain Control class for atsc slicer
 *
 * Given perfect data, output values will be +/- {7, 5, 3, 1}
 */

class ATSC_API atsci_slicer_agc {

 public:
  atsci_slicer_agc () : _gain(1), dc(0.0025) {};


  float gain () { return _gain; }

#if 1
  float scale (float input){
    float t = input * _gain;
    float output = t - REFERENCE;
    float error = REFERENCE - dc.filter (t);
    _gain += error * RATE;
    return output;
  }
#else
  float scale(float input){
    float avg = dc.filter(input);
    if(fabs(avg)<.1)avg=.1;
    _gain += _gain*.99 + .01* REFERENCE/avg;
    return input*_gain - REFERENCE;
  }
#endif

 protected:

  static const float			REFERENCE = 1.25;	// pilot reference value
  static const float 			RATE = 1.0e-5;		// adjustment rate
  float					_gain;			// current gain
  gr::filter::single_pole_iir<float,float,float>	dc;
};

#endif /* _ATSC_SLICER_AGC_H_ */

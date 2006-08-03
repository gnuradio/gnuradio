/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _GRI_AGC_CC_H_
#define _GRI_AGC_CC_H_

#include <math.h>

/*!
 * \brief high performance Automatic Gain Control class
 *
 * For Power the absolute value of the complex number is used.
 */

class gri_agc_cc {

 public:
  gri_agc_cc (float rate = 1e-4, float reference = 1.0, float gain = 1.0, float max_gain = 0.0)
    : _rate(rate), _reference(reference), _gain(gain), _max_gain(max_gain) {};

  float rate () const      { return _rate; }
  float reference () const { return _reference; }
  float gain () const 	   { return _gain;  }
  float max_gain() const   { return _max_gain; }

  void set_rate (float rate) { _rate = rate; }
  void set_reference (float reference) { _reference = reference; }
  void set_gain (float gain) { _gain = gain; }
  void set_max_gain(float max_gain) { _max_gain = max_gain; }
  
  gr_complex scale (gr_complex input){
    gr_complex output = input * _gain;
    _gain += (_reference - sqrt(output.real()*output.real()+output.imag()*output.imag())) * _rate; //use abs or cabs to get approximation by absolute value, 
                                                     //note that abs is computationally more intensive then norm for a complex number
    if (_max_gain > 0.0 && _gain > _max_gain)
       _gain = _max_gain;						     
    return output;
  }

  void scaleN (gr_complex output[], const gr_complex input[], unsigned n){
    for (unsigned i = 0; i < n; i++)
      output[i] = scale (input[i]);
  }
  
 protected:
  float _rate;			// adjustment rate
  float	_reference;		// reference value
  float	_gain;			// current gain
  float _max_gain;		// max allowable gain
};

#endif /* _GRI_AGC_CC_H_ */

/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#ifndef _GRI_AGC2_CC_H_
#define _GRI_AGC2_CC_H_

#include <gr_core_api.h>
#include <math.h>

/*!
 * \brief high performance Automatic Gain Control class
 *
 * For Power the absolute value of the complex number is used.
 */
class GR_CORE_API gri_agc2_cc {

 public:
  gri_agc2_cc (float attack_rate = 1e-1, float decay_rate = 1e-2, float reference = 1.0, 
	       float gain = 1.0, float max_gain = 0.0)
    : _attack_rate(attack_rate), _decay_rate(decay_rate), _reference(reference),
      _gain(gain), _max_gain(max_gain) {};

  float decay_rate () const  { return _decay_rate; }
  float attack_rate () const { return _attack_rate; }
  float reference () const   { return _reference; }
  float gain () const 	     { return _gain;  }
  float max_gain() const     { return _max_gain; }

  void set_decay_rate (float rate) { _decay_rate = rate; }
  void set_attack_rate (float rate) { _attack_rate = rate; }
  void set_reference (float reference) { _reference = reference; }
  void set_gain (float gain) { _gain = gain; }
  void set_max_gain(float max_gain) { _max_gain = max_gain; }

  gr_complex scale (gr_complex input){
    gr_complex output = input * _gain;
    
    float tmp = -_reference + sqrt(output.real()*output.real() + 
				   output.imag()*output.imag());
    float rate = _decay_rate;
    if((tmp) > _gain)
    	rate = _attack_rate;
    _gain -= tmp*rate;
    
#if 0
    fprintf(stdout, "rate = %f\ttmp = %f\t gain = %f\n", rate, tmp, _gain);
#endif

    // Not sure about this; will blow up if _gain < 0 (happens when rates are too high),
    // but is this the solution?
    if (_gain < 0.0)
	_gain = 10e-5;

    if (_max_gain > 0.0 && _gain > _max_gain)
       _gain = _max_gain;						     
    return output;
  }

  void scaleN (gr_complex output[], const gr_complex input[], unsigned n){
    for (unsigned i = 0; i < n; i++)
      output[i] = scale (input[i]);
  }
  
 protected:
  float _attack_rate;		// attack rate for fast changing signals
  float _decay_rate;		// decay rate for slow changing signals
  float	_reference;		// reference value
  float	_gain;			// current gain
  float _max_gain;		// max allowable gain
};

#endif /* _GRI_AGC2_CC_H_ */

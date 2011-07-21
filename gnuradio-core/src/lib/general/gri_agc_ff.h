/* -*- c++ -*- */
/*
 * Copyright 2002,2006 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GRI_AGC_FF_H
#define INCLUDED_GRI_AGC_FF_H

#include <gr_core_api.h>
#include <math.h>

/*!
 * \brief high performance Automatic Gain Control class
 *
 * Power is approximated by absolute value
 */
class GR_CORE_API gri_agc_ff {

 public:
  gri_agc_ff (float rate = 1e-4, float reference = 1.0,
	      float gain = 1.0, float max_gain = 0.0)
    : _rate(rate), _reference(reference), _gain(gain), _max_gain(max_gain) {};

  float rate () const      { return _rate; }
  float reference () const { return _reference; }
  float gain () const 	   { return _gain;  }
  float max_gain () const  { return _max_gain; }
  
  void set_rate (float rate) { _rate = rate; }
  void set_reference (float reference) { _reference = reference; }
  void set_gain (float gain) { _gain = gain; }
  void set_max_gain (float max_gain) { _max_gain = max_gain; }
  
  float scale (float input){
    float output = input * _gain;
    _gain += (_reference - fabsf (output)) * _rate;
    if (_max_gain > 0.0 && _gain > _max_gain)
      _gain = _max_gain;
    return output;
  }

  void scaleN (float output[], const float input[], unsigned n){
    for (unsigned i = 0; i < n; i++)
      output[i] = scale (input[i]);
  }
  
 protected:
  float _rate;			// adjustment rate
  float	_reference;		// reference value
  float	_gain;			// current gain
  float _max_gain;		// maximum gain
};

#endif /* INCLUDED_GRI_AGC_FF_H */


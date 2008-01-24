/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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
#ifndef _GR_VCO_H_
#define _GR_VCO_H_


#include <vector>
#include <gr_sincos.h>
#include <cmath>
#include <gr_complex.h>

/*!
 * \brief base class template for Voltage Controlled Oscillator (VCO)
 * \ingroup misc
 */

//FIXME  Eventually generalize this to fixed point

template<class o_type, class i_type> 
class gr_vco {
public:
  gr_vco () : d_phase (0) {}

  virtual ~gr_vco () {}

  // radians
  void set_phase (double angle) {
    d_phase = angle;
  }

  void adjust_phase (double delta_phase) {
    d_phase += delta_phase;
    if (fabs (d_phase) > M_PI){
      
      while (d_phase > M_PI)
	d_phase -= 2*M_PI;

      while (d_phase < -M_PI)
	d_phase += 2*M_PI;
    }
  }

  double get_phase () const { return d_phase; }

  // compute sin and cos for current phase angle
  void sincos (float *sinx, float *cosx) const;

  // compute cos or sin for current phase angle
  float cos () const { return std::cos (d_phase); }
  float sin () const { return std::sin (d_phase); }

  // compute a block at a time
  void cos (float *output, const float *input, int noutput_items, double k, double ampl = 1.0);

protected:
  double d_phase;
};

template<class o_type, class i_type> 
void
gr_vco<o_type,i_type>::sincos (float *sinx, float *cosx) const
{
  gr_sincosf (d_phase, sinx, cosx);
}

template<class o_type, class i_type> 
void
gr_vco<o_type,i_type>::cos (float *output, const float *input, int noutput_items, double k, double ampl)
{
  for (int i = 0; i < noutput_items; i++){
    output[i] = cos() * ampl;
    adjust_phase(input[i] * k);
  }
}
#endif /* _GR_VCO_H_ */

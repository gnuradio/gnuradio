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
#ifndef _GR_NCO_H_
#define _GR_NCO_H_


#include <vector>
#include <gr_sincos.h>
#include <cmath>
#include <gr_complex.h>

/*!
 * \brief base class template for Numerically Controlled Oscillator (NCO)
 * \ingroup misc
 */


//FIXME  Eventually generalize this to fixed point

template<class o_type, class i_type> 
class gr_nco {
public:
  gr_nco () : phase (0), phase_inc(0) {}

  virtual ~gr_nco () {}

  // radians
  void set_phase (double angle) {
    phase = angle;
  }

  void adjust_phase (double delta_phase) {
    phase += delta_phase;
  }


  // angle_rate is in radians / step
  void set_freq (double angle_rate){
    phase_inc = angle_rate;
  }

  // angle_rate is a delta in radians / step
  void adjust_freq (double delta_angle_rate)
  {
    phase_inc += delta_angle_rate;
  }

  // increment current phase angle

  void step () 
  { 
    phase += phase_inc; 
    if (fabs (phase) > M_PI){
      
      while (phase > M_PI)
	phase -= 2*M_PI;

      while (phase < -M_PI)
	phase += 2*M_PI;
    }
  }

  void step (int n)
  {
    phase += phase_inc * n;
    if (fabs (phase) > M_PI){
      
      while (phase > M_PI)
	phase -= 2*M_PI;

      while (phase < -M_PI)
	phase += 2*M_PI;
    }
  }

  // units are radians / step
  double get_phase () const { return phase; }
  double get_freq () const { return phase_inc; }

  // compute sin and cos for current phase angle
  void sincos (float *sinx, float *cosx) const;

  // compute cos or sin for current phase angle
  float cos () const { return std::cos (phase); }
  float sin () const { return std::sin (phase); }

  // compute a block at a time
  void sin (float *output, int noutput_items, double ampl = 1.0);
  void cos (float *output, int noutput_items, double ampl = 1.0);
  void sincos (gr_complex *output, int noutput_items, double ampl = 1.0);
  void sin (short *output, int noutput_items, double ampl = 1.0);
  void cos (short *output, int noutput_items, double ampl = 1.0);
  void sin (int *output, int noutput_items, double ampl = 1.0);
  void cos (int *output, int noutput_items, double ampl = 1.0);

protected:
  double phase;
  double phase_inc;
};

template<class o_type, class i_type> 
void
gr_nco<o_type,i_type>::sincos (float *sinx, float *cosx) const
{
  gr_sincosf (phase, sinx, cosx);
}

template<class o_type, class i_type> 
void
gr_nco<o_type,i_type>::sin (float *output, int noutput_items, double ampl)
{
  for (int i = 0; i < noutput_items; i++){
    output[i] = (float)(sin () * ampl);
    step ();
  }
}

template<class o_type, class i_type> 
void
gr_nco<o_type,i_type>::cos (float *output, int noutput_items, double ampl)
{
  for (int i = 0; i < noutput_items; i++){
    output[i] = (float)(cos () * ampl);
    step ();
  }
}

template<class o_type, class i_type> 
void
gr_nco<o_type,i_type>::sin (short *output, int noutput_items, double ampl)
{
  for (int i = 0; i < noutput_items; i++){
    output[i] = (short)(sin() * ampl);
    step ();
  }
}

template<class o_type, class i_type> 
void
gr_nco<o_type,i_type>::cos (short *output, int noutput_items, double ampl)
{
  for (int i = 0; i < noutput_items; i++){
    output[i] = (short)(cos () * ampl);
    step ();
  }
}

template<class o_type, class i_type> 
void
gr_nco<o_type,i_type>::sin (int *output, int noutput_items, double ampl)
{
  for (int i = 0; i < noutput_items; i++){
    output[i] = (int)(sin () * ampl);
    step ();
  }
}

template<class o_type, class i_type> 
void
gr_nco<o_type,i_type>::cos (int *output, int noutput_items, double ampl)
{
  for (int i = 0; i < noutput_items; i++){
    output[i] = (int)(cos () * ampl);
    step ();
  }
}

template<class o_type, class i_type> 
void
gr_nco<o_type,i_type>::sincos (gr_complex *output, int noutput_items, double ampl)
{
  for (int i = 0; i < noutput_items; i++){
    float cosx, sinx;
    sincos (&sinx, &cosx);
    output[i] = gr_complex(cosx * ampl, sinx * ampl);
    step ();
  }
}
#endif /* _NCO_H_ */

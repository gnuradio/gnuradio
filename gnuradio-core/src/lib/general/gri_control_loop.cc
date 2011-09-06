/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gri_control_loop.h>
#include <gr_math.h>
#include <stdexcept>

#define M_TWOPI (2.0f*M_PI)

gri_control_loop::gri_control_loop(float loop_bw,
				   float max_freq, float min_freq)
  : d_phase(0), d_freq(0), d_max_freq(max_freq), d_min_freq(min_freq)
{
  // Set the damping factor for a critically damped system
  d_damping = sqrtf(2.0f)/2.0f;

  // Set the bandwidth, which will then call update_gains()
  set_loop_bandwidth(loop_bw);
}

gri_control_loop::~gri_control_loop()
{
}

void
gri_control_loop::update_gains()
{
  float denom = (1.0 + 2.0*d_damping*d_loop_bw + d_loop_bw*d_loop_bw);
  d_alpha = (4*d_damping*d_loop_bw) / denom;
  d_beta = (4*d_loop_bw*d_loop_bw) / denom;
}

void
gri_control_loop::advance_loop(float error)
{
  d_freq = d_freq + d_beta * error;
  d_phase = d_phase + d_freq + d_alpha * error;
}


void
gri_control_loop::phase_wrap()
{
  while(d_phase>M_TWOPI)
    d_phase -= M_TWOPI;
  while(d_phase<-M_TWOPI)
    d_phase += M_TWOPI;
}

void
gri_control_loop::frequency_limit()
{  
  if (d_freq > d_max_freq)
    d_freq = d_max_freq;
  else if (d_freq < d_min_freq)
    d_freq = d_min_freq;  
}

/*******************************************************************
    SET FUNCTIONS
*******************************************************************/

void
gri_control_loop::set_loop_bandwidth(float bw)
{
  if(bw < 0) {
    throw std::out_of_range ("gri_control_loop: invalid bandwidth. Must be >= 0.");
  }
  
  d_loop_bw = bw;
  update_gains();
}

void
gri_control_loop::set_damping_factor(float df) 
{
  if(df < 0 || df > 1.0) {
    throw std::out_of_range ("gri_control_loop: invalid damping factor. Must be in [0,1].");
  }
  
  d_damping = df;
  update_gains();
}

void
gri_control_loop::set_alpha(float alpha)
{
  if(alpha < 0 || alpha > 1.0) {
    throw std::out_of_range ("gri_control_loop: invalid alpha. Must be in [0,1].");
  }
  d_alpha = alpha;
}

void
gri_control_loop::set_beta(float beta)
{
  if(beta < 0 || beta > 1.0) {
    throw std::out_of_range ("gri_control_loop: invalid beta. Must be in [0,1].");
  }
  d_beta = beta;
}

void
gri_control_loop::set_frequency(float freq)
{
  if(freq > d_max_freq)
    d_freq = d_min_freq;
  else if(freq < d_min_freq)
    d_freq = d_max_freq;
  else
    d_freq = freq;
}

void
gri_control_loop::set_phase(float phase)
{
  d_phase = phase;
  while(d_phase>M_TWOPI)
    d_phase -= M_TWOPI;
  while(d_phase<-M_TWOPI)
    d_phase += M_TWOPI;
}

   
/*******************************************************************
    GET FUNCTIONS
*******************************************************************/


float
gri_control_loop::get_loop_bandwidth() const
{
  return d_loop_bw;
}

float
gri_control_loop::get_damping_factor() const
{
  return d_damping;
}

float
gri_control_loop::get_alpha() const
{
  return d_alpha;
}

float
gri_control_loop::get_beta() const
{
  return d_beta;
}

float
gri_control_loop::get_frequency() const
{
  return d_freq;
}

float
gri_control_loop::get_phase() const
{
  return d_phase;
}

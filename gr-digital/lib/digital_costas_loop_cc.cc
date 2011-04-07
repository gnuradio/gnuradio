/* -*- c++ -*- */
/*
 * Copyright 2006,2010,2011 Free Software Foundation, Inc.
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

#include <digital_costas_loop_cc.h>
#include <gr_io_signature.h>
#include <gr_expj.h>
#include <gr_sincos.h>
#include <math.h>

#define M_TWOPI (2*M_PI)

digital_costas_loop_cc_sptr
digital_make_costas_loop_cc (float damping, float nat_freq,
			     int order
			     ) throw (std::invalid_argument)
{
  return gnuradio::get_initial_sptr(new digital_costas_loop_cc (damping,
								nat_freq,
								order));
}

digital_costas_loop_cc::digital_costas_loop_cc (float damping, float nat_freq,
						int order
						) throw (std::invalid_argument)
  : gr_sync_block ("costas_loop_cc",
		   gr_make_io_signature (1, 1, sizeof (gr_complex)),
		   gr_make_io_signature2 (1, 2, sizeof (gr_complex), sizeof(float))),
    
    d_max_freq(1.0), d_min_freq(-1.0), d_phase(0), d_freq(0.0),
    d_nat_freq(nat_freq), d_damping(damping),
    d_order(order), d_phase_detector(NULL)
{
  // initialize gains from the natural freq and damping factors
  update_gains();

  switch(d_order) {
  case 2:
    d_phase_detector = &digital_costas_loop_cc::phase_detector_2;
    break;

  case 4:
    d_phase_detector = &digital_costas_loop_cc::phase_detector_4;
    break;

  case 8:
    d_phase_detector = &digital_costas_loop_cc::phase_detector_8;
    break;

  default: 
    throw std::invalid_argument("order must be 2, 4, or 8");
    break;
  }
}

float
digital_costas_loop_cc::phase_detector_8(gr_complex sample) const
{
  float K = sqrt(2.0) - 1;
  if(abs(sample.real()) >= abs(sample.imag())) {
    return ((sample.real()>0 ? 1.0 : -1.0) * sample.imag() -
	    (sample.imag()>0 ? 1.0 : -1.0) * sample.real() * K);
  }
  else {
    return ((sample.real()>0 ? 1.0 : -1.0) * sample.imag() * K -
	    (sample.imag()>0 ? 1.0 : -1.0) * sample.real());
  }
}

float
digital_costas_loop_cc::phase_detector_4(gr_complex sample) const
{

  return ((sample.real()>0 ? 1.0 : -1.0) * sample.imag() -
	  (sample.imag()>0 ? 1.0 : -1.0) * sample.real());
}

float
digital_costas_loop_cc::phase_detector_2(gr_complex sample) const
{
  return (sample.real()*sample.imag());
}

void
digital_costas_loop_cc::set_natural_freq(float w)
{
  d_nat_freq = w;
  update_gains();
}

void
digital_costas_loop_cc::set_damping_factor(float eta)
{
  d_damping = eta;
  update_gains();
}

void
digital_costas_loop_cc::update_gains()
{
  d_beta = d_nat_freq*d_nat_freq;
  d_alpha = 2*d_damping*d_nat_freq;
}

int
digital_costas_loop_cc::work (int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
{
  const gr_complex *iptr = (gr_complex *) input_items[0];
  gr_complex *optr = (gr_complex *) output_items[0];
  float *foptr = (float *) output_items[1];

  bool write_foptr = output_items.size() >= 2;

  float error;
  gr_complex nco_out;
  
  if (write_foptr) {

    for (int i = 0; i < noutput_items; i++){
      nco_out = gr_expj(-d_phase);
      optr[i] = iptr[i] * nco_out;
      
      error = (*this.*d_phase_detector)(optr[i]);
      if (error > 1)
	error = 1;
      else if (error < -1)
	error = -1;
	
      d_freq = d_freq + d_beta * error;
      d_phase = d_phase + d_freq + d_alpha * error;

      while(d_phase>M_TWOPI)
	d_phase -= M_TWOPI;
      while(d_phase<-M_TWOPI)
	d_phase += M_TWOPI;

      if (d_freq > d_max_freq)
	d_freq = d_min_freq;
      else if (d_freq < d_min_freq)
	d_freq = d_max_freq;
      
      foptr[i] = d_freq;
    } 
  } else {
    for (int i = 0; i < noutput_items; i++){
      nco_out = gr_expj(-d_phase);
      optr[i] = iptr[i] * nco_out;
      
      error = (*this.*d_phase_detector)(optr[i]);
      if (error > 1)
	error = 1;
      else if (error < -1)
	error = -1;
      
      d_freq = d_freq + d_beta * error;
      d_phase = d_phase + d_freq + d_alpha * error;
      
      while(d_phase>M_TWOPI)
	d_phase -= M_TWOPI;
      while(d_phase<-M_TWOPI)
	d_phase += M_TWOPI;
      
      if (d_freq > d_max_freq)
	d_freq = d_max_freq;
      else if (d_freq < d_min_freq)
	d_freq = d_min_freq;
      
    }
  }
  return noutput_items;
}

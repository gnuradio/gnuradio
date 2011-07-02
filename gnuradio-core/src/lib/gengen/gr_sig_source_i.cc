/* -*- c++ -*- */
/*
 * Copyright 2004,2010 Free Software Foundation, Inc.
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

// WARNING: this file is machine generated.  Edits will be over written

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gr_sig_source_i.h>
#include <algorithm>
#include <gr_io_signature.h>
#include <stdexcept>
#include <gr_complex.h>


gr_sig_source_i::gr_sig_source_i (double sampling_freq, gr_waveform_t waveform,
		double frequency, double ampl, int offset)
  : gr_sync_block ("sig_source_i",
		   gr_make_io_signature (0, 0, 0),
		   gr_make_io_signature (1, 1, sizeof (int))),
    d_sampling_freq (sampling_freq), d_waveform (waveform), d_frequency (frequency),
    d_ampl (ampl), d_offset (offset)
{
  d_nco.set_freq (2 * M_PI * d_frequency / d_sampling_freq);
}

gr_sig_source_i_sptr
gr_make_sig_source_i (double sampling_freq, gr_waveform_t waveform,
		     double frequency, double ampl, int offset)
{
  return gnuradio::get_initial_sptr(new gr_sig_source_i (sampling_freq, waveform, frequency, ampl, offset));
}

int
gr_sig_source_i::work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items)
{
  int *optr = (int *) output_items[0];
  int t;

  switch (d_waveform){

#if 0	// complex?

  case GR_CONST_WAVE:
    t = (gr_complex) d_ampl + d_offset;
    for (int i = 0; i < noutput_items; i++)	// FIXME unroll
      optr[i] = t;
    break;
    
  case GR_SIN_WAVE:
  case GR_COS_WAVE:
    d_nco.sincos (optr, noutput_items, d_ampl);
    if (d_offset == gr_complex(0,0))
      break;

    for (int i = 0; i < noutput_items; i++){
      optr[i] += d_offset;
    }
    break;
    
  /* Implements a real square wave high from -PI to 0.  
  * The imaginary square wave leads by 90 deg.
  */
  case GR_SQR_WAVE:	
    for (int i = 0; i < noutput_items; i++){
      if (d_nco.get_phase() < -1*M_PI/2)
        optr[i] = gr_complex(d_ampl, 0)+d_offset;
      else if (d_nco.get_phase() < 0)
        optr[i] = gr_complex(d_ampl, d_ampl)+d_offset;
      else if (d_nco.get_phase() < M_PI/2)
        optr[i] = gr_complex(0, d_ampl)+d_offset;
      else
        optr[i] = d_offset;		
      d_nco.step();
    }
    break;
	
  /* Implements a real triangle wave rising from -PI to 0 and  
  * falling from 0 to PI. The imaginary triangle wave leads by 90 deg.
  */
  case GR_TRI_WAVE:	
    for (int i = 0; i < noutput_items; i++){
      if (d_nco.get_phase() < -1*M_PI/2){
        optr[i] = gr_complex(d_ampl*d_nco.get_phase()/M_PI + d_ampl, 
          -1*d_ampl*d_nco.get_phase()/M_PI - d_ampl/2)+d_offset;
      }
      else if (d_nco.get_phase() < 0){
        optr[i] = gr_complex(d_ampl*d_nco.get_phase()/M_PI + d_ampl,
          d_ampl*d_nco.get_phase()/M_PI + d_ampl/2)+d_offset;
      }
      else if (d_nco.get_phase() < M_PI/2){
        optr[i] = gr_complex(-1*d_ampl*d_nco.get_phase()/M_PI + d_ampl, 
          d_ampl*d_nco.get_phase()/M_PI + d_ampl/2)+d_offset;
      }
      else{
        optr[i] = gr_complex(-1*d_ampl*d_nco.get_phase()/M_PI + d_ampl, 
          -1*d_ampl*d_nco.get_phase()/M_PI + 3*d_ampl/2)+d_offset;
      }
      d_nco.step();
    }
    break;
	
  /* Implements a real saw tooth wave rising from -PI to PI. 
  * The imaginary saw tooth wave leads by 90 deg.
  */
  case GR_SAW_WAVE:	
    for (int i = 0; i < noutput_items; i++){
      if (d_nco.get_phase() < -1*M_PI/2){
        optr[i] = gr_complex(d_ampl*d_nco.get_phase()/(2*M_PI) + d_ampl/2, 
          d_ampl*d_nco.get_phase()/(2*M_PI) + 5*d_ampl/4)+d_offset;
      }
      else{
        optr[i] = gr_complex(d_ampl*d_nco.get_phase()/(2*M_PI) + d_ampl/2,
          d_ampl*d_nco.get_phase()/(2*M_PI) + d_ampl/4)+d_offset;
      }
      d_nco.step();
    }  
    break;

#else			// nope...

  case GR_CONST_WAVE:
    t = (int) d_ampl + d_offset;
    for (int i = 0; i < noutput_items; i++)	// FIXME unroll
      optr[i] = t;
    break;
    
  case GR_SIN_WAVE:
    d_nco.sin (optr, noutput_items, d_ampl);
    if (d_offset == 0)
      break;

    for (int i = 0; i < noutput_items; i++){
      optr[i] += d_offset;
    }
    break;

  case GR_COS_WAVE:
    d_nco.cos (optr, noutput_items, d_ampl);
    if (d_offset == 0)
      break;

    for (int i = 0; i < noutput_items; i++){
      optr[i] += d_offset;
    }
    break;
   
  /* The square wave is high from -PI to 0.	*/ 
  case GR_SQR_WAVE:		
    t = (int) d_ampl + d_offset;
    for (int i = 0; i < noutput_items; i++){
      if (d_nco.get_phase() < 0)
        optr[i] = t;
      else
        optr[i] = d_offset;
      d_nco.step();
    }
    break;
	
  /* The triangle wave rises from -PI to 0 and falls from 0 to PI.	*/ 
  case GR_TRI_WAVE:	
    for (int i = 0; i < noutput_items; i++){
      double t = d_ampl*d_nco.get_phase()/M_PI;
      if (d_nco.get_phase() < 0)
	optr[i] = static_cast<int>(t + d_ampl + d_offset);
      else
	optr[i] = static_cast<int>(-1*t + d_ampl + d_offset);
      d_nco.step();
    }
    break;
	
  /* The saw tooth wave rises from -PI to PI.	*/
  case GR_SAW_WAVE:	
    for (int i = 0; i < noutput_items; i++){
      t = static_cast<int>(d_ampl*d_nco.get_phase()/(2*M_PI) + d_ampl/2 + d_offset);
      optr[i] = t;	   	
      d_nco.step();
    }
    break;

#endif

  default:
    throw std::runtime_error ("gr_sig_source: invalid waveform");
  }

  return noutput_items;
}

void
gr_sig_source_i::set_sampling_freq (double sampling_freq)
{
  d_sampling_freq = sampling_freq;
  d_nco.set_freq (2 * M_PI * d_frequency / d_sampling_freq);
}

void
gr_sig_source_i::set_waveform (gr_waveform_t waveform)
{
  d_waveform = waveform;
}

void
gr_sig_source_i::set_frequency (double frequency)
{
  d_frequency = frequency;
  d_nco.set_freq (2 * M_PI * d_frequency / d_sampling_freq);
}

void
gr_sig_source_i::set_amplitude (double ampl)
{
  d_ampl = ampl;
}

void
gr_sig_source_i::set_offset (int offset)
{
  d_offset = offset;
}


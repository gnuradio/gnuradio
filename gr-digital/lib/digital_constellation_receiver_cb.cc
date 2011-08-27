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

#include <gr_io_signature.h>
#include <gr_prefs.h>
#include <digital_constellation_receiver_cb.h>
#include <stdexcept>
#include <gr_math.h>
#include <gr_expj.h>


#define M_TWOPI (2*M_PI)
#define VERBOSE_MM     0     // Used for debugging symbol timing loop
#define VERBOSE_COSTAS 0     // Used for debugging phase and frequency tracking

// Public constructor

digital_constellation_receiver_cb_sptr 
digital_make_constellation_receiver_cb(digital_constellation_sptr constell,
				       float loop_bw, float fmin, float fmax)
{
  return gnuradio::get_initial_sptr(new digital_constellation_receiver_cb (constell,
									   loop_bw,
									   fmin, fmax));
}
 
static int ios[] = {sizeof(char), sizeof(float), sizeof(float), sizeof(float)};
static std::vector<int> iosig(ios, ios+sizeof(ios)/sizeof(int));
digital_constellation_receiver_cb::digital_constellation_receiver_cb (digital_constellation_sptr constellation, 
								      float loop_bw, float fmin, float fmax)
  : gr_block ("constellation_receiver_cb",
	      gr_make_io_signature (1, 1, sizeof (gr_complex)),
	      gr_make_io_signaturev (1, 4, iosig)),
    gri_control_loop(loop_bw, fmax, fmin),
    d_constellation(constellation), 
    d_current_const_point(0)
{
  if (d_constellation->dimensionality() != 1)
    throw std::runtime_error ("This receiver only works with constellations of dimension 1.");
}

void
digital_constellation_receiver_cb::phase_error_tracking(float phase_error)
{
  advance_loop(phase_error);
  phase_wrap();
  frequency_limit();
  
#if VERBOSE_COSTAS
  printf("cl: phase_error: %f  phase: %f  freq: %f  sample: %f+j%f  constellation: %f+j%f\n",
	 phase_error, d_phase, d_freq, sample.real(), sample.imag(), 
	 d_constellation->points()[d_current_const_point].real(),
	 d_constellation->points()[d_current_const_point].imag());
#endif
}

int
digital_constellation_receiver_cb::general_work (int noutput_items,
						 gr_vector_int &ninput_items,
						 gr_vector_const_void_star &input_items,
						 gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  unsigned char *out = (unsigned char *) output_items[0];

  int i=0;

  float phase_error;
  unsigned int sym_value;
  gr_complex sample, nco;

  float *out_err = 0, *out_phase = 0, *out_freq = 0;
  if(output_items.size() == 4) {
    out_err = (float *) output_items[1];
    out_phase = (float *) output_items[2];
    out_freq = (float *) output_items[3];
  }

  while((i < noutput_items) && (i < ninput_items[0])) {
    sample = in[i];
    nco = gr_expj(d_phase);   // get the NCO value for derotating the current sample
    sample = nco*sample;      // get the downconverted symbol

    sym_value = d_constellation->decision_maker_pe(&sample, &phase_error);
    phase_error_tracking(phase_error);  // corrects phase and frequency offsets

    out[i] = sym_value;

    if(output_items.size() == 4) {
      out_err[i] = phase_error;
      out_phase[i] = d_phase;
      out_freq[i] = d_freq;
    }
    i++;
  }

  consume_each(i);
  return i;
}


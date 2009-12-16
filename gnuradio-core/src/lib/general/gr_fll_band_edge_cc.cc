/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

#include <gr_fll_band_edge_cc.h>
#include <gr_fir_ccc.h>
#include <gr_fir_util.h>
#include <gri_fft.h>
#include <gr_io_signature.h>
#include <gr_expj.h>
#include <gr_math.h>
#include <cstdio>

#define M_TWOPI (2*M_PI)

float sinc(float x)
{
  if(x == 0)
    return 1;
  else
    return sin(M_PI*x)/(M_PI*x);
}
  


gr_fll_band_edge_cc_sptr gr_make_fll_band_edge_cc (float samps_per_sym, float rolloff,
						   int filter_size, float gain_alpha, float gain_beta)
{
  return gr_fll_band_edge_cc_sptr (new gr_fll_band_edge_cc (samps_per_sym, rolloff,
							    filter_size, gain_alpha, gain_beta));
}


gr_fll_band_edge_cc::gr_fll_band_edge_cc (float samps_per_sym, float rolloff,
					  int filter_size, float alpha, float beta)
  : gr_sync_block ("fll_band_edge_cc",
		   gr_make_io_signature (1, 1, sizeof(gr_complex)),
		   gr_make_io_signature (1, 1, sizeof(gr_complex))),
    d_alpha(alpha), d_beta(beta), d_updated (false)
{
  // base this on the number of samples per symbol
  d_max_freq =  M_TWOPI * (2.0/samps_per_sym);
  d_min_freq = -M_TWOPI * (2.0/samps_per_sym);

  d_freq = 0;
  d_phase = 0;

  int M = rint(filter_size / samps_per_sym);
  float power = 0;
  std::vector<float> bb_taps;
  for(int i = 0; i < filter_size; i++) {
    float k = -M + i*2.0/samps_per_sym;
    float tap = sinc(rolloff*k - 0.5) + sinc(rolloff*k + 0.5);
    power += tap;

    bb_taps.push_back(tap);
  }

  int N = (bb_taps.size() - 1.0)/2.0;
  std::vector<gr_complex> taps_lower;
  std::vector<gr_complex> taps_upper;
  for(int i = 0; i < bb_taps.size(); i++) {
    float tap = bb_taps[i] / power;

    float k = (-N + i)/(2.0*samps_per_sym);     //rng = scipy.arange(-nn2, nn2+1) / (2*spb);

    gr_complex t1 = tap * gr_expj(-2*M_PI*(1+rolloff)*k);
    gr_complex t2 = tap * gr_expj(2*M_PI*(1+rolloff)*k);

    taps_lower.push_back(t1);
    taps_upper.push_back(t2);
  }

  std::vector<gr_complex> vtaps(0, taps_lower.size());
  d_filter_upper = gr_fir_util::create_gr_fir_ccc(vtaps);
  d_filter_lower = gr_fir_util::create_gr_fir_ccc(vtaps);

  set_taps_lower(taps_lower);
  set_taps_upper(taps_upper);
}

gr_fll_band_edge_cc::~gr_fll_band_edge_cc ()
{
}

void
gr_fll_band_edge_cc::set_taps_lower (const std::vector<gr_complex> &taps)
{
  unsigned int i;

  for(i = 0; i < taps.size(); i++) {
    d_taps_lower.push_back(taps[i]);
  }

  d_filter_lower->set_taps(d_taps_lower);

  // Set the history to ensure enough input items for each filter
  set_history(d_taps_lower.size()+1);

  d_updated = true;
}

void
gr_fll_band_edge_cc::set_taps_upper (const std::vector<gr_complex> &taps)
{
  unsigned int i;

  for(i = 0; i < taps.size(); i++) {
    d_taps_upper.push_back(taps[i]);
  }

  d_filter_upper->set_taps(d_taps_upper);

  // Set the history to ensure enough input items for each filter
  set_history(d_taps_upper.size()+1);

  d_updated = true;
}

void
gr_fll_band_edge_cc::print_taps()
{
  unsigned int i;
  printf("Upper Band-edge: [");
  for(i = 0; i < d_taps_upper.size(); i++) {
    printf(" %.4e + %.4ej,", d_taps_upper[i].real(), d_taps_upper[i].imag());
  }
  printf("]\n\n");

  printf("Lower Band-edge: [");
  for(i = 0; i < d_taps_lower.size(); i++) {
    printf(" %.4e + %.4ej,", d_taps_lower[i].real(), d_taps_lower[i].imag());
  }
  printf("]\n\n");
}

int
gr_fll_band_edge_cc::work (int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
{
  const gr_complex *in  = (const gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];

  if (d_updated) {
    d_updated = false;
    return 0;		     // history requirements may have changed.
  }

  int i;
  gr_complex nco_out;
  float out_upper, out_lower;
  float error;
  for(i = 0; i < noutput_items; i++) {
    nco_out = gr_expj(d_phase);
    out[i] = in[i] * nco_out;

    out_upper = norm(d_filter_upper->filter(&out[i]));
    out_lower = norm(d_filter_lower->filter(&out[i]));
    error = out_lower - out_upper;
    d_error = 0.1*error + 0.9*d_error;  // average error

    d_freq = d_freq + d_beta * error;
    d_phase = d_phase + d_freq + d_alpha * error;

    if(d_phase > M_PI)
      d_phase -= M_TWOPI;
    else if(d_phase < -M_PI)
      d_phase += M_TWOPI;
    
    if (d_freq > d_max_freq)
      d_freq = d_max_freq;
    else if (d_freq < d_min_freq)
      d_freq = d_min_freq;
  }

  return noutput_items;
}

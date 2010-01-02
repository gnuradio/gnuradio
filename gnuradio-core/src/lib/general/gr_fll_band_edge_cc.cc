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


static int ios[] = {sizeof(gr_complex), sizeof(float), sizeof(float), sizeof(float)};
static std::vector<int> iosig(ios, ios+sizeof(ios)/sizeof(int));
gr_fll_band_edge_cc::gr_fll_band_edge_cc (float samps_per_sym, float rolloff,
					  int filter_size, float alpha, float beta)
  : gr_sync_block ("fll_band_edge_cc",
		   gr_make_io_signature (1, 1, sizeof(gr_complex)),
		   gr_make_io_signaturev (1, 4, iosig)),
    d_alpha(alpha), d_beta(beta), d_updated (false)
{
  // base this on the number of samples per symbol
  d_max_freq =  M_TWOPI * (2.0/samps_per_sym);
  d_min_freq = -M_TWOPI * (2.0/samps_per_sym);

  d_freq = 0;
  d_phase = 0;

  set_alpha(alpha);

  design_filter(samps_per_sym, rolloff, filter_size);
}

gr_fll_band_edge_cc::~gr_fll_band_edge_cc ()
{
  delete d_filter_lower;
  delete d_filter_upper;
}

void
gr_fll_band_edge_cc::set_alpha(float alpha) 
{ 
  float eta = sqrt(2.0)/2.0;
  float theta = alpha;
  d_alpha = (4*eta*theta) / (1.0 + 2.0*eta*theta + theta*theta);
  d_beta = (4*theta*theta) / (1.0 + 2.0*eta*theta + theta*theta);
}

void
gr_fll_band_edge_cc::design_filter(float samps_per_sym, float rolloff, int filter_size)
{
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
  for(unsigned int i = 0; i < bb_taps.size(); i++) {
    float tap = bb_taps[i] / power;

    float k = (-N + (int)i)/(2.0*samps_per_sym);
    
    gr_complex t1 = tap * gr_expj(-2*M_PI*(1+rolloff)*k);
    gr_complex t2 = tap * gr_expj(2*M_PI*(1+rolloff)*k);

    taps_lower.push_back(t1);
    taps_upper.push_back(t2);
  }

  std::vector<gr_complex> vtaps(0, taps_lower.size());
  d_filter_upper = gr_fir_util::create_gr_fir_ccc(vtaps);
  d_filter_lower = gr_fir_util::create_gr_fir_ccc(vtaps);

  d_filter_lower->set_taps(taps_lower);
  d_filter_upper->set_taps(taps_upper);

  d_updated = true;

  // Set the history to ensure enough input items for each filter
  set_history(filter_size+1);

}

void
gr_fll_band_edge_cc::print_taps()
{
  unsigned int i;
  std::vector<gr_complex> taps_upper = d_filter_upper->get_taps();
  std::vector<gr_complex> taps_lower = d_filter_lower->get_taps();

  printf("Upper Band-edge: [");
  for(i = 0; i < taps_upper.size(); i++) {
    printf(" %.4e + %.4ej,", taps_upper[i].real(), taps_upper[i].imag());
  }
  printf("]\n\n");

  printf("Lower Band-edge: [");
  for(i = 0; i < taps_lower.size(); i++) {
    printf(" %.4e + %.4ej,", taps_lower[i].real(), taps_lower[i].imag());
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

  float *frq, *phs, *err;
  if(output_items.size() > 2) {
    frq = (float *) output_items[1];
    phs = (float *) output_items[2];
    err = (float *) output_items[3];
  }

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
    d_error = 0.01*error + 0.99*d_error;  // average error

    d_freq = d_freq + d_beta * d_error;
    d_phase = d_phase + d_freq + d_alpha * d_error;

    if(d_phase > M_PI)
      d_phase -= M_TWOPI;
    else if(d_phase < -M_PI)
      d_phase += M_TWOPI;

    if (d_freq > d_max_freq)
      d_freq = d_max_freq;
    else if (d_freq < d_min_freq)
      d_freq = d_min_freq;

    if(output_items.size() > 2) {
      frq[i] = d_freq;
      phs[i] = d_phase;
      err[i] = d_error;
    }
  }


  return noutput_items;
}

/* -*- c++ -*- */
/*
 * Copyright 2006-2008,2010,2011 Free Software Foundation, Inc.
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

#include "ofdm_frame_acquisition_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/expj.h>
#include <gnuradio/math.h>
#include <cstdio>

namespace gr {
  namespace digital {

#define VERBOSE 0
#define M_TWOPI (2*M_PI)
#define MAX_NUM_SYMBOLS 1000

    ofdm_frame_acquisition::sptr
    ofdm_frame_acquisition::make(unsigned int occupied_carriers,
				 unsigned int fft_length,
				 unsigned int cplen,
				 const std::vector<gr_complex> &known_symbol,
				 unsigned int max_fft_shift_len)
    {
      return gnuradio::get_initial_sptr
	(new ofdm_frame_acquisition_impl(occupied_carriers, fft_length, cplen,
					 known_symbol, max_fft_shift_len));
    }

    ofdm_frame_acquisition_impl::ofdm_frame_acquisition_impl(unsigned occupied_carriers,
							     unsigned int fft_length,
							     unsigned int cplen,
							     const std::vector<gr_complex> &known_symbol,
							     unsigned int max_fft_shift_len)
      : block("ofdm_frame_acquisition",
		 io_signature::make2(2, 2, sizeof(gr_complex)*fft_length, sizeof(char)*fft_length),
		 io_signature::make2(2, 2, sizeof(gr_complex)*occupied_carriers, sizeof(char))),
	d_occupied_carriers(occupied_carriers),
	d_fft_length(fft_length),
	d_cplen(cplen),
	d_freq_shift_len(max_fft_shift_len),
	d_known_symbol(known_symbol),
	d_coarse_freq(0),
	d_phase_count(0)
    {
      GR_LOG_WARN(d_logger, "The gr::digital::ofdm_frame_acquisition block has been deprecated.");

      d_symbol_phase_diff.resize(d_fft_length);
      d_known_phase_diff.resize(d_occupied_carriers);
      d_hestimate.resize(d_occupied_carriers);

      unsigned int i = 0, j = 0;

      std::fill(d_known_phase_diff.begin(), d_known_phase_diff.end(), 0);
      for(i = 0; i < d_known_symbol.size()-2; i+=2) {
	d_known_phase_diff[i] = norm(d_known_symbol[i] - d_known_symbol[i+2]);
      }

      d_phase_lut = new gr_complex[(2*d_freq_shift_len+1) * MAX_NUM_SYMBOLS];
      for(i = 0; i <= 2*d_freq_shift_len; i++) {
	for(j = 0; j < MAX_NUM_SYMBOLS; j++) {
	  d_phase_lut[j + i*MAX_NUM_SYMBOLS] =  gr_expj(-M_TWOPI*d_cplen/d_fft_length*(i-d_freq_shift_len)*j);
	}
      }
    }

    ofdm_frame_acquisition_impl::~ofdm_frame_acquisition_impl()
    {
      delete [] d_phase_lut;
    }

    void
    ofdm_frame_acquisition_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      for(unsigned i = 0; i < ninputs; i++)
	ninput_items_required[i] = 1;
    }

    gr_complex
    ofdm_frame_acquisition_impl::coarse_freq_comp(int freq_delta, int symbol_count)
    {
      //  return gr_complex(cos(-M_TWOPI*freq_delta*d_cplen/d_fft_length*symbol_count),
      //	    sin(-M_TWOPI*freq_delta*d_cplen/d_fft_length*symbol_count));

      return gr_expj(-M_TWOPI*freq_delta*d_cplen/d_fft_length*symbol_count);

      //return d_phase_lut[MAX_NUM_SYMBOLS * (d_freq_shift_len + freq_delta) + symbol_count];
    }

    void
    ofdm_frame_acquisition_impl::correlate(const gr_complex *symbol, int zeros_on_left)
    {
      unsigned int i,j;

      std::fill(d_symbol_phase_diff.begin(), d_symbol_phase_diff.end(), 0);
      for(i = 0; i < d_fft_length-2; i++) {
	d_symbol_phase_diff[i] = norm(symbol[i] - symbol[i+2]);
      }

      // sweep through all possible/allowed frequency offsets and select the best
      int index = 0;
      float max = 0, sum=0;
      for(i =  zeros_on_left - d_freq_shift_len; i < zeros_on_left + d_freq_shift_len; i++) {
	sum = 0;
	for(j = 0; j < d_occupied_carriers; j++) {
	  sum += (d_known_phase_diff[j] * d_symbol_phase_diff[i+j]);
	}
	if(sum > max) {
	  max = sum;
	  index = i;
	}
      }

      // set the coarse frequency offset relative to the edge of the occupied tones
      d_coarse_freq = index - zeros_on_left;
    }

    void
    ofdm_frame_acquisition_impl::calculate_equalizer(const gr_complex *symbol, int zeros_on_left)
    {
      unsigned int i=0;

      // Set first tap of equalizer
      d_hestimate[0] = d_known_symbol[0] /
	(coarse_freq_comp(d_coarse_freq,1)*symbol[zeros_on_left+d_coarse_freq]);

      // set every even tap based on known symbol
      // linearly interpolate between set carriers to set zero-filled carriers
      // FIXME: is this the best way to set this?
      for(i = 2; i < d_occupied_carriers; i+=2) {
	d_hestimate[i] = d_known_symbol[i] /
	  (coarse_freq_comp(d_coarse_freq,1)*(symbol[i+zeros_on_left+d_coarse_freq]));
	d_hestimate[i-1] = (d_hestimate[i] + d_hestimate[i-2]) / gr_complex(2.0, 0.0);
      }

      // with even number of carriers; last equalizer tap is wrong
      if(!(d_occupied_carriers & 1)) {
	d_hestimate[d_occupied_carriers-1] = d_hestimate[d_occupied_carriers-2];
      }

      if(VERBOSE) {
	fprintf(stderr, "Equalizer setting:\n");
	for(i = 0; i < d_occupied_carriers; i++) {
	  gr_complex sym = coarse_freq_comp(d_coarse_freq,1)*symbol[i+zeros_on_left+d_coarse_freq];
	  gr_complex output = sym * d_hestimate[i];
	  fprintf(stderr, "sym: %+.4f + j%+.4f  ks: %+.4f + j%+.4f  eq: %+.4f + j%+.4f  ==>  %+.4f + j%+.4f\n",
		  sym .real(), sym.imag(),
		  d_known_symbol[i].real(), d_known_symbol[i].imag(),
		  d_hestimate[i].real(), d_hestimate[i].imag(),
		  output.real(), output.imag());
	}
	fprintf(stderr, "\n");
      }
    }

    int
    ofdm_frame_acquisition_impl::general_work(int noutput_items,
					      gr_vector_int &ninput_items,
					      gr_vector_const_void_star &input_items,
					      gr_vector_void_star &output_items)
    {
      const gr_complex *symbol = (const gr_complex *)input_items[0];
      const char *signal_in = (const char *)input_items[1];

      gr_complex *out = (gr_complex *) output_items[0];
      char *signal_out = (char *) output_items[1];

      int unoccupied_carriers = d_fft_length - d_occupied_carriers;
      int zeros_on_left = (int)ceil(unoccupied_carriers/2.0);

      if(signal_in[0]) {
	d_phase_count = 1;
	correlate(symbol, zeros_on_left);
	calculate_equalizer(symbol, zeros_on_left);
	signal_out[0] = 1;
      }
      else {
	signal_out[0] = 0;
      }

      for(unsigned int i = 0; i < d_occupied_carriers; i++) {
	out[i] = d_hestimate[i]*coarse_freq_comp(d_coarse_freq,d_phase_count)
	  *symbol[i+zeros_on_left+d_coarse_freq];
      }

      d_phase_count++;
      if(d_phase_count == MAX_NUM_SYMBOLS) {
	d_phase_count = 1;
      }

      consume_each(1);
      return 1;
    }

  } /* namespace digital */
} /* namespace gr */

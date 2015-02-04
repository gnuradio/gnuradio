/* -*- c++ -*- */
/*
 * Copyright 2009-2012,2014 Free Software Foundation, Inc.
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

#include "fll_band_edge_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/expj.h>
#include <cstdio>

namespace gr {
  namespace digital {

#define M_TWOPI (2*M_PI)

    float sinc(float x)
    {
      if(x == 0)
	return 1;
      else
	return sin(M_PI*x)/(M_PI*x);
    }

    fll_band_edge_cc::sptr
    fll_band_edge_cc::make(float samps_per_sym, float rolloff,
			   int filter_size, float bandwidth)
    {
      return gnuradio::get_initial_sptr
	(new fll_band_edge_cc_impl(samps_per_sym, rolloff,
				   filter_size, bandwidth));
    }

    static int ios[] = {sizeof(gr_complex), sizeof(float), sizeof(float), sizeof(float)};
    static std::vector<int> iosig(ios, ios+sizeof(ios)/sizeof(int));
    fll_band_edge_cc_impl::fll_band_edge_cc_impl(float samps_per_sym, float rolloff,
						 int filter_size, float bandwidth)
      : sync_block("fll_band_edge_cc",
		      io_signature::make(1, 1, sizeof(gr_complex)),
		      io_signature::makev(1, 4, iosig)),
	blocks::control_loop(bandwidth, M_TWOPI*(2.0/samps_per_sym),
                                 -M_TWOPI*(2.0/samps_per_sym)),
	d_updated(false)
    {
      // Initialize samples per symbol
      if(samps_per_sym <= 0) {
	throw std::out_of_range("fll_band_edge_cc: invalid number of sps. Must be > 0.");
      }
      d_sps = samps_per_sym;

      // Initialize rolloff factor
      if(rolloff < 0 || rolloff > 1.0) {
	throw std::out_of_range("fll_band_edge_cc: invalid rolloff factor. Must be in [0,1].");
      }
      d_rolloff = rolloff;

      // Initialize filter length
      if(filter_size <= 0) {
	throw std::out_of_range("fll_band_edge_cc: invalid filter size. Must be > 0.");
      }
      d_filter_size = filter_size;

      // Build the band edge filters
      design_filter(d_sps, d_rolloff, d_filter_size);
    }

    fll_band_edge_cc_impl::~fll_band_edge_cc_impl()
    {
      delete d_filter_upper;
      delete d_filter_lower;
    }

    /*******************************************************************
     SET FUNCTIONS
    *******************************************************************/

    void
    fll_band_edge_cc_impl::set_samples_per_symbol(float sps)
    {
      if(sps <= 0) {
	throw std::out_of_range("fll_band_edge_cc: invalid number of sps. Must be > 0.");
      }
      d_sps = sps;
      set_max_freq(M_TWOPI*(2.0/sps));
      set_min_freq(-M_TWOPI*(2.0/sps));
      design_filter(d_sps, d_rolloff, d_filter_size);
    }

    void
    fll_band_edge_cc_impl::set_rolloff(float rolloff)
    {
      if(rolloff < 0 || rolloff > 1.0) {
	throw std::out_of_range("fll_band_edge_cc: invalid rolloff factor. Must be in [0,1].");
      }
      d_rolloff = rolloff;
      design_filter(d_sps, d_rolloff, d_filter_size);
    }

    void
    fll_band_edge_cc_impl::set_filter_size(int filter_size)
    {
      if(filter_size <= 0) {
	throw std::out_of_range("fll_band_edge_cc: invalid filter size. Must be > 0.");
      }
      d_filter_size = filter_size;
      design_filter(d_sps, d_rolloff, d_filter_size);
    }

    /*******************************************************************
     GET FUNCTIONS
    *******************************************************************/

    float
    fll_band_edge_cc_impl::samples_per_symbol() const
    {
      return d_sps;
    }

    float
    fll_band_edge_cc_impl::rolloff() const
    {
      return d_rolloff;
    }

    int
    fll_band_edge_cc_impl::filter_size() const
    {
      return d_filter_size;
    }

    /*******************************************************************
     *******************************************************************/

    void
    fll_band_edge_cc_impl::design_filter(float samps_per_sym,
					 float rolloff, int filter_size)
    {
      int M = rint(filter_size / samps_per_sym);
      float power = 0;

      // Create the baseband filter by adding two sincs together
      std::vector<float> bb_taps;
      for(int i = 0; i < filter_size; i++) {
	float k = -M + i*2.0/samps_per_sym;
	float tap = sinc(rolloff*k - 0.5) + sinc(rolloff*k + 0.5);
	power += tap;

	bb_taps.push_back(tap);
      }

      d_taps_lower.resize(filter_size);
      d_taps_upper.resize(filter_size);

      // Create the band edge filters by spinning the baseband
      // filter up and down to the right places in frequency.
      // Also, normalize the power in the filters
      int N = (bb_taps.size() - 1.0)/2.0;
      for(int i = 0; i < filter_size; i++) {
	float tap = bb_taps[i] / power;

	float k = (-N + (int)i)/(2.0*samps_per_sym);

	gr_complex t1 = tap * gr_expj(-M_TWOPI*(1+rolloff)*k);
	gr_complex t2 = tap * gr_expj(M_TWOPI*(1+rolloff)*k);

	d_taps_lower[filter_size-i-1] = t1;
	d_taps_upper[filter_size-i-1] = t2;
      }

      d_updated = true;

      // Set the history to ensure enough input items for each filter
      set_history(filter_size+1);
      d_filter_upper = new gr::filter::kernel::fir_filter_with_buffer_ccc(d_taps_upper);
      d_filter_lower = new gr::filter::kernel::fir_filter_with_buffer_ccc(d_taps_lower);
    }

    void
    fll_band_edge_cc_impl::print_taps()
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
    fll_band_edge_cc_impl::work(int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];

      float *frq = NULL;
      float *phs = NULL;
      float *err = NULL;
      if(output_items.size() == 4) {
	frq = (float*)output_items[1];
	phs = (float*)output_items[2];
	err = (float*)output_items[3];
      }

      if(d_updated) {
	d_updated = false;
	return 0;     // history requirements may have changed.
      }

      int i;
      float error;
      gr_complex nco_out;
      gr_complex out_upper, out_lower;
      gr_complex out_uppersse, out_lowersse;

      for(i = 0; i < noutput_items; i++) {
	nco_out = gr_expj(d_phase);
	out[i] = in[i] * nco_out;

	// Perform the dot product of the output with the filters
	out_upper = d_filter_lower->filter(out[i]);
	out_lower = d_filter_upper->filter(out[i]);

	error = norm(out_lower) - norm(out_upper);

	advance_loop(error);
	phase_wrap();
	frequency_limit();

	if(output_items.size() == 4) {
	  frq[i] = d_freq;
	  phs[i] = d_phase;
	  err[i] = error;
	}
      }

      return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012 Free Software Foundation, Inc.
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

#include <cstdio>
#include <cmath>

#include "pfb_clock_sync_fff_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>

namespace gr {
  namespace digital {

    pfb_clock_sync_fff::sptr
    pfb_clock_sync_fff::make(double sps, float gain,
			     const std::vector<float> &taps,
			     unsigned int filter_size,
			     float init_phase,
			     float max_rate_deviation,
			     int osps)
    {
      return gnuradio::get_initial_sptr
	(new pfb_clock_sync_fff_impl(sps, gain, taps,
				     filter_size,
				     init_phase,
				     max_rate_deviation,
				     osps));
    }

    static int ios[] = {sizeof(float), sizeof(float), sizeof(float), sizeof(float)};
    static std::vector<int> iosig(ios, ios+sizeof(ios)/sizeof(int));
    pfb_clock_sync_fff_impl::pfb_clock_sync_fff_impl(double sps, float loop_bw,
						     const std::vector<float> &taps,
						     unsigned int filter_size,
						     float init_phase,
						     float max_rate_deviation,
						     int osps)
      : block("pfb_clock_sync_fff",
		 io_signature::make(1, 1, sizeof(float)),
		 io_signature::makev(1, 4, iosig)),
	d_updated(false), d_nfilters(filter_size),
	d_max_dev(max_rate_deviation),
	d_osps(osps), d_error(0), d_out_idx(0)
    {
      if(taps.size() == 0)
        throw std::runtime_error("pfb_clock_sync_fff: please specify a filter.\n");

      // Let scheduler adjust our relative_rate.
      enable_update_rate(true);

      d_nfilters = filter_size;
      d_sps = floor(sps);

      // Set the damping factor for a critically damped system
      d_damping = 2*d_nfilters;

      // Set the bandwidth, which will then call update_gains()
      set_loop_bandwidth(loop_bw);

      // Store the last filter between calls to work
      // The accumulator keeps track of overflow to increment the stride correctly.
      // set it here to the fractional difference based on the initial phaes
      d_k = init_phase;
      d_rate = (sps-floor(sps))*(double)d_nfilters;
      d_rate_i = (int)floor(d_rate);
      d_rate_f = d_rate - (float)d_rate_i;
      d_filtnum = (int)floor(d_k);

      d_filters = std::vector<kernel::fir_filter_fff*>(d_nfilters);
      d_diff_filters = std::vector<kernel::fir_filter_fff*>(d_nfilters);

      // Create an FIR filter for each channel and zero out the taps
      std::vector<float> vtaps(1,0);
      for(int i = 0; i < d_nfilters; i++) {
	d_filters[i] = new kernel::fir_filter_fff(1, vtaps);
	d_diff_filters[i] = new kernel::fir_filter_fff(1, vtaps);
      }

      // Now, actually set the filters' taps
      std::vector<float> dtaps;
      create_diff_taps(taps, dtaps);
      set_taps(taps, d_taps, d_filters);
      set_taps(dtaps, d_dtaps, d_diff_filters);

      set_relative_rate((float)d_osps/(float)d_sps);
    }

    pfb_clock_sync_fff_impl::~pfb_clock_sync_fff_impl()
    {
      for(int i = 0; i < d_nfilters; i++) {
	delete d_filters[i];
	delete d_diff_filters[i];
      }
    }

    bool
    pfb_clock_sync_fff_impl::check_topology(int ninputs, int noutputs)
    {
      return noutputs == 1 || noutputs == 4;
    }

    void
    pfb_clock_sync_fff_impl::forecast(int noutput_items,
                                      gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size ();
      for(unsigned i = 0; i < ninputs; i++)
        ninput_items_required[i] = (noutput_items + history()) * (d_sps/d_osps);
    }

    void
    pfb_clock_sync_fff_impl::update_taps(const std::vector<float> &taps)
    {
      d_updated_taps = taps;
      d_updated = true;
    }

    /*******************************************************************
     SET FUNCTIONS
    *******************************************************************/

    void
    pfb_clock_sync_fff_impl::set_loop_bandwidth(float bw)
    {
      if(bw < 0) {
	throw std::out_of_range("pfb_clock_sync_fff: invalid bandwidth. Must be >= 0.");
      }

      d_loop_bw = bw;
      update_gains();
    }

    void
    pfb_clock_sync_fff_impl::set_damping_factor(float df)
    {
      if(df < 0 || df > 1.0) {
	throw std::out_of_range("pfb_clock_sync_fff: invalid damping factor. Must be in [0,1].");
      }

      d_damping = df;
      update_gains();
    }

    void
    pfb_clock_sync_fff_impl::set_alpha(float alpha)
    {
      if(alpha < 0 || alpha > 1.0) {
	throw std::out_of_range("pfb_clock_sync_fff: invalid alpha. Must be in [0,1].");
      }
      d_alpha = alpha;
    }

    void
    pfb_clock_sync_fff_impl::set_beta(float beta)
    {
      if(beta < 0 || beta > 1.0) {
	throw std::out_of_range("pfb_clock_sync_fff: invalid beta. Must be in [0,1].");
      }
      d_beta = beta;
    }

    /*******************************************************************
     GET FUNCTIONS
    *******************************************************************/

    float
    pfb_clock_sync_fff_impl::loop_bandwidth() const
    {
      return d_loop_bw;
    }

    float
    pfb_clock_sync_fff_impl::damping_factor() const
    {
      return d_damping;
    }

    float
    pfb_clock_sync_fff_impl::alpha() const
    {
      return d_alpha;
    }

    float
    pfb_clock_sync_fff_impl::beta() const
    {
      return d_beta;
    }

    float
    pfb_clock_sync_fff_impl::clock_rate() const
    {
      return d_rate_f;
    }

    /*******************************************************************
     *******************************************************************/

    void
    pfb_clock_sync_fff_impl::update_gains()
    {
      float denom = (1.0 + 2.0*d_damping*d_loop_bw + d_loop_bw*d_loop_bw);
      d_alpha = (4*d_damping*d_loop_bw) / denom;
      d_beta = (4*d_loop_bw*d_loop_bw) / denom;
    }

    void
    pfb_clock_sync_fff_impl::set_taps(const std::vector<float> &newtaps,
				      std::vector< std::vector<float> > &ourtaps,
				      std::vector<kernel::fir_filter_fff*> &ourfilter)
    {
      int i,j;

      unsigned int ntaps = newtaps.size();
      d_taps_per_filter = (unsigned int)ceil((double)ntaps/(double)d_nfilters);

      // Create d_numchan vectors to store each channel's taps
      ourtaps.resize(d_nfilters);

      // Make a vector of the taps plus fill it out with 0's to fill
      // each polyphase filter with exactly d_taps_per_filter
      std::vector<float> tmp_taps;
      tmp_taps = newtaps;
      while((float)(tmp_taps.size()) < d_nfilters*d_taps_per_filter) {
	tmp_taps.push_back(0.0);
      }

      // Partition the filter
      for(i = 0; i < d_nfilters; i++) {
	// Each channel uses all d_taps_per_filter with 0's if not enough taps to fill out
	ourtaps[i] = std::vector<float>(d_taps_per_filter, 0);
	for(j = 0; j < d_taps_per_filter; j++) {
	  ourtaps[i][j] = tmp_taps[i + j*d_nfilters];
	}

	// Build a filter for each channel and add it's taps to it
	ourfilter[i]->set_taps(ourtaps[i]);
      }

      // Set the history to ensure enough input items for each filter
      set_history(d_taps_per_filter + d_sps + d_sps);

      // Make sure there is enough output space for d_osps outputs/input.
      set_output_multiple(d_osps);
    }

    void
    pfb_clock_sync_fff_impl::create_diff_taps(const std::vector<float> &newtaps,
					      std::vector<float> &difftaps)
    {
      std::vector<float> diff_filter(3);
      diff_filter[0] = -1;
      diff_filter[1] = 0;
      diff_filter[2] = 1;

      float pwr = 0;
      difftaps.clear();
      difftaps.push_back(0);
      for(unsigned int i = 0; i < newtaps.size()-2; i++) {
	float tap = 0;
	for(unsigned int j = 0; j < diff_filter.size(); j++) {
	  tap += diff_filter[j]*newtaps[i+j];
	}
	difftaps.push_back(tap);
        pwr += fabsf(tap);
      }
      difftaps.push_back(0);

      // Normalize the taps
      for(unsigned int i = 0; i < difftaps.size(); i++) {
        difftaps[i] *= d_nfilters/pwr;
        if(difftaps[i] != difftaps[i]) {
          throw std::runtime_error("pfb_clock_sync_fff::create_diff_taps produced NaN.");
        }
      }
    }

    std::string
    pfb_clock_sync_fff_impl::taps_as_string() const
    {
      int i, j;
      std::stringstream str;
      str.precision(4);
      str.setf(std::ios::scientific);

      str << "[ ";
      for(i = 0; i < d_nfilters; i++) {
	str << "[" << d_taps[i][0] << ", ";
	for(j = 1; j < d_taps_per_filter-1; j++) {
	  str << d_taps[i][j] << ", ";
	}
	str << d_taps[i][j] << "],";
      }
      str << " ]" << std::endl;

      return str.str();
    }

    std::string
    pfb_clock_sync_fff_impl::diff_taps_as_string() const
    {
      int i, j;
      std::stringstream str;
      str.precision(4);
      str.setf(std::ios::scientific);

      str << "[ ";
      for(i = 0; i < d_nfilters; i++) {
	str << "[" << d_dtaps[i][0] << ", ";
	for(j = 1; j < d_taps_per_filter-1; j++) {
	  str << d_dtaps[i][j] << ", ";
	}
	str << d_dtaps[i][j] << "],";
      }
      str << " ]" << std::endl;

      return str.str();
    }

    std::vector< std::vector<float> >
    pfb_clock_sync_fff_impl::taps() const
    {
      return d_taps;
    }

    std::vector< std::vector<float> >
    pfb_clock_sync_fff_impl::diff_taps() const
    {
      return d_dtaps;
    }

    std::vector<float>
    pfb_clock_sync_fff_impl::channel_taps(int channel) const
    {
      std::vector<float> taps;
      for(int i = 0; i < d_taps_per_filter; i++) {
	taps.push_back(d_taps[channel][i]);
      }
      return taps;
    }

    std::vector<float>
    pfb_clock_sync_fff_impl::diff_channel_taps(int channel) const
    {
      std::vector<float> taps;
      for(int i = 0; i < d_taps_per_filter; i++) {
	taps.push_back(d_dtaps[channel][i]);
      }
      return taps;
    }

    int
    pfb_clock_sync_fff_impl::general_work(int noutput_items,
					  gr_vector_int &ninput_items,
					  gr_vector_const_void_star &input_items,
					  gr_vector_void_star &output_items)
    {
      float *in = (float *) input_items[0];
      float *out = (float *) output_items[0];

      if(d_updated) {
        std::vector<float> dtaps;
        create_diff_taps(d_updated_taps, dtaps);
        set_taps(d_updated_taps, d_taps, d_filters);
        set_taps(dtaps, d_dtaps, d_diff_filters);
	d_updated = false;
	return 0;		     // history requirements may have changed.
      }

      float *err = NULL, *outrate = NULL, *outk = NULL;
      if(output_items.size() == 4) {
	err = (float *) output_items[1];
	outrate = (float*)output_items[2];
	outk = (float*)output_items[3];
      }

      int i = 0, count = 0;

      // produce output as long as we can and there are enough input samples
      while(i < noutput_items) {
	while(d_out_idx < d_osps) {
	  d_filtnum = (int)floor(d_k);

	  // Keep the current filter number in [0, d_nfilters]
	  // If we've run beyond the last filter, wrap around and go to next sample
	  // If we've gone below 0, wrap around and go to previous sample
	  while(d_filtnum >= d_nfilters) {
	    d_k -= d_nfilters;
	    d_filtnum -= d_nfilters;
	    count += 1;
	  }
	  while(d_filtnum < 0) {
	    d_k += d_nfilters;
	    d_filtnum += d_nfilters;
	    count -= 1;
	  }

	  out[i+d_out_idx] = d_filters[d_filtnum]->filter(&in[count+d_out_idx]);
	  d_k = d_k + d_rate_i + d_rate_f; // update phase
	  d_out_idx++;

	  if(output_items.size() == 4) {
	    err[i] = d_error;
	    outrate[i] = d_rate_f;
	    outk[i] = d_k;
	  }

	  // We've run out of output items we can create; return now.
	  if(i+d_out_idx >= noutput_items) {
	    consume_each(count);
	    return i;
	  }
	}

	// reset here; if we didn't complete a full osps samples last time,
	// the early return would take care of it.
	d_out_idx = 0;

	// Update the phase and rate estimates for this symbol
	float diff = d_diff_filters[d_filtnum]->filter(&in[count]);
	d_error = out[i] * diff;

	// Run the control loop to update the current phase (k) and
	// tracking rate estimates based on the error value
        // Interpolating here to update rates for ever sps.
        for(int s = 0; s < d_sps; s++) {
          d_rate_f = d_rate_f + d_beta*d_error;
          d_k = d_k + d_rate_f + d_alpha*d_error;
        }

	// Keep our rate within a good range
	d_rate_f = gr::branchless_clip(d_rate_f, d_max_dev);

	i+=d_osps;
	count += (int)floor(d_sps);
      }

      consume_each(count);
      return i;
    }

  } /* namespace digital */
} /* namespace gr */

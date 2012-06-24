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


#ifndef INCLUDED_PFB_ARB_RESAMPLER_CCF_IMPL_H
#define	INCLUDED_PFB_ARB_RESAMPLER_CCF_IMPL_H

#include <filter/pfb_arb_resampler_ccf.h>
#include <filter/fir_filter.h>
#include <gruel/thread.h>

namespace gr {
  namespace filter {

    class FILTER_API pfb_arb_resampler_ccf_impl : public pfb_arb_resampler_ccf
    {
    private:
      std::vector<kernel::fir_filter_ccf*> d_filters;
      std::vector<kernel::fir_filter_ccf*> d_diff_filters;
      std::vector< std::vector<float> > d_taps;
      std::vector< std::vector<float> > d_dtaps;
      unsigned int d_int_rate;          // the number of filters (interpolation rate)
      unsigned int d_dec_rate;          // the stride through the filters (decimation rate)
      float        d_flt_rate;          // residual rate for the linear interpolation
      float        d_acc;
      unsigned int d_last_filter;
      int          d_start_index;
      unsigned int d_taps_per_filter;
      bool         d_updated;
      gruel::mutex d_mutex; // mutex to protect set/work access

      void create_diff_taps(const std::vector<float> &newtaps,
			    std::vector<float> &difftaps);

      /*!
       * Resets the filterbank's filter taps with the new prototype filter
       * \param newtaps    (vector of floats) The prototype filter to populate the filterbank.
       *                   The taps should be generated at the interpolated sampling rate.
       * \param ourtaps    (vector of floats) Reference to our internal member of holding the taps.
       * \param ourfilter  (vector of filters) Reference to our internal filter to set the taps for.
       */
      void create_taps(const std::vector<float> &newtaps,
		       std::vector< std::vector<float> > &ourtaps,
		       std::vector<kernel::fir_filter_ccf*> &ourfilter);

    public:
      pfb_arb_resampler_ccf_impl(float rate,
				 const std::vector<float> &taps,
				 unsigned int filter_size);

      ~pfb_arb_resampler_ccf_impl();

      void set_taps(const std::vector<float> &taps);
      std::vector<std::vector<float> > taps() const;
      void print_taps();
      void set_rate(float rate);

      void set_phase(float ph);
      float phase() const;

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_PFB_ARB_RESAMPLER_CCF_IMPL_H */

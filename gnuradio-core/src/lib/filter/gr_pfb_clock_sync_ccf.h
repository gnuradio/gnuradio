/* -*- c++ -*- */
/*
 * Copyright 2009,2010 Free Software Foundation, Inc.
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


#ifndef INCLUDED_GR_PFB_CLOCK_SYNC_CCF_H
#define	INCLUDED_GR_PFB_CLOCK_SYNC_CCF_H

#include <gr_block.h>

class gr_pfb_clock_sync_ccf;
typedef boost::shared_ptr<gr_pfb_clock_sync_ccf> gr_pfb_clock_sync_ccf_sptr;
gr_pfb_clock_sync_ccf_sptr gr_make_pfb_clock_sync_ccf (double sps, float gain,
						       const std::vector<float> &taps,
						       unsigned int filter_size=32,
						       float init_phase=0,
						       float max_rate_deviation=1.5);

class gr_fir_ccf;

/*!
 * \class gr_pfb_clock_sync_ccf
 *
 * \brief Timing synchronizer using polyphase filterbanks
 *
 * \ingroup filter_blk
 * 
 */

class gr_pfb_clock_sync_ccf : public gr_block
{
 private:
  /*!
   * Build the polyphase filterbank timing synchronizer.
   */
  friend gr_pfb_clock_sync_ccf_sptr gr_make_pfb_clock_sync_ccf (double sps, float gain,
								const std::vector<float> &taps,
								unsigned int filter_size,
								float init_phase,
								float max_rate_deviation);

  bool			   d_updated;
  double                   d_sps;
  double                   d_sample_num;
  float                    d_alpha;
  float                    d_beta;
  int                      d_nfilters;
  std::vector<gr_fir_ccf*> d_filters;
  std::vector<gr_fir_ccf*> d_diff_filters;
  std::vector< std::vector<float> > d_taps;
  std::vector< std::vector<float> > d_dtaps;
  float                    d_k;
  float                    d_rate;
  float                    d_rate_i;
  float                    d_rate_f;
  float                    d_max_dev;
  int                      d_filtnum;
  int                      d_taps_per_filter;

  /*!
   * Build the polyphase filterbank timing synchronizer.
   */
  gr_pfb_clock_sync_ccf (double sps, float gain,
			 const std::vector<float> &taps,
			 unsigned int filter_size,
			 float init_phase,
			 float max_rate_deviation);
  
  void create_diff_taps(const std::vector<float> &newtaps,
			std::vector<float> &difftaps);

public:
  ~gr_pfb_clock_sync_ccf ();
  
  /*!
   * Resets the filterbank's filter taps with the new prototype filter
   */
  void set_taps (const std::vector<float> &taps,
		 std::vector< std::vector<float> > &ourtaps,
		 std::vector<gr_fir_ccf*> &ourfilter);
  std::vector<float> channel_taps(int channel);
  std::vector<float> diff_channel_taps(int channel);

  /*!
   * Print all of the filterbank taps to screen.
   */
  void print_taps();
  void print_diff_taps();

  void set_alpha(float alpha)
  {
    d_alpha = alpha;
  }
  void set_beta(float beta)
  {
    d_beta = beta;
  }

  void set_max_rate_deviation(float m)
  {
    d_max_dev = m;
  }
  
  bool check_topology(int ninputs, int noutputs);

  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

#endif

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


#ifndef INCLUDED_GR_PFB_CLOCK_SYNC_FFF_H
#define	INCLUDED_GR_PFB_CLOCK_SYNC_FFF_H

#include <gr_core_api.h>
#include <gr_block.h>

class gr_pfb_clock_sync_fff;
typedef boost::shared_ptr<gr_pfb_clock_sync_fff> gr_pfb_clock_sync_fff_sptr;
GR_CORE_API gr_pfb_clock_sync_fff_sptr gr_make_pfb_clock_sync_fff (double sps, float gain,
						       const std::vector<float> &taps,
						       unsigned int filter_size=32,
						       float init_phase=0,
						       float max_rate_deviation=1.5);

class gr_fir_fff;

/*!
 * \class gr_pfb_clock_sync_fff
 *
 * \brief Timing synchronizer using polyphase filterbanks
 *
 * \ingroup filter_blk
 * \ingroup pfb_blk
 * 
 * This block performs timing synchronization for PAM signals by
 * minimizing the derivative of the filtered signal, which in turn
 * maximizes the SNR and minimizes ISI.
 *
 * This approach works by setting up two filterbanks; one filterbank
 * contains the signal's pulse shaping matched filter (such as a root
 * raised cosine filter), where each branch of the filterbank contains
 * a different phase of the filter.  The second filterbank contains
 * the derivatives of the filters in the first filterbank. Thinking of
 * this in the time domain, the first filterbank contains filters that
 * have a sinc shape to them. We want to align the output signal to be
 * sampled at exactly the peak of the sinc shape. The derivative of
 * the sinc contains a zero at the maximum point of the sinc (sinc(0)
 * = 1, sinc(0)' = 0).  Furthermore, the region around the zero point
 * is relatively linear. We make use of this fact to generate the
 * error signal.
 *
 * If the signal out of the derivative filters is d_i[n] for the ith
 * filter, and the output of the matched filter is x_i[n], we
 * calculate the error as: e[n] = (Re{x_i[n]} * Re{d_i[n]} +
 * Im{x_i[n]} * Im{d_i[n]}) / 2.0 This equation averages the error in
 * the real and imaginary parts. There are two reasons we multiply by
 * the signal itself. First, if the symbol could be positive or
 * negative going, but we want the error term to always tell us to go
 * in the same direction depending on which side of the zero point we
 * are on. The sign of x_i[n] adjusts the error term to do
 * this. Second, the magnitude of x_i[n] scales the error term
 * depending on the symbol's amplitude, so larger signals give us a
 * stronger error term because we have more confidence in that
 * symbol's value.  Using the magnitude of x_i[n] instead of just the
 * sign is especially good for signals with low SNR.
 *
 * The error signal, e[n], gives us a value proportional to how far
 * away from the zero point we are in the derivative signal. We want
 * to drive this value to zero, so we set up a second order loop. We
 * have two variables for this loop; d_k is the filter number in the
 * filterbank we are on and d_rate is the rate which we travel through
 * the filters in the steady state. That is, due to the natural clock
 * differences between the transmitter and receiver, d_rate represents
 * that difference and would traverse the filter phase paths to keep
 * the receiver locked. Thinking of this as a second-order PLL, the
 * d_rate is the frequency and d_k is the phase. So we update d_rate
 * and d_k using the standard loop equations based on two error
 * signals, d_alpha and d_beta.  We have these two values set based on
 * each other for a critically damped system, so in the block
 * constructor, we just ask for "gain," which is d_alpha while d_beta
 * is equal to (gain^2)/4.
 *
 * The block's parameters are:
 *
 * \li \p sps: The clock sync block needs to know the number of samples per
 * symbol, because it defaults to return a single point representing
 * the symbol. The sps can be any positive real number and does not
 * need to be an integer.
 *
 * \li \p loop_bw: The loop bandwidth is used to set the gain of the
 * inner control loop (see:
 * http://gnuradio.squarespace.com/blog/2011/8/13/control-loop-gain-values.html).
 * This should be set small (a value of around 2pi/100 is suggested in
 * that blog post as the step size for the number of radians around
 * the unit circle to move relative to the error).
 *
 * \li \p taps: One of the most important parameters for this block is
 * the taps of the filter. One of the benefits of this algorithm is
 * that you can put the matched filter in here as the taps, so you get
 * both the matched filter and sample timing correction in one go. So
 * create your normal matched filter. For a typical digital
 * modulation, this is a root raised cosine filter. The number of taps
 * of this filter is based on how long you expect the channel to be;
 * that is, how many symbols do you want to combine to get the current
 * symbols energy back (there's probably a better way of stating
 * that). It's usually 5 to 10 or so. That gives you your filter, but
 * now we need to think about it as a filter with different phase
 * profiles in each filter. So take this number of taps and multiply
 * it by the number of filters. This is the number you would use to
 * create your prototype filter. When you use this in the PFB
 * filerbank, it segments these taps into the filterbanks in such a
 * way that each bank now represents the filter at different phases,
 * equally spaced at 2pi/N, where N is the number of filters.
 *
 * \li \p filter_size (default=32): The number of filters can also be
 * set and defaults to 32. With 32 filters, you get a good enough
 * resolution in the phase to produce very small, almost unnoticeable,
 * ISI.  Going to 64 filters can reduce this more, but after that
 * there is very little gained for the extra complexity.
 *
 * \li \p init_phase (default=0): The initial phase is another
 * settable parameter and refers to the filter path the algorithm
 * initially looks at (i.e., d_k starts at init_phase). This value
 * defaults to zero, but it might be useful to start at a different
 * phase offset, such as the mid-point of the filters.
 *
 * \li \p max_rate_deviation (default=1.5): The next parameter is the
 * max_rate_devitation, which defaults to 1.5. This is how far we
 * allow d_rate to swing, positive or negative, from 0. Constraining
 * the rate can help keep the algorithm from walking too far away to
 * lock during times when there is no signal.
 *
 * \li \p osps: note that unlike the ccf version of this algorithm,
 * this block does \a not have a setting for the number of output
 * samples per symbol. This is mostly because it should not be
 * necessary as the reason for having multiple output sps is to
 * perform equalization and the equalizers will take in complex
 * numbers in order to do magnitude and phase correction.
 */

class GR_CORE_API gr_pfb_clock_sync_fff : public gr_block
{
 private:
  /*!
   * Build the polyphase filterbank timing synchronizer.
   * \param sps (double) The number of samples per second in the incoming signal
   * \param gain (float) The alpha gain of the control loop; beta = (gain^2)/4 by default.
   * \param taps (vector<int>) The filter taps.
   * \param filter_size (uint) The number of filters in the filterbank (default = 32).
   * \param init_phase (float) The initial phase to look at, or which filter to start 
   *                           with (default = 0).
   * \param max_rate_deviation (float) Distance from 0 d_rate can get (default = 1.5).
   *
   */
  friend GR_CORE_API gr_pfb_clock_sync_fff_sptr gr_make_pfb_clock_sync_fff (double sps, float gain,
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
  std::vector<gr_fir_fff*> d_filters;
  std::vector<gr_fir_fff*> d_diff_filters;
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
  gr_pfb_clock_sync_fff (double sps, float gain,
			 const std::vector<float> &taps,
			 unsigned int filter_size,
			 float init_phase,
			 float max_rate_deviation);
  
  void create_diff_taps(const std::vector<float> &newtaps,
			std::vector<float> &difftaps);

public:
  ~gr_pfb_clock_sync_fff ();
  
  /*!
   * Resets the filterbank's filter taps with the new prototype filter
   */
  void set_taps (const std::vector<float> &taps,
		 std::vector< std::vector<float> > &ourtaps,
		 std::vector<gr_fir_fff*> &ourfilter);

  /*!
   * Returns the taps of the matched filter
   */
  std::vector<float> channel_taps(int channel);

  /*!
   * Returns the taps in the derivative filter
   */
  std::vector<float> diff_channel_taps(int channel);

  /*!
   * Print all of the filterbank taps to screen.
   */
  void print_taps();

  /*!
   * Print all of the filterbank taps of the derivative filter to screen.
   */
  void print_diff_taps();

  /*!
   * Set the gain value alpha for the control loop
   */  
  void set_alpha(float alpha)
  {
    d_alpha = alpha;
  }

  /*!
   * Set the gain value beta for the control loop
   */  
  void set_beta(float beta)
  {
    d_beta = beta;
  }

  /*!
   * Set the maximum deviation from 0 d_rate can have
   */  
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

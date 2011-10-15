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


#ifndef INCLUDED_GR_PFB_DECIMATOR_CCF_H
#define	INCLUDED_GR_PFB_DECIMATOR_CCF_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_pfb_decimator_ccf;
typedef boost::shared_ptr<gr_pfb_decimator_ccf> gr_pfb_decimator_ccf_sptr;
GR_CORE_API gr_pfb_decimator_ccf_sptr gr_make_pfb_decimator_ccf (unsigned int decim, 
						     const std::vector<float> &taps,
						     unsigned int channel=0);

class gr_fir_ccf;
class gri_fft_complex;

/*!
 * \class gr_pfb_decimator_ccf
 * \brief Polyphase filterbank bandpass decimator with gr_complex 
 *        input, gr_complex output and float taps
 *
 * \ingroup filter_blk
 * \ingroup pfb_blk
 * 
 * This block takes in a signal stream and performs interger down-
 * sampling (decimation) with a polyphase filterbank. The first input
 * is the integer specifying how much to decimate by. The second
 * input is a vector (Python list) of floating-point taps of the 
 * prototype filter. The third input specifies the channel to extract.
 * By default, the zeroth channel is used, which is the baseband 
 * channel (first Nyquist zone).
 *
 * The <EM>channel</EM> parameter specifies which channel to use since
 * this class is capable of bandpass decimation. Given a complex input
 * stream at a sampling rate of <EM>fs</EM> and a decimation rate of
 * <EM>decim</EM>, the input frequency domain is split into 
 * <EM>decim</EM> channels that represent the Nyquist zones. Using the
 * polyphase filterbank, we can select any one of these channels to
 * decimate.
 *
 * The output signal will be the basebanded and decimated signal from
 * that channel. This concept is very similar to the PFB channelizer
 * (see #gr_pfb_channelizer_ccf) where only a single channel is 
 * extracted at a time.
 *
 * The filter's taps should be based on the sampling rate before
 * decimation.
 *
 * For example, using the GNU Radio's firdes utility to building
 * filters, we build a low-pass filter with a sampling rate of 
 * <EM>fs</EM>, a 3-dB bandwidth of <EM>BW</EM> and a transition
 * bandwidth of <EM>TB</EM>. We can also specify the out-of-band
 * attenuation to use, <EM>ATT</EM>, and the filter window
 * function (a Blackman-harris window in this case). The first input
 *  is the gain of the filter, which we specify here as unity.
 *
 *      <B><EM>self._taps = gr.firdes.low_pass_2(1, fs, BW, TB, 
 *           attenuation_dB=ATT, window=gr.firdes.WIN_BLACKMAN_hARRIS)</EM></B>
 *
 * The PFB decimator code takes the taps generated above and builds a
 * set of filters. The set contains <EM>decim</EM> number of filters
 * and each filter contains ceil(taps.size()/decim) number of taps.
 * Each tap from the filter prototype is sequentially inserted into
 * the next filter. When all of the input taps are used, the remaining
 * filters in the filterbank are filled out with 0's to make sure each 
 * filter has the same number of taps.
 *
 * The theory behind this block can be found in Chapter 6 of 
 * the following book.
 *
 *    <B><EM>f. harris, "Multirate Signal Processing for Communication 
 *       Systems," Upper Saddle River, NJ: Prentice Hall, Inc. 2004.</EM></B>
 */

class GR_CORE_API gr_pfb_decimator_ccf : public gr_sync_block
{
 private:
  /*!
   * Build the polyphase filterbank decimator.
   * \param decim   (unsigned integer) Specifies the decimation rate to use
   * \param taps    (vector/list of floats) The prototype filter to populate the filterbank.
   * \param channel (unsigned integer) Selects the channel to return [default=0].
   */
  friend GR_CORE_API gr_pfb_decimator_ccf_sptr gr_make_pfb_decimator_ccf (unsigned int decim,
							      const std::vector<float> &taps,
							      unsigned int channel);

  std::vector<gr_fir_ccf*> d_filters;
  std::vector< std::vector<float> > d_taps;
  gri_fft_complex         *d_fft;
  unsigned int             d_rate;
  unsigned int             d_chan;
  unsigned int             d_taps_per_filter;
  bool			   d_updated;
  gr_complex              *d_rotator;

  /*!
   * Build the polyphase filterbank decimator.
   * \param decim   (unsigned integer) Specifies the decimation rate to use
   * \param taps    (vector/list of floats) The prototype filter to populate the filterbank.
   * \param channel (unsigned integer) Selects the channel to return [default=0].
   */
  gr_pfb_decimator_ccf (unsigned int decim, 
			const std::vector<float> &taps,
			unsigned int channel);

public:
  ~gr_pfb_decimator_ccf ();
  
  /*!
   * Resets the filterbank's filter taps with the new prototype filter
   * \param taps    (vector/list of floats) The prototype filter to populate the filterbank.
   */
   void set_taps (const std::vector<float> &taps);

  /*!
   * Print all of the filterbank taps to screen.
   */
  void print_taps();
   
 //void set_channel (unsigned int channel);

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif

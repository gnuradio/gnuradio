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


#ifndef INCLUDED_GR_PFB_CHANNELIZER_CCF_H
#define	INCLUDED_GR_PFB_CHANNELIZER_CCF_H

#include <gr_core_api.h>
#include <gr_block.h>

class gr_pfb_channelizer_ccf;
typedef boost::shared_ptr<gr_pfb_channelizer_ccf> gr_pfb_channelizer_ccf_sptr;
GR_CORE_API gr_pfb_channelizer_ccf_sptr gr_make_pfb_channelizer_ccf (unsigned int numchans, 
							 const std::vector<float> &taps,
							 float oversample_rate=1);

class gr_fir_ccf;
class gri_fft_complex;


/*!
 * \class gr_pfb_channelizer_ccf
 *
 * \brief Polyphase filterbank channelizer with 
 *        gr_complex input, gr_complex output and float taps
 *
 * \ingroup filter_blk
 * \ingroup pfb_blk
 *
 * This block takes in complex inputs and channelizes it to <EM>M</EM>
 * channels of equal bandwidth. Each of the resulting channels is
 * decimated to the new rate that is the input sampling rate
 * <EM>fs</EM> divided by the number of channels, <EM>M</EM>.
 *
 * The PFB channelizer code takes the taps generated above and builds
 * a set of filters. The set contains <EM>M</EM> number of filters
 * and each filter contains ceil(taps.size()/decim) number of taps.
 * Each tap from the filter prototype is sequentially inserted into
 * the next filter. When all of the input taps are used, the remaining
 * filters in the filterbank are filled out with 0's to make sure each
 * filter has the same number of taps.
 *
 * Each filter operates using the gr_fir filter classs of GNU Radio,
 * which takes the input stream at <EM>i</EM> and performs the inner
 * product calculation to <EM>i+(n-1)</EM> where <EM>n</EM> is the
 * number of filter taps. To efficiently handle this in the GNU Radio
 * structure, each filter input must come from its own input
 * stream. So the channelizer must be provided with <EM>M</EM> streams
 * where the input stream has been deinterleaved. This is most easily
 * done using the gr_stream_to_streams block.
 *
 * The output is then produced as a vector, where index <EM>i</EM> in
 * the vector is the next sample from the <EM>i</EM>th channel. This
 * is most easily handled by sending the output to a
 * gr_vector_to_streams block to handle the conversion and passing
 * <EM>M</EM> streams out.
 *
 * The input and output formatting is done using a hier_block2 called
 * pfb_channelizer_ccf. This can take in a single stream and outputs
 * <EM>M</EM> streams based on the behavior described above.
 *
 * The filter's taps should be based on the input sampling rate.
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
 * The filter output can also be overs ampled. The over sampling rate 
 * is the ratio of the the actual output sampling rate to the normal 
 * output sampling rate. It must be rationally related to the number 
 * of channels as N/i for i in [1,N], which gives an outputsample rate
 * of [fs/N, fs] where fs is the input sample rate and N is the number
 * of channels.
 *
 * For example, for 6 channels with fs = 6000 Hz, the normal rate is 
 * 6000/6 = 1000 Hz. Allowable oversampling rates are 6/6, 6/5, 6/4, 
 * 6/3, 6/2, and 6/1 where the output sample rate of a 6/1 oversample
 * ratio is 6000 Hz, or 6 times the normal 1000 Hz. A rate of 6/5 = 1.2,
 * so the output rate would be 1200 Hz.
 *
 * The theory behind this block can be found in Chapter 6 of 
 * the following book.
 *
 *    <B><EM>f. harris, "Multirate Signal Processing for Communication 
 *       Systems," Upper Saddle River, NJ: Prentice Hall, Inc. 2004.</EM></B>
 *
 */

class GR_CORE_API gr_pfb_channelizer_ccf : public gr_block
{
 private:
  /*!
   * Build the polyphase filterbank decimator.
   * \param numchans (unsigned integer) Specifies the number of channels <EM>M</EM>
   * \param taps    (vector/list of floats) The prototype filter to populate the filterbank.
   * \param oversample_rate (float)   The over sampling rate is the ratio of the the actual
   *                                  output sampling rate to the normal output sampling rate.
   *                                   It must be rationally related to the number of channels
   *				      as N/i for i in [1,N], which gives an outputsample rate 
   *				      of [fs/N, fs] where fs is the input sample rate and N is
   *				      the number of channels.
   *				      
   *				      For example, for 6 channels with fs = 6000 Hz, the normal
   *				      rate is 6000/6 = 1000 Hz. Allowable oversampling rates
   *				      are 6/6, 6/5, 6/4, 6/3, 6/2, and 6/1 where the output
   *				      sample rate of a 6/1 oversample ratio is 6000 Hz, or
   *				      6 times the normal 1000 Hz.
   */
  friend GR_CORE_API gr_pfb_channelizer_ccf_sptr gr_make_pfb_channelizer_ccf (unsigned int numchans,
								  const std::vector<float> &taps,
								  float oversample_rate);

  bool			   d_updated;
  unsigned int             d_numchans;
  float                    d_oversample_rate;
  std::vector<gr_fir_ccf*> d_filters;
  std::vector< std::vector<float> > d_taps;
  unsigned int             d_taps_per_filter;
  gri_fft_complex         *d_fft;
  int                     *d_idxlut;
  int                      d_rate_ratio;
  int                      d_output_multiple;

  /*!
   * Build the polyphase filterbank decimator.
   * \param numchans (unsigned integer) Specifies the number of channels <EM>M</EM>
   * \param taps    (vector/list of floats) The prototype filter to populate the filterbank.
   * \param oversample_rate (float)   The output over sampling rate.
   */
  gr_pfb_channelizer_ccf (unsigned int numchans, 
			  const std::vector<float> &taps,
			  float oversample_rate);

public:
  ~gr_pfb_channelizer_ccf ();
  
  /*!
   * Resets the filterbank's filter taps with the new prototype filter
   * \param taps    (vector/list of floats) The prototype filter to populate the filterbank.
   */
  void set_taps (const std::vector<float> &taps);

  /*!
   * Print all of the filterbank taps to screen.
   */
  void print_taps();
  
  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

#endif

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


#ifndef INCLUDED_GR_PFB_ARB_RESAMPLER_CCF_H
#define	INCLUDED_GR_PFB_ARB_RESAMPLER_CCF_H

#include <gr_core_api.h>
#include <gr_block.h>

class gr_pfb_arb_resampler_ccf;
typedef boost::shared_ptr<gr_pfb_arb_resampler_ccf> gr_pfb_arb_resampler_ccf_sptr;
GR_CORE_API gr_pfb_arb_resampler_ccf_sptr gr_make_pfb_arb_resampler_ccf (float rate,
							     const std::vector<float> &taps,
							     unsigned int filter_size=32);

class gr_fir_ccf;

/*!
 * \class gr_pfb_arb_resampler_ccf
 *
 * \brief Polyphase filterbank arbitrary resampler with 
 *        gr_complex input, gr_complex output and float taps
 *
 * \ingroup filter_blk
 * \ingroup pfb_blk
 * 
 * This block takes in a signal stream and performs arbitrary
 * resampling. The resampling rate can be any real
 * number <EM>r</EM>. The resampling is done by constructing
 * <EM>N</EM> filters where <EM>N</EM> is the interpolation rate.  We
 * then calculate <EM>D</EM> where <EM>D = floor(N/r)</EM>.
 *
 * Using <EM>N</EM> and <EM>D</EM>, we can perform rational resampling
 * where <EM>N/D</EM> is a rational number close to the input rate
 * <EM>r</EM> where we have <EM>N</EM> filters and we cycle through
 * them as a polyphase filterbank with a stride of <EM>D</EM> so that
 * <EM>i+1 = (i + D) % N</EM>.
 *
 * To get the arbitrary rate, we want to interpolate between two
 * points. For each value out, we take an output from the current
 * filter, <EM>i</EM>, and the next filter <EM>i+1</EM> and then
 * linearly interpolate between the two based on the real resampling
 * rate we want.
 *
 * The linear interpolation only provides us with an approximation to
 * the real sampling rate specified. The error is a quantization error
 * between the two filters we used as our interpolation points.  To
 * this end, the number of filters, <EM>N</EM>, used determines the
 * quantization error; the larger <EM>N</EM>, the smaller the
 * noise. You can design for a specified noise floor by setting the
 * filter size (parameters <EM>filter_size</EM>). The size defaults to
 * 32 filters, which is about as good as most implementations need.
 *
 * The trick with designing this filter is in how to specify the taps
 * of the prototype filter. Like the PFB interpolator, the taps are
 * specified using the interpolated filter rate. In this case, that
 * rate is the input sample rate multiplied by the number of filters
 * in the filterbank, which is also the interpolation rate. All other
 * values should be relative to this rate.
 *
 * For example, for a 32-filter arbitrary resampler and using the
 * GNU Radio's firdes utility to build the filter, we build a low-pass
 * filter with a sampling rate of <EM>fs</EM>, a 3-dB bandwidth of
 * <EM>BW</EM> and a transition bandwidth of <EM>TB</EM>. We can also
 * specify the out-of-band attenuation to use, <EM>ATT</EM>, and the
 * filter window function (a Blackman-harris window in this case). The
 * first input is the gain of the filter, which we specify here as the
 * interpolation rate (<EM>32</EM>).
 *
 *      <B><EM>self._taps = gr.firdes.low_pass_2(32, 32*fs, BW, TB, 
 *           attenuation_dB=ATT, window=gr.firdes.WIN_BLACKMAN_hARRIS)</EM></B>
 *
 * The theory behind this block can be found in Chapter 7.5 of 
 * the following book.
 *
 *    <B><EM>f. harris, "Multirate Signal Processing for Communication 
 *       Systems", Upper Saddle River, NJ: Prentice Hall, Inc. 2004.</EM></B>
 */

class GR_CORE_API gr_pfb_arb_resampler_ccf : public gr_block
{
 private:
  /*!
   * Build the polyphase filterbank arbitray resampler.
   * \param rate  (float) Specifies the resampling rate to use
   * \param taps  (vector/list of floats) The prototype filter to populate the filterbank. The taps
   *                                      should be generated at the filter_size sampling rate.
   * \param filter_size (unsigned int) The number of filters in the filter bank. This is directly
                                       related to quantization noise introduced during the resampling.
				       Defaults to 32 filters.
   */
  friend GR_CORE_API gr_pfb_arb_resampler_ccf_sptr gr_make_pfb_arb_resampler_ccf (float rate,
								      const std::vector<float> &taps,
								      unsigned int filter_size);

  std::vector<gr_fir_ccf*> d_filters;
  std::vector<gr_fir_ccf*> d_diff_filters;
  std::vector< std::vector<float> > d_taps;
  std::vector< std::vector<float> > d_dtaps;
  unsigned int             d_int_rate;          // the number of filters (interpolation rate)
  unsigned int             d_dec_rate;          // the stride through the filters (decimation rate)
  float                    d_flt_rate;          // residual rate for the linear interpolation
  float                    d_acc;
  unsigned int             d_last_filter;
  int                      d_start_index;
  unsigned int             d_taps_per_filter;
  bool			   d_updated;

  /*!
   * Build the polyphase filterbank arbitray resampler.
   * \param rate  (float) Specifies the resampling rate to use
   * \param taps  (vector/list of floats) The prototype filter to populate the filterbank. The taps
   *                                      should be generated at the filter_size sampling rate.
   * \param filter_size (unsigned int) The number of filters in the filter bank. This is directly
                                       related to quantization noise introduced during the resampling.
				       Defaults to 32 filters.
   */
  gr_pfb_arb_resampler_ccf (float rate, 
			    const std::vector<float> &taps,
			    unsigned int filter_size);

  void create_diff_taps(const std::vector<float> &newtaps,
			std::vector<float> &difftaps);

  /*!
   * Resets the filterbank's filter taps with the new prototype filter
   * \param newtaps    (vector of floats) The prototype filter to populate the filterbank. 
   *                   The taps should be generated at the interpolated sampling rate.
   * \param ourtaps    (vector of floats) Reference to our internal member of holding the taps.
   * \param ourfilter  (vector of filters) Reference to our internal filter to set the taps for.
   */
  void create_taps (const std::vector<float> &newtaps,
		    std::vector< std::vector<float> > &ourtaps,
		    std::vector<gr_fir_ccf*> &ourfilter);

  
public:
  ~gr_pfb_arb_resampler_ccf ();

  // FIXME: See about a set_taps function during runtime.

  /*!
   * Print all of the filterbank taps to screen.
   */
  void print_taps();
  void set_rate (float rate) { 
    d_dec_rate = (unsigned int)floor(d_int_rate/rate);
    d_flt_rate = (d_int_rate/rate) - d_dec_rate;
    set_relative_rate(rate);
  }

  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

#endif

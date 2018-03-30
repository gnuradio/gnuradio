/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
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


#ifndef INCLUDED_PFB_DECIMATOR_CCF_H
#define	INCLUDED_PFB_DECIMATOR_CCF_H

#include <gnuradio/filter/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace filter {

    /*!
     * \brief Polyphase filterbank bandpass decimator with gr_complex
     *        input, gr_complex output and float taps
     * \ingroup channelizers_blk
     *
     * \details
     * This block takes in a signal stream and performs integer down-
     * sampling (decimation) with a polyphase filterbank. The first
     * input is the integer specifying how much to decimate by. The
     * second input is a vector (Python list) of floating-point taps
     * of the prototype filter. The third input specifies the channel
     * to extract.  By default, the zeroth channel is used, which is
     * the baseband channel (first Nyquist zone).
     *
     * The <EM>channel</EM> parameter specifies which channel to use
     * since this class is capable of bandpass decimation. Given a
     * complex input stream at a sampling rate of <EM>fs</EM> and a
     * decimation rate of <EM>decim</EM>, the input frequency domain
     * is split into <EM>decim</EM> channels that represent the
     * Nyquist zones. Using the polyphase filterbank, we can select
     * any one of these channels to decimate.
     *
     * The output signal will be the basebanded and decimated signal
     * from that channel. This concept is very similar to the PFB
     * channelizer (see #gr::filter::pfb_channelizer_ccf) where only a single
     * channel is extracted at a time.
     *
     * The filter's taps should be based on the sampling rate before
     * decimation.
     *
     * For example, using the GNU Radio's firdes utility to building
     * filters, we build a low-pass filter with a sampling rate of
     * <EM>fs</EM>, a 3-dB bandwidth of <EM>BW</EM> and a transition
     * bandwidth of <EM>TB</EM>. We can also specify the out-of-band
     * attenuation to use, <EM>ATT</EM>, and the filter window
     * function (a Blackman-harris window in this case). The first
     * input is the gain of the filter, which we specify here as
     * unity.
     *
     *   <B><EM>self._taps = filter.firdes.low_pass_2(1, fs, BW, TB,
     *      attenuation_dB=ATT, window=filter.firdes.WIN_BLACKMAN_hARRIS)</EM></B>
     *
     * The PFB decimator code takes the taps generated above and
     * builds a set of filters. The set contains <EM>decim</EM>
     * filters and each filter contains ceil(taps.size()/decim)
     * taps.  Each tap from the filter prototype is
     * sequentially inserted into the next filter. When all of the
     * input taps are used, the remaining filters in the filterbank
     * are filled out with 0's to make sure each filter has the same
     * number of taps.
     *
     * The theory behind this block can be found in Chapter 6 of
     * the following book:
     *
     *   <B><EM>f. harris, "Multirate Signal Processing for Communication
     *      Systems," Upper Saddle River, NJ: Prentice Hall, Inc. 2004.</EM></B>
     */

    class FILTER_API pfb_decimator_ccf : virtual public sync_block
    {
    public:
      // gr::filter::pfb_decimator_ccf::sptr
      typedef boost::shared_ptr<pfb_decimator_ccf> sptr;

      /*!
       * Build the polyphase filterbank decimator.
       * \param decim   (unsigned integer) Specifies the decimation rate to use
       * \param taps    (vector/list of floats) The prototype filter to populate the filterbank.
       * \param channel (unsigned integer) Selects the channel to return [default=0].
       * \param use_fft_rotator (bool) Rotate channels using FFT method instead of exp(phi).
       *                For larger values of \p channel, the FFT method will perform better.
       *                Generally, this value of \p channel is small (~5), but could be
       *                architecture-specific (Default: true).
       * \param use_fft_filters (bool) Use FFT filters (fast convolution) instead of FIR filters.
       *                FFT filters perform better for larger numbers of taps but is
       *                architecture-specific (Default: true).
       */
      static sptr make(unsigned int decim,
                       const std::vector<float> &taps,
                       unsigned int channel,
                       bool use_fft_rotator=true,
                       bool use_fft_filters=true);

      /*!
       * Resets the filterbank's filter taps with the new prototype filter
       * \param taps    (vector/list of floats) The prototype filter to populate the filterbank.
       */
      virtual void set_taps(const std::vector<float> &taps) = 0;

      /*!
       * Return a vector<vector<>> of the filterbank taps
       */
      virtual std::vector<std::vector<float> > taps() const = 0;

      /*!
       * Print all of the filterbank taps to screen.
       */
      virtual void print_taps() = 0;

      virtual void set_channel(const unsigned int channel) = 0;
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_PFB_DECIMATOR_CCF_H */

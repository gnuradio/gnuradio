/* -*- c++ -*- */
/*
 * Copyright 2010,2012 Free Software Foundation, Inc.
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


#ifndef INCLUDED_PFB_SYNTHESIZER_CCF_H
#define	INCLUDED_PFB_SYNTHESIZER_CCF_H

#include <gnuradio/filter/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
  namespace filter {

    /*!
     * \brief Polyphase synthesis filterbank with
     *        gr_complex input, gr_complex output and float taps
     * \ingroup channelizers_blk
     *
     * \details
     *
     * The PFB synthesis filterbank combines multiple baseband signals
     * into a single channelized signal. Each input stream is,
     * essentially, modulated onto an output channel according the the
     * channel mapping (see set_channel_map for details).
     *
     * Setting this filterbank up means selecting the number of output
     * channels, the prototype filter, and whether to handle channels
     * at 2x the sample rate (this is generally used only for
     * reconstruction filtering).
     *
     * The number of channels sets the maximum number of channels to
     * use, but not all input streams must be connected. For M total
     * channels, we can connect inputs 0 to N where N < M-1. Because
     * of the way GNU Radio handles stream connections, we must
     * connect the channels consecutively, and so we must use the
     * set_channel_map if the desired output channels are not the same
     * as the the default mapping. This features gives us the
     * flexibility to output to any given channel. Generally, we try
     * to not use the channels at the edge of the spectrum to avoid
     * issues with filtering and roll-off of the transmitter or
     * receiver.
     *
     * When using the 2x sample rate mode, we specify the number of
     * channels that will be used. However, the actual output signal
     * will be twice this number of channels. This is mainly important
     * to know when setting the channel map. For M channels, the
     * channel mapping can specy from 0 to 2M-1 channels to output
     * onto.
     *
     * For more details about this and the concepts of reconstruction
     * filtering, see:
     *
     *    <B><EM>f. harris, "Multirate Signal Processing for Communication
     *       Systems," Upper Saddle River, NJ: Prentice Hall, Inc. 2004.</EM></B>
     *
     *    <B><EM>X. Chen, E. Venosa, and fred harris, “Polyphase analysis
     *       filter bank up-converts unequal channel bandwidths with
     *       arbitrary center frequencies - design ii,” in SDR’10-WinnComm,
     *       2010.</EM></B>
     *
     *    <B><EM>E. Venosa, X. Chen, and fred harris, “Polyphase analysis
     *       filter bank down-converts unequal channel bandwidths with
     *       arbitrary center frequencies - design I,” in SDR’10-WinnComm,
     *       2010.</EM></B>
     *
     *    <B><EM>f. j. harris, C. Dick, X. Chen, and E. Venosa, “Wideband 160-
     *       channel polyphase filter bank cable TV channeliser,” in IET
     *       Signal Processing, 2010.</EM></B>
     */
    class FILTER_API pfb_synthesizer_ccf : virtual public sync_interpolator
    {
    public:
      // gr::filter::pfb_synthesizer_ccf::sptr
      typedef boost::shared_ptr<pfb_synthesizer_ccf> sptr;

      /*!
       * Build the polyphase synthesis filterbank.
       * \param numchans (unsigned integer) Specifies the number of
       *                 channels <EM>M</EM>
       * \param taps    (vector/list of floats) The prototype filter to
       *                populate the filterbank.
       * \param twox    (bool) use 2x oversampling or not (default is no)
       */
      static sptr make(unsigned int numchans,
                       const std::vector<float> &taps,
                       bool twox=false);

      /*!
       * Resets the filterbank's filter taps with the new prototype filter
       * \param taps    (vector/list of floats) The prototype filter to
       *                populate the filterbank.
       */
      virtual void set_taps(const std::vector<float> &taps) = 0;

      /*!
       * Print all of the filterbank taps to screen.
       */
      virtual void print_taps() = 0;

      /*!
       * Return a vector<vector<>> of the filterbank taps
       */
      virtual std::vector<std::vector<float> > taps() const = 0;

      /*!
       * Set the channel map. Channels are numbers as:
       * <pre>
       *  N/2+1 | ... | N-1 | 0 | 1 |  2 | ... | N/2
       * <------------------- 0 -------------------->
       *                    freq
       * </pre>
       *
       * So input stream 0 goes to channel 0, etc. Setting a new channel
       * map allows the user to specify where in frequency he/she wants
       * the input stream to go. This is especially useful to avoid
       * putting signals into the channels on the edge of the spectrum
       * which can either wrap around (in the case of odd number of
       * channels) and be affected by filter rolloff in the transmitter.
       *
       * The map must be at least the number of streams being sent to the
       * block. Less and the algorithm will not have enough data to
       * properly setup the buffers. Any more channels specified will be
       * ignored.
       */
      virtual void set_channel_map(const std::vector<int> &map) = 0;

      /*!
       * Gets the current channel map.
       */
      virtual std::vector<int> channel_map() const = 0;
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_PFB_SYNTHESIZER_CCF_H */

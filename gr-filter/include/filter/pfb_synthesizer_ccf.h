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

#include <filter/api.h>
#include <gr_sync_interpolator.h>

namespace gr {
  namespace filter {

    /*!
     * \class pfb_synthesizer_ccf
     *
     * \brief Polyphase synthesis filterbank with
     *        gr_complex input, gr_complex output and float taps
     *
     * \ingroup filter_blk
     * \ingroup pfb_blk
     */

    class FILTER_API pfb_synthesizer_ccf : virtual public gr_sync_interpolator
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
       *     N/2+1 | ... | N-1 | 0 | 1 |  2 | ... | N/2
       *    <------------------- 0 -------------------->
       *                        freq
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

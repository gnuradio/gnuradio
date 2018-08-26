/* -*- c++ -*- */
/*
 * Copyright 2013, 2018 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_OFDM_CYCLIC_PREFIXER_H
#define INCLUDED_DIGITAL_OFDM_CYCLIC_PREFIXER_H

#include <gnuradio/digital/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Adds a cyclic prefix and performs optional pulse shaping on OFDM symbols.
     * \ingroup ofdm_blk
     *
     * \details
     * Input: OFDM symbols (in the time domain, i.e. after the IFFT). Optionally,
     *        entire frames can be processed. In this case, \p len_tag_key must be
     *        specified which holds the key of the tag that denotes how
     *        many OFDM symbols are in a frame.
     * Output: A stream of (scalar) complex symbols, which include the cyclic prefix
     *         and the pulse shaping.
     *         Note: If complete frames are processed, and \p rolloff_len is greater
     *         than zero, the final OFDM symbol is followed by the delay line of
     *         the pulse shaping.
     *
     * The pulse shape is a raised cosine in the time domain.
     *
     * Different CP lengths as for instance needed in LTE are supported. This
     * is why one of the inputs is std::vector<int>. After every CP given has
     * been prepended to symbols, each with the length of the IFFT operation,
     * the mechanism will wrap around and start over. To give an example, the
     * input tuple for LTE with an FFT length of 2048 would be (160,) +
     * (144,)*6, which is equal to (160, 144, 144, 144, 144, 144, 144). A
     * uniform CP would be indicated by (uniform_cp_length, ).
     *
     * This block does some sanity checking: 1. It is not allowed to have a
     * vector of CP lengths, which are only 0.  2. Not a single CP in the
     * vector must be longer than the rolloff. 3. Not a single CP is allowed to
     * be < 0.
     */

    class DIGITAL_API ofdm_cyclic_prefixer : virtual public tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<ofdm_cyclic_prefixer> sptr;

      /*! \brief Make is overloaded to maintain API backwardscompatibility.
       *         This will map one CP length to a vector of length 1.
       * \param input_size IFFT length (i.e. length of the OFDM symbols).
       * \param output_size FFT length + cyclic prefix length (in samples).
       * \param rolloff_len Length of the rolloff flank in samples.
       * \param len_tag_key For framed processing the key of the length tag.
       */
      static sptr make
      (
        size_t input_size,
        size_t output_size,
        int rolloff_len=0,
        const std::string &len_tag_key=""
      );

      /*! \brief This interface should be used for new applications.
       * \param fft_len IFFT length (i.e. length of the OFDM symbols).
       * \param cp_lengths CP lengths. Wraps arround after reaching the end.
       * \param rolloff_len Length of the rolloff flank in samples.
       * \param len_tag_key For framed processing the key of the length tag.
       */
      static sptr make
      (
        int fft_len,
        std::vector<int> cp_lengths,
        int rolloff_len=0,
        const std::string &len_tag_key=""
      );
    };
  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_OFDM_CYCLIC_PREFIXER_H */

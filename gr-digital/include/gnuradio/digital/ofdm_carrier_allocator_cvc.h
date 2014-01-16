/* -*- c++ -*- */
/* 
 * Copyright 2013 Free Software Foundation, Inc.
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


#ifndef INCLUDED_DIGITAL_OFDM_CARRIER_ALLOCATOR_CVC_H
#define INCLUDED_DIGITAL_OFDM_CARRIER_ALLOCATOR_CVC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Create frequency domain OFDM symbols from complex values, add pilots.
     * \ingroup ofdm_blk
     *
     * \details
     * This block turns a stream of complex, scalar modulation symbols into vectors
     * which are the input for an IFFT in an OFDM transmitter. It also supports the
     * possibility of placing pilot symbols onto the carriers.
     *
     * The carriers can be allocated freely, if a carrier is not allocated, it is set
     * to zero. This allows doing OFDMA-style carrier allocations.
     *
     * Input: A tagged stream of complex scalars. The first item must have a tag
     *        containing the number of complex symbols in this frame.
     * Output: A tagged stream of complex vectors of length fft_len. This can directly
     *         be connected to an FFT block. Make sure to set this block to 'reverse'
     *         for the IFFT. If \p output_is_shifted is true, the FFT block must activate
     *         FFT shifting, otherwise, set shifting to false. If given, sync words are
     *         prepended to the output. Note that sync words are prepended verbatim,
     *         make sure they are shifted (or not).
     *
     * Carrier indexes are always such that index 0 is the DC carrier (note: you should
     * not allocate this carrier). The carriers below the DC carrier are either indexed
     * with negative numbers, or with indexes larger than \p fft_len/2. Index -1 and index
     * \p fft_len-1 both identify the carrier below the DC carrier.
     *
     * There are some basic checks in place during initialization which check that the
     * carrier allocation table is valid. However, it is possible to overwrite data symbols
     * with pilot symbols, or provide a carrier allocation that has mismatching pilot symbol
     * positions and -values.
     *
     * Tags are propagated such that a tag on an incoming complex symbol is mapped to the
     * corresponding OFDM symbol. There is one exception: If a tag is on the first OFDM
     * symbol, it is assumed that this tag should stay there, so it is moved to the front
     * even if a sync word is included (any other tags will never be attached to the
     * sync word). This allows tags to control the transmit timing to pass through in the
     * correct position.
     */
    class DIGITAL_API ofdm_carrier_allocator_cvc : virtual public tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<ofdm_carrier_allocator_cvc> sptr;

      virtual std::string len_tag_key() = 0;
      virtual const int fft_len() = 0;
      virtual std::vector<std::vector<int> > occupied_carriers() = 0;

      /*
       * \param fft_len FFT length, is also the maximum width of the OFDM symbols, the
       *                output vector size and maximum value for elements in
       *                \p occupied_carriers and \p pilot_carriers.
       * \param occupied_carriers A vector of vectors of indexes. Example: if
       *                          occupied_carriers = ((1, 2, 3), (1, 2, 4)), the first
       *                          three input symbols will be mapped to carriers 1, 2
       *                          and 3. After that, a new OFDM symbol is started. The next
       *                          three input symbols will be placed onto carriers 1, 2
       *                          and 4 of the second OFDM symbol. The allocation then
       *                          starts from the beginning.
       *                          Order matters! The first input symbol is always mapped
       *                          onto occupied_carriers[0][0].
       * \param pilot_carriers The position of the pilot symbols. Same as occupied_carriers,
       *                       but the actual symbols are taken from pilot_symbols instead
       *                       of the input stream.
       * \param pilot_symbols The pilot symbols which are placed onto the pilot carriers.
       *                      pilot_symbols[0][0] is placed onto the first OFDM symbol, on
       *                      carrier index pilot_carriers[0][0] etc.
       * \param sync_words OFDM symbols that are prepended to the OFDM frame (usually for
       *                   synchronisation purposes, e.g. OFDM symbols with every second
       *                   sub-carrier being idle). Is a vector of complex vectors of length
       *                   \p fft_len
       * \param len_tag_key The key of the tag identifying the length of the input packet.
       */
      static sptr make(
	  int fft_len,
	  const std::vector<std::vector<int> > &occupied_carriers,
	  const std::vector<std::vector<int> > &pilot_carriers,
	  const std::vector<std::vector<gr_complex> > &pilot_symbols,
	  const std::vector<std::vector<gr_complex> > &sync_words,
	  const std::string &len_tag_key = "packet_len",
	  const bool output_is_shifted=true);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_OFDM_CARRIER_ALLOCATOR_CVC_H */


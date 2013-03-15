/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
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

#include <digital/api.h>
#include <gr_tagged_stream_block.h>

class digital_ofdm_carrier_allocator_cvc;
typedef boost::shared_ptr<digital_ofdm_carrier_allocator_cvc> digital_ofdm_carrier_allocator_cvc_sptr;

/*
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
 * \param len_tag_key The key of the tag identifying the length of the input packet.
 */
DIGITAL_API digital_ofdm_carrier_allocator_cvc_sptr
digital_make_ofdm_carrier_allocator_cvc (
		int fft_len,
		const std::vector<std::vector<int> > &occupied_carriers,
		const std::vector<std::vector<int> > &pilot_carriers,
		const std::vector<std::vector<gr_complex> > &pilot_symbols,
		const std::string &len_tag_key = "packet_len");

/*!
 * \brief Create frequency domain OFDM symbols from complex values, add pilots.
 * \ingroup ofdm_blk
 *
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
 *         for the IFFT and to deactivate FFT shifting.
 *
 * Carrier indexes are always such that index 0 is the DC carrier (note: you should
 * not allocate this carrier). The carriers below the DC carrier are either indexed
 * with negative numbers, or with indexes larger than fft_len/2. Index -1 and index
 * fft_len-1 both identify the carrier below the DC carrier.
 *
 */
class DIGITAL_API digital_ofdm_carrier_allocator_cvc : public gr_tagged_stream_block
{
 private:
  friend DIGITAL_API digital_ofdm_carrier_allocator_cvc_sptr digital_make_ofdm_carrier_allocator_cvc (int fft_len, const std::vector<std::vector<int> > &occupied_carriers, const std::vector<std::vector<int> > &pilot_carriers, const std::vector<std::vector<gr_complex> > &pilot_symbols, const std::string &len_tag_key);

  digital_ofdm_carrier_allocator_cvc(int fft_len, const std::vector<std::vector<int> > &occupied_carriers, const std::vector<std::vector<int> > &pilot_carriers, const std::vector<std::vector<gr_complex> > &pilot_symbols, const std::string &len_tag_key);

  //! FFT length
  const int d_fft_len;
  //! Which carriers/symbols carry data
  std::vector<std::vector<int> > d_occupied_carriers;
  //! Which carriers/symbols carry pilots symbols
  std::vector<std::vector<int> > d_pilot_carriers;
  //! Value of said pilot symbols
  const std::vector<std::vector<gr_complex> > d_pilot_symbols;
  int d_symbols_per_set;

 protected:
  int calculate_output_stream_length(const gr_vector_int &ninput_items);

 public:
  ~digital_ofdm_carrier_allocator_cvc();

  std::string len_tag_key() { return d_length_tag_key_str; };

  const int fft_len() { return d_fft_len; };
  std::vector<std::vector<int> > occupied_carriers() { return d_occupied_carriers; };

  int work (int noutput_items,
      gr_vector_int &ninput_items,
      gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items);
};

#endif /* INCLUDED_DIGITAL_OFDM_CARRIER_ALLOCATOR_CVC_H */


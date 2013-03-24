/* -*- c++ -*- */
/*
 * Copyright 2004-2006,2011 Free Software Foundation, Inc.
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

#include <digital_api.h>
#include <gr_tagged_stream_block.h>

class digital_ofdm_cyclic_prefixer;
typedef boost::shared_ptr<digital_ofdm_cyclic_prefixer> digital_ofdm_cyclic_prefixer_sptr;

/*!
 * \param input_size FFT length (i.e. length of the OFDM symbols)
 * \param output_size FFT length + cyclic prefix length (in samples)
 * \param rolloff_len Length of the rolloff flank in samples
 * \param len_tag_key For framed processing the key of the length tag
 */
DIGITAL_API digital_ofdm_cyclic_prefixer_sptr
digital_make_ofdm_cyclic_prefixer (size_t input_size,
		size_t output_size,
		int rolloff_len=0,
		const std::string &len_tag_key="");


/*!
 * \brief Adds a cyclic prefix and performs pulse shaping on OFDM symbols.
 * \ingroup ofdm_blk
 *
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
 */
class DIGITAL_API digital_ofdm_cyclic_prefixer : public gr_tagged_stream_block
{
  friend DIGITAL_API digital_ofdm_cyclic_prefixer_sptr
    digital_make_ofdm_cyclic_prefixer (size_t input_size, size_t output_size, int rolloff_len, const std::string &len_tag_key);


 protected:
  digital_ofdm_cyclic_prefixer (size_t input_size, size_t output_size, int rolloff_len, const std::string &len_tag_key);

  //! Return the number of complex samples from the number of OFDM symbols (includes rolloff)
  int calculate_output_stream_length(const gr_vector_int &ninput_items);

 public:
  int work(int noutput_items,
	       gr_vector_int &ninput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

 private:
  size_t d_fft_len;
  //! FFT length + CP length in samples
  size_t d_output_size;
  //! Length of the cyclic prefix in samples
  int d_cp_size;
  //! Length of pulse rolloff in samples
  int d_rolloff_len;
  //! Buffers the up-flank (at the beginning of the cyclic prefix)
  std::vector<float> d_up_flank;
  //! Buffers the down-flank (which trails the symbol)
  std::vector<float> d_down_flank;
  std::vector<gr_complex> d_delay_line; // We do this explicitly to avoid outputting zeroes (i.e. no history!)
};

#endif /* INCLUDED_DIGITAL_OFDM_CYCLIC_PREFIXER_H */


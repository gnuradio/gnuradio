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

GR_SWIG_BLOCK_MAGIC(digital, ofdm_carrier_allocator_cvc);

digital_ofdm_carrier_allocator_cvc_sptr
digital_make_ofdm_carrier_allocator_cvc (
		int fft_len,
		const std::vector<std::vector<int> > &occupied_carriers,
		const std::vector<std::vector<int> > &pilot_carriers,
		const std::vector<std::vector<gr_complex> > &pilot_symbols,
		const std::string &len_tag_key = "packet_len");

class digital_ofdm_carrier_allocator_cvc : public gr_tagged_stream_block
{
 private:
  digital_ofdm_carrier_allocator_cvc(int fft_len, const std::vector<std::vector<int> > &occupied_carriers, const std::vector<std::vector<int> > &pilot_carriers, const std::vector<std::vector<gr_complex> > &pilot_symbols, const std::string &len_tag_key);
};


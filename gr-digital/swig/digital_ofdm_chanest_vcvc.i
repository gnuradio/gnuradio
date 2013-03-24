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

GR_SWIG_BLOCK_MAGIC(digital, ofdm_chanest_vcvc);

digital_ofdm_chanest_vcvc_sptr
digital_make_ofdm_chanest_vcvc (
		const std::vector<gr_complex> &sync_symbol1,
		const std::vector<gr_complex> &sync_symbol2,
		int n_data_symbols,
		int eq_noise_red_len=0,
		int max_carr_offset=-1,
		bool force_one_sync_symbol=false);

class digital_ofdm_chanest_vcvc : public gr_block
{
 private:
  digital_ofdm_chanest_vcvc(const std::vector<gr_complex> &sync_symbol1, const std::vector<gr_complex> &sync_symbol2, int n_data_symbols, int eq_noise_red_len, int max_carr_offset, bool force_one_sync_symbol);

 public:
};


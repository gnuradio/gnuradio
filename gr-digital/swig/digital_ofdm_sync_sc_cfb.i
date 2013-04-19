/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(digital, ofdm_sync_sc_cfb)

digital_ofdm_sync_sc_cfb_sptr
digital_make_ofdm_sync_sc_cfb (int fft_len, int cp_len, bool use_even_carriers=false);

class digital_ofdm_sync_sc_cfb : public gr_hier_block2
{
 private:
  digital_ofdm_sync_sc_cfb(int fft_len, int cp_len, bool use_even_carriers);
};


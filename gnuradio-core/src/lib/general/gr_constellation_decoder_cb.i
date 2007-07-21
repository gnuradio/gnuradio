/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(gr,constellation_decoder_cb)

gr_constellation_decoder_cb_sptr 
	gr_make_constellation_decoder_cb (const std::vector<gr_complex> &sym_position, 
                                          const std::vector<unsigned char> &sym_value_out);

class gr_constellation_decoder_cb : public gr_sync_block
{
 private:
  gr_constellation_decoder_cb (const std::vector<gr_complex> &sym_position, 
                               const std::vector<unsigned char> &sym_value_out);

  friend gr_constellation_decoder_cb_sptr 
  	gr_make_constellation_decoder_cb (const std::vector<gr_complex> &sym_position, 	
                                          const std::vector<unsigned char> &sym_value_out);

 public:
  int set_constellation(const std::vector<gr_complex> &sym_position, 
                        const std::vector<unsigned char> &sym_value_out);
  ~gr_constellation_decoder_cb();
};

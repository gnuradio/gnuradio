/* -*- c++ -*- */
/*
 * Copyright 2006, 2011 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(digital,constellation_decoder_cb)

digital_constellation_decoder_cb_sptr 
digital_make_constellation_decoder_cb (digital_constellation_sptr constellation);

class digital_constellation_decoder_cb : public gr_sync_block
{
 private:
  digital_constellation_decoder_cb (digital_constellation_sptr constellation);

  friend digital_constellation_decoder_cb_sptr 
  	gr_make_constellation_decoder_cb (digital_constellation_sptr constellation);

 public:
  ~digital_constellation_decoder_cb();
};

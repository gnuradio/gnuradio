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

GR_SWIG_BLOCK_MAGIC(digital,ofdm_cyclic_prefixer);

digital_ofdm_cyclic_prefixer_sptr
digital_make_ofdm_cyclic_prefixer (size_t input_size,
		size_t output_size,
		int rolloff_len=0,
		const std::string &len_tag_key="");


class digital_ofdm_cyclic_prefixer : public gr_tagged_stream_block
{
};


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

#ifndef INCLUDED_GR_UNPACK_K_BITS_BB_H
#define	INCLUDED_GR_UNPACK_K_BITS_BB_H

#include <gr_core_api.h>
#include <gr_sync_interpolator.h>

class gr_unpack_k_bits_bb;
typedef boost::shared_ptr<gr_unpack_k_bits_bb> gr_unpack_k_bits_bb_sptr;
GR_CORE_API gr_unpack_k_bits_bb_sptr gr_make_unpack_k_bits_bb (unsigned k);

class gr_unpack_k_bits_bb;

/*!
 * \brief Converts a byte with k relevent bits to k output bytes with 1 bit in the LSB.
 * \ingroup converter_blk
 */
class GR_CORE_API gr_unpack_k_bits_bb : public gr_sync_interpolator
{
 private:
  friend GR_CORE_API gr_unpack_k_bits_bb_sptr gr_make_unpack_k_bits_bb (unsigned k);

  gr_unpack_k_bits_bb (unsigned k);

  unsigned d_k;    // number of relevent bits to unpack into k output bytes
  
 public:
  ~gr_unpack_k_bits_bb ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif

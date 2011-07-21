/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_SCRAMBLER_BB_H
#define INCLUDED_GR_SCRAMBLER_BB_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include "gri_lfsr.h"

class gr_scrambler_bb;
typedef boost::shared_ptr<gr_scrambler_bb> gr_scrambler_bb_sptr;

GR_CORE_API gr_scrambler_bb_sptr gr_make_scrambler_bb(int mask, int seed, int len);

/*!
 * Scramble an input stream using an LFSR.  This block works on the LSB only
 * of the input data stream, i.e., on an "unpacked binary" stream, and 
 * produces the same format on its output.
 * 
 * \param mask     Polynomial mask for LFSR
 * \param seed     Initial shift register contents
 * \param len      Shift register length
 *
 * \ingroup coding_blk
 */

class GR_CORE_API gr_scrambler_bb : public gr_sync_block
{
  friend GR_CORE_API gr_scrambler_bb_sptr gr_make_scrambler_bb(int mask, int seed, int len);

  gri_lfsr d_lfsr;

  gr_scrambler_bb(int mask, int seed, int len);

public:
  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_SCRAMBLER_BB_H */

/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_DIFF_ENCODER_BB_H
#define INCLUDED_GR_DIFF_ENCODER_BB_H

#include <digital_api.h>
#include <gr_sync_block.h>

class digital_diff_encoder_bb;
typedef boost::shared_ptr<digital_diff_encoder_bb> digital_diff_encoder_bb_sptr;

DIGITAL_API digital_diff_encoder_bb_sptr
digital_make_diff_encoder_bb(unsigned int modulus);

/*!
 * \brief y[0] = (x[0] + y[-1]) % M
 * \ingroup coding_blk
 *
 * Uses current and previous symbols and the alphabet modulus to
 * perform differential encoding.
 */
class DIGITAL_API digital_diff_encoder_bb : public gr_sync_block
{
  friend DIGITAL_API digital_diff_encoder_bb_sptr
    digital_make_diff_encoder_bb(unsigned int modulus);
  digital_diff_encoder_bb(unsigned int modulus);

  unsigned int	d_last_out;
  unsigned int	d_modulus;

 public:
  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif

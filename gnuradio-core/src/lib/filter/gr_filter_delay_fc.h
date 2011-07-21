/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FILTER_DELAY_FC_H
#define INCLUDED_GR_FILTER_DELAY_FC_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_io_signature.h>
#include <gr_types.h>

class gr_filter_delay_fc;
typedef boost::shared_ptr<gr_filter_delay_fc> gr_filter_delay_fc_sptr;

// public constructor
GR_CORE_API gr_filter_delay_fc_sptr gr_make_filter_delay_fc (const std::vector<float> &taps);

class gr_fir_fff;

/*!
 * \brief Filter-Delay Combination Block.
 * \ingroup filter_blk
 *
 * The block takes one or two float stream and outputs a complex
 * stream. If only one float stream is input, the real output is
 * a delayed version of this input and the imaginary output is the
 * filtered output. If two floats are connected to the input, then
 * the real output is the delayed version of the first input, and
 * the imaginary output is the filtered output. The delay in the 
 * real path accounts for the group delay introduced by the filter
 * in the imaginary path. The filter taps needs to be calculated
 * before initializing this block.
 *
 */
class GR_CORE_API gr_filter_delay_fc : public gr_sync_block
{
 public:
  ~gr_filter_delay_fc ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

 protected:
  gr_filter_delay_fc (const std::vector<float> &taps);

 private:
  unsigned int  d_delay;
  gr_fir_fff	*d_fir;

  friend GR_CORE_API gr_filter_delay_fc_sptr gr_make_filter_delay_fc (const std::vector<float> &taps);
};

#endif /* INCLUDED_GR_FILTER_DELAY_FC_H */

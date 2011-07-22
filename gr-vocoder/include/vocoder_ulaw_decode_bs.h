/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VOCODER_ULAW_DECODE_BS_H
#define INCLUDED_VOCODER_ULAW_DECODE_BS_H

#include <vocoder_api.h>
#include <gr_sync_block.h>

class vocoder_ulaw_decode_bs;

typedef boost::shared_ptr<vocoder_ulaw_decode_bs> vocoder_ulaw_decode_bs_sptr;

VOCODER_API vocoder_ulaw_decode_bs_sptr vocoder_make_ulaw_decode_bs();

/*!
 * \brief This block performs ulaw audio decoding. 
 *
 * \ingroup vocoder_blk
 */

class VOCODER_API vocoder_ulaw_decode_bs : public gr_sync_block
{
private:
  friend VOCODER_API vocoder_ulaw_decode_bs_sptr vocoder_make_ulaw_decode_bs();

  vocoder_ulaw_decode_bs();

 public:
  ~vocoder_ulaw_decode_bs();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_VOCODER_ULAW_DECODE_BS_H */

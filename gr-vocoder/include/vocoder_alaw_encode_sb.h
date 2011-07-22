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

#ifndef INCLUDED_VOCODER_ALAW_ENCODER_SB_H
#define INCLUDED_VOCODER_ALAW_ENCODER_SB_H

#include <vocoder_api.h>
#include <gr_sync_block.h>

class vocoder_alaw_encode_sb;

typedef boost::shared_ptr<vocoder_alaw_encode_sb> vocoder_alaw_encode_sb_sptr;

VOCODER_API vocoder_alaw_encode_sb_sptr vocoder_make_alaw_encode_sb();

/*!
 * \brief This block performs g.711 alaw audio encoding. 
 *
 * \ingroup vocoder_blk
 */
class VOCODER_API vocoder_alaw_encode_sb : public gr_sync_block
{
private:
  friend VOCODER_API vocoder_alaw_encode_sb_sptr vocoder_make_alaw_encode_sb();

  vocoder_alaw_encode_sb();

 public:
  ~vocoder_alaw_encode_sb();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_VOCODER_ALAW_ENCODE_SB_H */

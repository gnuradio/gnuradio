/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FAKE_CHANNEL_CODER_PP_H
#define INCLUDED_GR_FAKE_CHANNEL_CODER_PP_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_fake_channel_encoder_pp;
typedef boost::shared_ptr<gr_fake_channel_encoder_pp> gr_fake_channel_encoder_pp_sptr;

GR_CORE_API gr_fake_channel_encoder_pp_sptr
gr_make_fake_channel_encoder_pp(int input_vlen, int output_vlen);

/*!
 * \brief pad packet with alternating 1,0 pattern.
 * \ingroup coding_blk
 *
 * input: stream of byte vectors; output: stream of byte vectors
 */
class GR_CORE_API gr_fake_channel_encoder_pp : public gr_sync_block
{
  int	d_input_vlen;
  int	d_output_vlen;
  
  gr_fake_channel_encoder_pp(int input_vlen, int output_vlen);

  friend GR_CORE_API gr_fake_channel_encoder_pp_sptr
         gr_make_fake_channel_encoder_pp(int input_vlen, int output_vlen);
  
public:
  ~gr_fake_channel_encoder_pp();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

// ------------------------------------------------------------------------

class gr_fake_channel_decoder_pp;
typedef boost::shared_ptr<gr_fake_channel_decoder_pp> gr_fake_channel_decoder_pp_sptr;

GR_CORE_API gr_fake_channel_decoder_pp_sptr
gr_make_fake_channel_decoder_pp(int input_vlen, int output_vlen);

/*!
 * \brief remove fake padding from packet
 * \ingroup coding_blk
 *
 * input: stream of byte vectors; output: stream of byte vectors
 */
class GR_CORE_API gr_fake_channel_decoder_pp : public gr_sync_block
{
  int	d_input_vlen;
  int	d_output_vlen;
  
  gr_fake_channel_decoder_pp(int input_vlen, int output_vlen);

  friend GR_CORE_API gr_fake_channel_decoder_pp_sptr
         gr_make_fake_channel_decoder_pp(int input_vlen, int output_vlen);
  
public:
  ~gr_fake_channel_decoder_pp();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_FAKE_CHANNEL_CODER_PP_H */

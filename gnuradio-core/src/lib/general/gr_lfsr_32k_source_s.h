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

#ifndef INCLUDED_GR_LFSR_32K_SOURCE_S_H
#define INCLUDED_GR_LFSR_32K_SOURCE_S_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gri_lfsr_32k.h>

class gr_lfsr_32k_source_s;
typedef boost::shared_ptr<gr_lfsr_32k_source_s> gr_lfsr_32k_source_s_sptr;

GR_CORE_API gr_lfsr_32k_source_s_sptr gr_make_lfsr_32k_source_s ();

/*!
 * \brief LFSR pseudo-random source with period of 2^15 bits (2^11 shorts)
 * \ingroup source_blk
 *
 * This source is typically used along with gr_check_lfsr_32k_s to test
 * the USRP using its digital loopback mode.
 */
class GR_CORE_API gr_lfsr_32k_source_s : public gr_sync_block
{
  friend GR_CORE_API gr_lfsr_32k_source_s_sptr gr_make_lfsr_32k_source_s ();

  
  static const int BUFSIZE = 2048 - 1;	// ensure pattern isn't packet aligned
  int		d_index;
  short		d_buffer[BUFSIZE];

  gr_lfsr_32k_source_s ();

 public:

  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};


#endif

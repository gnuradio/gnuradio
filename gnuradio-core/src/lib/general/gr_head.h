/* -*- c++ -*- */
/*
 * Copyright 2004,2009 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_HEAD_H
#define INCLUDED_GR_HEAD_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <stddef.h>			// size_t

class gr_head;
typedef boost::shared_ptr<gr_head> gr_head_sptr;

/*!
 * \brief copies the first N items to the output then signals done
 * \ingroup slicedice_blk
 *
 * Useful for building test cases
 */

class GR_CORE_API gr_head : public gr_sync_block
{
  friend GR_CORE_API gr_head_sptr gr_make_head (size_t sizeof_stream_item, unsigned long long nitems);
  gr_head (size_t sizeof_stream_item, unsigned long long nitems);

  unsigned long long	d_nitems;
  unsigned long long	d_ncopied_items;

 public:
  int work (int noutput_items,
		 gr_vector_const_void_star &input_items,
		 gr_vector_void_star &output_items);

  void reset() { d_ncopied_items = 0; }
};

GR_CORE_API gr_head_sptr
gr_make_head (size_t sizeof_stream_item, unsigned long long nitems);


#endif /* INCLUDED_GR_HEAD_H */

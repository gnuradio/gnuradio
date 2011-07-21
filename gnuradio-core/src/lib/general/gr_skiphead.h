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

#ifndef INCLUDED_GR_SKIPHEAD_H
#define INCLUDED_GR_SKIPHEAD_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <stddef.h>      // size_t

class gr_skiphead;
typedef boost::shared_ptr<gr_skiphead> gr_skiphead_sptr;

GR_CORE_API gr_skiphead_sptr
gr_make_skiphead (size_t itemsize, uint64_t nitems_to_skip);


/*!
 * \brief skips the first N items, from then on copies items to the output
 * \ingroup slicedice_blk
 *
 * Useful for building test cases and sources which have metadata or junk at the start
 */

class GR_CORE_API gr_skiphead : public gr_block
{
  friend GR_CORE_API gr_skiphead_sptr gr_make_skiphead (size_t itemsize, uint64_t nitems_to_skip);
  gr_skiphead (size_t itemsize, uint64_t nitems_to_skip);

  uint64_t  		d_nitems_to_skip;
  uint64_t		d_nitems;		// total items seen

 public:

  int general_work(int noutput_items,
		   gr_vector_int &ninput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_SKIPHEAD_H */

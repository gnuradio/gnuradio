/* -*- c++ -*- */
/*
 * Copyright 2004,2005 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_STREAMS_TO_VECTOR_H
#define INCLUDED_GR_STREAMS_TO_VECTOR_H

#include <gr_core_api.h>
#include <gr_sync_interpolator.h>

class gr_streams_to_vector;
typedef boost::shared_ptr<gr_streams_to_vector> gr_streams_to_vector_sptr;

GR_CORE_API gr_streams_to_vector_sptr 
gr_make_streams_to_vector (size_t item_size, size_t nstreams);


/*!
 * \brief convert N streams of items to 1 stream of vector length N
 * \ingroup slicedice_blk
 */
class GR_CORE_API gr_streams_to_vector : public gr_sync_block
{
  friend GR_CORE_API gr_streams_to_vector_sptr
    gr_make_streams_to_vector (size_t item_size, size_t nstreams);

 protected:
  gr_streams_to_vector (size_t item_size, size_t nstreams);

 public:
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_STREAMS_TO_VECTOR_H */

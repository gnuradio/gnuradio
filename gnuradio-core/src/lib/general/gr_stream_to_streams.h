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
#ifndef INCLUDED_GR_STREAM_TO_STREAMS_H
#define INCLUDED_GR_STREAM_TO_STREAMS_H

#include <gr_core_api.h>
#include <gr_sync_decimator.h>

class gr_stream_to_streams;
typedef boost::shared_ptr<gr_stream_to_streams> gr_stream_to_streams_sptr;

GR_CORE_API gr_stream_to_streams_sptr 
gr_make_stream_to_streams (size_t item_size, size_t nstreams);


/*!
 * \brief convert a stream of items into a N streams of items
 * \ingroup slicedice_blk
 *
 * Converts a stream of N items into N streams of 1 item.
 * Repeat ad infinitum.
 */
class GR_CORE_API gr_stream_to_streams : public gr_sync_decimator
{
  friend GR_CORE_API gr_stream_to_streams_sptr
    gr_make_stream_to_streams (size_t item_size, size_t nstreams);

 protected:
  gr_stream_to_streams (size_t item_size, size_t nstreams);

 public:
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_STREAM_TO_STREAMS_H */

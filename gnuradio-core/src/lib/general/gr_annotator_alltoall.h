/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_ANNOTATOR_ALLTOALL_H
#define	INCLUDED_GR_ANNOTATOR_ALLTOALL_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_annotator_alltoall;
typedef boost::shared_ptr<gr_annotator_alltoall> gr_annotator_alltoall_sptr;

// public constructor
GR_CORE_API gr_annotator_alltoall_sptr 
gr_make_annotator_alltoall (int when, size_t sizeof_stream_item);

/*!
 * \brief All-to-all stream annotator testing block. FOR TESTING PURPOSES ONLY.
 *
 * This block creates tags to be sent downstream every 10,000 items it sees. The
 * tags contain the name and ID of the instantiated block, use "seq" as a key,
 * and have a counter that increments by 1 for every tag produced that is used
 * as the tag's value. The tags are propagated using the all-to-all policy.
 *
 * It also stores a copy of all tags it sees flow past it. These tags can be
 * recalled externally with the data() member.
 *
 * This block is only meant for testing and showing how to use the tags.
 */
class GR_CORE_API gr_annotator_alltoall : public gr_sync_block
{
 public:
  ~gr_annotator_alltoall ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
  
  std::vector<gr_tag_t> data() const
  {
    return d_stored_tags;
  }

protected:
  gr_annotator_alltoall (int when, size_t sizeof_stream_item);

 private:
  size_t d_itemsize;
  uint64_t d_when;
  uint64_t d_tag_counter;
  std::vector<gr_tag_t> d_stored_tags;

  friend GR_CORE_API gr_annotator_alltoall_sptr
  gr_make_annotator_alltoall (int when, size_t sizeof_stream_item);
};

#endif

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

#ifndef INCLUDED_GR_ANNOTATOR_RAW_H
#define	INCLUDED_GR_ANNOTATOR_RAW_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gruel/pmt.h>
#include <gruel/thread.h>

class gr_annotator_raw;
typedef boost::shared_ptr<gr_annotator_raw> gr_annotator_raw_sptr;

// public constructor
GR_CORE_API gr_annotator_raw_sptr 
gr_make_annotator_raw(size_t sizeof_stream_item);

/*!
 * \brief raw stream annotator testing block.
 *
 * This block creates arbitrary tags to be sent downstream
 * blocks to be sent are set manually via accessor methods and are sent only once.
 *
 * This block is intended for testing of tag related blocks
 */
class GR_CORE_API gr_annotator_raw : public gr_sync_block
{
 public:
  ~gr_annotator_raw();
  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);

  // insert a tag to be added
  void add_tag(uint64_t offset, pmt::pmt_t key, pmt::pmt_t val);

protected:
  gr_annotator_raw(size_t sizeof_stream_item);

 private:
  size_t d_itemsize;
  std::vector<gr_tag_t> d_queued_tags;
  gruel::mutex d_mutex;

  friend GR_CORE_API gr_annotator_raw_sptr
  gr_make_annotator_raw(size_t sizeof_stream_item);
};

#endif

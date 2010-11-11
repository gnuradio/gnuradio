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

#ifndef INCLUDED_GR_ANNOTATOR_1TO1_H
#define	INCLUDED_GR_ANNOTATOR_1TO1_H

#include <gr_sync_block.h>

class gr_annotator_1to1;
typedef boost::shared_ptr<gr_annotator_1to1> gr_annotator_1to1_sptr;

// public constructor
gr_annotator_1to1_sptr 
gr_make_annotator_1to1 (size_t sizeof_stream_item);

class gr_annotator_1to1 : public gr_sync_block
{
 public:
  ~gr_annotator_1to1 ();
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

protected:
  gr_annotator_1to1 (size_t sizeof_stream_item);

 private:
  size_t d_itemsize;
  std::stringstream d_sout;
  uint64_t d_tag_counter;

  friend gr_annotator_1to1_sptr
  gr_make_annotator_1to1 (size_t sizeof_stream_item);
};

#endif

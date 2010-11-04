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

#ifndef INCLUDED_GR_RANDOM_ANNOTATOR_H
#define	INCLUDED_GR_RANDOM_ANNOTATOR_H

#include <gr_sync_block.h>

class gr_random_annotator;
typedef boost::shared_ptr<gr_random_annotator> gr_random_annotator_sptr;

// public constructor
gr_random_annotator_sptr 
gr_make_random_annotator (size_t sizeof_stream_item);

class gr_random_annotator : public gr_sync_block
{
 public:
  ~gr_random_annotator ();
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

protected:
  gr_random_annotator (size_t sizeof_stream_item);

 private:
  friend gr_random_annotator_sptr
  gr_make_random_annotator (size_t sizeof_stream_item);
};

#endif

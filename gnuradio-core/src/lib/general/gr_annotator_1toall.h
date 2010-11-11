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

#ifndef INCLUDED_GR_ANNOTATOR_1TOALL_H
#define	INCLUDED_GR_ANNOTATOR_1TOALL_H

#include <gr_sync_block.h>

class gr_annotator_1toall;
typedef boost::shared_ptr<gr_annotator_1toall> gr_annotator_1toall_sptr;

// public constructor
gr_annotator_1toall_sptr 
gr_make_annotator_1toall (size_t sizeof_stream_item);

class gr_annotator_1toall : public gr_sync_block
{
 public:
  ~gr_annotator_1toall ();
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

protected:
  gr_annotator_1toall (size_t sizeof_stream_item);

 private:
  size_t d_itemsize;
  std::stringstream d_sout;
  uint64_t d_tag_counter;

  friend gr_annotator_1toall_sptr
  gr_make_annotator_1toall (size_t sizeof_stream_item);
};

#endif

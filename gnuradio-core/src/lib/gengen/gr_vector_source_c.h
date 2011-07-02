/* -*- c++ -*- */
/*
 * Copyright 2004,2008 Free Software Foundation, Inc.
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

// WARNING: this file is machine generated.  Edits will be over written

#ifndef INCLUDED_GR_VECTOR_SOURCE_C_H
#define INCLUDED_GR_VECTOR_SOURCE_C_H

#include <gr_sync_block.h>

class gr_vector_source_c;
typedef boost::shared_ptr<gr_vector_source_c> gr_vector_source_c_sptr;

/*!
 * \brief source of gr_complex's that gets its data from a vector
 * \ingroup source_blk
 */

class gr_vector_source_c : public gr_sync_block {
  friend gr_vector_source_c_sptr 
  gr_make_vector_source_c (const std::vector<gr_complex> &data, bool repeat, int vlen);

  std::vector<gr_complex>	d_data;
  bool			d_repeat;
  unsigned int		d_offset;
  int			d_vlen;

  gr_vector_source_c (const std::vector<gr_complex> &data, bool repeat, int vlen);

 public:
  void rewind() {d_offset=0;}
  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

gr_vector_source_c_sptr
gr_make_vector_source_c (const std::vector<gr_complex> &data, bool repeat = false, int vlen = 1);

#endif

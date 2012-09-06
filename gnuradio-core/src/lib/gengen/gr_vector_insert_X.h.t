/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

// @WARNING@

#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <gr_core_api.h>
#include <gr_block.h>

class GR_CORE_API @NAME@;
typedef boost::shared_ptr<@NAME@> @NAME@_sptr;

/*!
 * \brief source of @TYPE@'s that gets its data from a vector
 * \ingroup source_blk
 */

class @NAME@ : public gr_block {
  friend GR_CORE_API @NAME@_sptr
  gr_make_@BASE_NAME@(const std::vector<@TYPE@> &data, int periodicity, int offset);

  std::vector<@TYPE@>	d_data;
  int		d_offset;
  int       d_periodicity;

  @NAME@(const std::vector<@TYPE@> &data, int periodicity, int offset);

 public:
  void rewind() {d_offset=0;}
  virtual int general_work(int noutput_items,
			   gr_vector_int &ninput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items);
  void set_data(const std::vector<@TYPE@> &data){ d_data = data; rewind(); }
};

GR_CORE_API @NAME@_sptr
gr_make_@BASE_NAME@(const std::vector<@TYPE@> &data, int periodicity, int offset=0);

#endif

/* -*- c++ -*- */
/*
 * Copyright 2004, 2009 Free Software Foundation, Inc.
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
#include <gr_sync_block.h>

class @NAME@;
typedef boost::shared_ptr<@NAME@> @SPTR_NAME@;

GR_CORE_API @SPTR_NAME@ gr_make_@BASE_NAME@ (size_t vlen = 1);

/*!
 * \brief output = input_0 -  input_1 - ...)
 * \ingroup math_blk
 *
 * Subtract across all input streams.
 */
class GR_CORE_API @NAME@ : public gr_sync_block
{
  friend GR_CORE_API @SPTR_NAME@ gr_make_@BASE_NAME@ (size_t vlen);

  @NAME@ (size_t vlen);

  size_t d_vlen;

 public:

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif

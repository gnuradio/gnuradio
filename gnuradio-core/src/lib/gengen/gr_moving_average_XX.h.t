/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#include <gr_sync_block.h>

class @NAME@;

typedef boost::shared_ptr<@NAME@> @SPTR_NAME@;

@SPTR_NAME@ gr_make_@BASE_NAME@ (int length, @O_TYPE@ scale, int max_iter = 4096);

/*!
 * \brief output is the moving sum of the last N samples, scaled by the scale factor
 * \ingroup filter.  max_iter limits how long we go without flushing the accumulator
 * This is necessary to avoid numerical instability for float and complex.
 *
 */
class @NAME@ : public gr_sync_block
{
private:
  friend @SPTR_NAME@ gr_make_@BASE_NAME@(int length, @O_TYPE@ scale, int max_iter);

  @NAME@ (int length, @O_TYPE@ scale, int max_iter = 4096);

  int d_length;
  @O_TYPE@ d_scale;
  int d_max_iter;

public:
  ~@NAME@ ();	

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* @GUARD_NAME@ */

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

#ifndef   	INCLUDED_GR_STRETCH_FF_H_
# define   	INCLUDED_GR_STRETCH_FF_H_

#include <gr_core_api.h>
#include <gr_sync_block.h>

/*!
 * \brief adjust y-range of an input vector by mapping to range
 * (max-of-input, stipulated-min). Primarily for spectral signature
 * matching by normalizing spectrum dynamic ranges.
 * \ingroup misc_blk
 */


class gr_stretch_ff;
typedef boost::shared_ptr<gr_stretch_ff> gr_stretch_ff_sptr;

GR_CORE_API gr_stretch_ff_sptr gr_make_stretch_ff(float lo, size_t vlen);

class GR_CORE_API gr_stretch_ff : public gr_sync_block
{
  friend GR_CORE_API gr_stretch_ff_sptr gr_make_stretch_ff(float lo, size_t vlen);

  float	d_lo;		// the constant
  size_t d_vlen;
  gr_stretch_ff(float lo, size_t vlen);

 public:
  float lo() const { return d_lo; }
  void set_lo(float lo) { d_lo = lo; }
  size_t vlen() const { return d_vlen; }

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif

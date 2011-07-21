/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_DEINTERLEAVE_H
#define INCLUDED_GR_DEINTERLEAVE_H

#include <gr_core_api.h>
#include <gr_sync_decimator.h>

class gr_deinterleave;
typedef boost::shared_ptr<gr_deinterleave> gr_deinterleave_sptr;

GR_CORE_API gr_deinterleave_sptr gr_make_deinterleave (size_t itemsize);

/*!
 * \brief deinterleave a single input into N outputs
 * \ingroup slicedice_blk
 */
class GR_CORE_API gr_deinterleave : public gr_sync_decimator
{
  friend GR_CORE_API gr_deinterleave_sptr gr_make_deinterleave (size_t itemsize);

  size_t	d_itemsize;

  gr_deinterleave (size_t itemsize);

public:
  ~gr_deinterleave ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  bool check_topology (int ninputs, int noutputs);

};

#endif /* INCLUDED_GR_DEINTERLEAVE_H */

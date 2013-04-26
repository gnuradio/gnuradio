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

#ifndef INCLUDED_GR_INTERLEAVE_H
#define INCLUDED_GR_INTERLEAVE_H

#include <gr_core_api.h>
#include <gr_sync_interpolator.h>

class gr_interleave;
typedef boost::shared_ptr<gr_interleave> gr_interleave_sptr;

GR_CORE_API gr_interleave_sptr gr_make_interleave (size_t itemsize);

/*!
 * \brief interleave N inputs to a single output
 */
class GR_CORE_API gr_interleave : public gr_sync_interpolator
{
  friend GR_CORE_API gr_interleave_sptr gr_make_interleave (size_t itemsize);

  size_t	d_itemsize;

  gr_interleave (size_t itemsize);

public:
  ~gr_interleave ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  bool check_topology (int ninputs, int noutputs);

};

#endif /* INCLUDED_GR_INTERLEAVE_H */

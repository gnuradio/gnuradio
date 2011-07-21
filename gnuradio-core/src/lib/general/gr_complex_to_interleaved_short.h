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

#ifndef INCLUDED_GR_COMPLEX_TO_INTERLEAVED_SHORT_H
#define INCLUDED_GR_COMPLEX_TO_INTERLEAVED_SHORT_H

#include <gr_core_api.h>
#include <gr_sync_interpolator.h>

class gr_complex_to_interleaved_short;
typedef boost::shared_ptr<gr_complex_to_interleaved_short>
  gr_complex_to_interleaved_short_sptr;

GR_CORE_API gr_complex_to_interleaved_short_sptr
gr_make_complex_to_interleaved_short ();

/*!
 * \brief Convert stream of complex to a stream of interleaved shorts
 * \ingroup converter_blk
 */

class GR_CORE_API gr_complex_to_interleaved_short : public gr_sync_interpolator
{
  friend GR_CORE_API gr_complex_to_interleaved_short_sptr gr_make_complex_to_interleaved_short ();
  gr_complex_to_interleaved_short ();

 public:
  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_COMPLEX_TO_INTERLEAVED_SHORT_H */

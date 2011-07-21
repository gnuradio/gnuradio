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

#ifndef INCLUDED_GR_INTERLEAVED_SHORT_TO_COMPLEX_H
#define INCLUDED_GR_INTERLEAVED_SHORT_TO_COMPLEX_H

#include <gr_core_api.h>
#include <gr_sync_decimator.h>

class gr_interleaved_short_to_complex;
typedef boost::shared_ptr<gr_interleaved_short_to_complex>
  gr_interleaved_short_to_complex_sptr;

GR_CORE_API gr_interleaved_short_to_complex_sptr
gr_make_interleaved_short_to_complex ();

/*!
 * \brief Convert stream of interleaved shorts to a stream of complex
 * \ingroup converter_blk
 */

class GR_CORE_API gr_interleaved_short_to_complex : public gr_sync_decimator
{
  friend GR_CORE_API gr_interleaved_short_to_complex_sptr gr_make_interleaved_short_to_complex ();
  gr_interleaved_short_to_complex ();

 public:
  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_INTERLEAVED_SHORT_TO_COMPLEX_H */

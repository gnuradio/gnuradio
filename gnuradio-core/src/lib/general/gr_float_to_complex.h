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

#ifndef INCLUDED_GR_FLOAT_TO_COMPLEX_H
#define INCLUDED_GR_FLOAT_TO_COMPLEX_H

#include <gr_sync_block.h>
#include <gr_complex.h>

class gr_float_to_complex;
typedef boost::shared_ptr<gr_float_to_complex> gr_float_to_complex_sptr;

gr_float_to_complex_sptr
gr_make_float_to_complex ();

/*!
 * \brief Convert 1 or 2 streams of float to a stream of gr_complex
 * \ingroup converter
 */

class gr_float_to_complex : public gr_sync_block
{
  friend gr_float_to_complex_sptr gr_make_float_to_complex ();
  gr_float_to_complex ();

 public:
  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};


#endif /* INCLUDED_GR_FLOAT_TO_COMPLEX_H */

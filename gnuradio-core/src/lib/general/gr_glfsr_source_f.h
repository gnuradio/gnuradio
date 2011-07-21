/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_GLFSR_SOURCE_F_H
#define INCLUDED_GR_GLFSR_SOURCE_F_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gri_glfsr;

class gr_glfsr_source_f;
typedef boost::shared_ptr<gr_glfsr_source_f> gr_glfsr_source_f_sptr;

GR_CORE_API gr_glfsr_source_f_sptr gr_make_glfsr_source_f(int degree, bool repeat=true, int mask=0, int seed=1);

/*!
 * \brief Galois LFSR pseudo-random source generating float outputs -1.0 - 1.0
 * \ingroup source_blk
 */
class GR_CORE_API gr_glfsr_source_f : public gr_sync_block
{
 private:
  friend GR_CORE_API gr_glfsr_source_f_sptr
  gr_make_glfsr_source_f(int degree, bool repeat, int mask, int seed);
  
  gri_glfsr *d_glfsr;

  bool d_repeat;
  unsigned int d_index;
  unsigned int d_length;
  
  gr_glfsr_source_f(int degree, bool repeat, int mask, int seed);

 public:

  ~gr_glfsr_source_f();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);

  unsigned int period() const { return d_length; }
  int mask() const;
};

#endif /* INCLUDED_GR_GLFSR_SOURCE_F_H */

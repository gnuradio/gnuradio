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

// WARNING: this file is machine generated.  Edits will be over written

#ifndef INCLUDED_GR_NOISE_SOURCE_C_H
#define INCLUDED_GR_NOISE_SOURCE_C_H

#include <gr_sync_block.h>
#include <gr_noise_type.h>
#include <gr_random.h>


class gr_noise_source_c;
typedef boost::shared_ptr<gr_noise_source_c> gr_noise_source_c_sptr;

gr_noise_source_c_sptr 
gr_make_noise_source_c (gr_noise_type_t type, float ampl, long seed = 3021);

/*!
 * \brief random number source
 * \ingroup source_blk
 */
class gr_noise_source_c : public gr_sync_block {
  friend gr_noise_source_c_sptr 
  gr_make_noise_source_c (gr_noise_type_t type, float ampl, long seed);

  gr_noise_type_t	d_type;
  float			d_ampl;
  gr_random		d_rng;

  gr_noise_source_c (gr_noise_type_t type, float ampl, long seed = 3021);

 public:
  void set_type (gr_noise_type_t type) { d_type = type; }
  void set_amplitude (float ampl) { d_ampl = ampl; }

  virtual int work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items);
};


#endif

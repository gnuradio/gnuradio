/* -*- c++ -*- */
/*
 * Copyright 2007,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_GLFSR_SOURCE_B_H
#define INCLUDED_GR_GLFSR_SOURCE_B_H

#include <digital_api.h>
#include <gr_sync_block.h>

class gri_glfsr;

class digital_glfsr_source_b;
typedef boost::shared_ptr<digital_glfsr_source_b> digital_glfsr_source_b_sptr;

DIGITAL_API digital_glfsr_source_b_sptr
digital_make_glfsr_source_b(int degree, bool repeat=true,
			    int mask=0, int seed=1);

/*!
 * \brief Galois LFSR pseudo-random source
 * \ingroup source_blk
 *
 * \param degree Degree of shift register must be in [1, 32]. If mask
 *               is 0, the degree determines a default mask (see
 *               digital_impl_glfsr.cc for the mapping).
 * \param repeat Set to repeat sequence.
 * \param mask   Allows a user-defined bit mask for indexes of the shift
 *               register to feed back.
 * \param seed   Initial setting for values in shift register.
 */
class DIGITAL_API digital_glfsr_source_b : public gr_sync_block
{
 private:
  friend DIGITAL_API digital_glfsr_source_b_sptr
    digital_make_glfsr_source_b(int degree, bool repeat,
				int mask, int seed);

  gri_glfsr *d_glfsr;

  bool d_repeat;
  unsigned int d_index;
  unsigned int d_length;

  digital_glfsr_source_b(int degree, bool repeat,
			 int mask, int seed);

 public:

  ~digital_glfsr_source_b();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);

  unsigned int period() const { return d_length; }
  int mask() const;
};

#endif /* INCLUDED_GR_GLFSR_SOURCE_B_H */

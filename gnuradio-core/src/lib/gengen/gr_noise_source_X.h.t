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

// @WARNING@

#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_noise_type.h>
#include <gr_random.h>


class @NAME@;
typedef boost::shared_ptr<@NAME@> @NAME@_sptr;

/*! \brief Make a noise source
 * \param type  the random distribution to use (see gr_noise_type.h)
 * \param ampl  a scaling factor for the output
 * \param seed seed for random generators. Note that for uniform and
 * Gaussian distributions, this should be a negative number.
 */
GR_CORE_API @NAME@_sptr 
gr_make_@BASE_NAME@ (gr_noise_type_t type, float ampl, long seed = 0);

/*!
 * \brief Random number source
 * \ingroup source_blk
 *
 * \details
 * Generate random values from different distributions. 
 * Currently, only Gaussian and uniform are enabled.
 *
 * \param type  the random distribution to use (see gr_noise_type.h)
 * \param ampl  a scaling factor for the output
 * \param seed seed for random generators. Note that for uniform and
 * Gaussian distributions, this should be a negative number.
 */
class GR_CORE_API @NAME@ : public gr_sync_block {
  friend GR_CORE_API @NAME@_sptr 

  gr_make_@BASE_NAME@ (gr_noise_type_t type, float ampl, long seed);

  gr_noise_type_t	d_type;
  float			d_ampl;
  gr_random		d_rng;

  @NAME@ (gr_noise_type_t type, float ampl, long seed = 0);

 public:
  void set_type (gr_noise_type_t type) { d_type = type; }
  void set_amplitude (float ampl) { d_ampl = ampl; }

  gr_noise_type_t type () const { return d_type; }
  float amplitude () const { return d_ampl; }

  virtual int work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items);
};


#endif

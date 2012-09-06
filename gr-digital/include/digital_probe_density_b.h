/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_PROBE_DENSITY_B_H
#define INCLUDED_GR_PROBE_DENSITY_B_H

#include <digital_api.h>
#include <gr_sync_block.h>

class digital_probe_density_b;

typedef boost::shared_ptr<digital_probe_density_b> digital_probe_density_b_sptr;

DIGITAL_API digital_probe_density_b_sptr
digital_make_probe_density_b(double alpha);

/*!
 * This block maintains a running average of the input stream and
 * makes it available as an accessor function.  The input stream
 * is type unsigned char.
 *
 * If you send this block a stream of unpacked bytes, it will tell
 * you what the bit density is.
 *
 * \param alpha     Average filter constant
 *
 */

class DIGITAL_API digital_probe_density_b : public gr_sync_block
{
private:
  friend DIGITAL_API digital_probe_density_b_sptr
    digital_make_probe_density_b(double alpha);

  double d_alpha;
  double d_beta;
  double d_density;

  digital_probe_density_b(double alpha);

public:
  ~digital_probe_density_b();

  /*!
   * \brief Returns the current density value
   */
  double density() const { return d_density; }

  /*!
   * \brief Set the average filter constant
   */
  void set_alpha(double alpha);

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_PROBE_DENSITY_B_H */

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

GR_SWIG_BLOCK_MAGIC(gr,fractional_interpolator_cc);

gr_fractional_interpolator_cc_sptr gr_make_fractional_interpolator_cc (float phase_shift, float interp_ratio);

class gr_fractional_interpolator_cc : public gr_block
{
private:
  gr_fractional_interpolator_cc (float phase_shift, float interp_ratio);

public:
  float mu() const;
  float interp_ratio() const;
  void set_mu (float mu);
  void set_interp_ratio (float interp_ratio);
};

/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(digital,kurtotic_equalizer_cc)

// retrieve info on the base class, without generating wrappers since
// the base class has a pure virual method.
%import "gr_adaptive_fir_ccc.i"

digital_kurtotic_equalizer_cc_sptr
digital_make_kurtotic_equalizer_cc(int num_taps,
				   float mu);

class digital_kurtotic_equalizer_cc : public gr_adaptive_fir_ccc
{
private:
  digital_kurtotic_equalizer_cc(int num_taps, float mu);

public:
  void set_gain(float mu);
};

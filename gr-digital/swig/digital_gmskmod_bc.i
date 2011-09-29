/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(digital, gmskmod_bc)

digital_gmskmod_bc_sptr
digital_make_gmskmod_bc(unsigned samples_per_sym=2,
			double bt=0.3, unsigned L=4);

class digital_gmskmod_bc : public gr_hier_block2
{
 private:
  digital_cpmmod_bc(int type, float h,
		    unsigned samples_per_sym,
		    double beta, unsigned L);

 public:
  std::vector<float> get_taps();
};


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

GR_SWIG_BLOCK_MAGIC(gr,glfsr_source_b);

gr_glfsr_source_b_sptr 
gr_make_glfsr_source_b(int degree, bool repeat=true, int mask=0, int seed=1)
  throw (std::runtime_error);

class gr_glfsr_source_b : public gr_sync_block 
{
protected:
  gr_glfsr_source_b(int degree, bool repeat, int mask, int seed);

public:
  unsigned int period() const;
  int mask() const;
};

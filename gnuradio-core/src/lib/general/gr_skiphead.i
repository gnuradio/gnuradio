/* -*- c++ -*- */
/*
 * Copyright 2005,2007,2010 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(gr,skiphead);

gr_skiphead_sptr gr_make_skiphead (size_t itemsize, uint64_t nitems_to_skip);

class gr_skiphead : public gr_block {
  friend gr_skiphead_sptr gr_make_skiphead (size_t itemsize, uint64_t nitems_to_skip);
  gr_skiphead (size_t itemsize, uint64_t nitems_to_skip);
};

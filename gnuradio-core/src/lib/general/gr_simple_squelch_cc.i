/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(gr,simple_squelch_cc);

gr_simple_squelch_cc_sptr
gr_make_simple_squelch_cc (double threshold_db, double alpha = 0.0001);

class gr_simple_squelch_cc : public gr_sync_block
{
public:
  bool unmuted () const { return d_unmuted; }
  void set_alpha (double alpha);
  void set_threshold (double decibels);

  double threshold() const;
  std::vector<float> squelch_range() const;
};

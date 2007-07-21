/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(gr,ctcss_squelch_ff);

%include gr_squelch_base_ff.i

gr_ctcss_squelch_ff_sptr 
gr_make_ctcss_squelch_ff(int rate, float freq, float level=0.01, int len=0, int ramp=0, bool gate=false);

class gr_ctcss_squelch_ff : public gr_squelch_base_ff
{
  gr_ctcss_squelch_ff(int rate, float freq, float level, int len, int ramp, bool gate);

public:
  std::vector<float> squelch_range() const;
  float level() const { return d_level; }
  void set_level(float level) { d_level = level; }
  int len() const { return d_len; }
};

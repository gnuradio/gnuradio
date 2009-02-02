/* -*- c++ -*- */
/*
 * Copyright 2006,2009 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(gr,pwr_squelch_cc);

// retrieve info on the base class, without generating wrappers since
// the base class has a pure virual method.
%import "gr_squelch_base_cc.i"

gr_pwr_squelch_cc_sptr 
gr_make_pwr_squelch_cc(double db, double alpha=0.0001, int ramp=0, bool gate=false);

class gr_pwr_squelch_cc : public gr_squelch_base_cc
{
private:
  gr_pwr_squelch_cc(double db, double alpha, int ramp, bool gate);

public:
  double threshold() const { return 10*log10(d_threshold); }
  void set_threshold(double db) { d_threshold = std::pow(10.0, db/10); }
  void set_alpha(double alpha) { d_iir.set_taps(alpha); }
  std::vector<float> squelch_range() const;
};

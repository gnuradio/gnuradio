/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(gr,fll_band_edge_cc);

gr_fll_band_edge_cc_sptr gr_make_fll_band_edge_cc (float samps_per_sym, float rolloff,
						   int filter_size, float alpha, float beta);

class gr_fll_band_edge_cc : public gr_sync_block
{
 private:
  gr_fll_band_edge_cc (float samps_per_sym, float rolloff,
		       int filter_size, float alpha, float beta);

 public:
  ~gr_fll_band_edge_cc ();

  void set_alpha (float alpha);
  void set_beta (float beta);
  void design_filter(float samps_per_sym, float rolloff, int filter_size);
  void print_taps();
};

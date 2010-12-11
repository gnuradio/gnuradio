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

GR_SWIG_BLOCK_MAGIC(gr,pfb_arb_resampler_ccf);

gr_pfb_arb_resampler_ccf_sptr gr_make_pfb_arb_resampler_ccf (float rate,
							     const std::vector<float> &taps,
							     unsigned int filter_size=32);

class gr_pfb_arb_resampler_ccf : public gr_block
{
 private:
  gr_pfb_arb_resampler_ccf (float rate,
			    const std::vector<float> &taps,
			    unsigned int filter_size);

 public:
  ~gr_pfb_arb_resampler_ccf ();

  //void set_taps (const std::vector<float> &taps);
  void print_taps();
  void set_rate (float rate);
};

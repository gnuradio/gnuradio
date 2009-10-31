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

GR_SWIG_BLOCK_MAGIC(gr,pfb_clock_sync_fff);

gr_pfb_clock_sync_fff_sptr gr_make_pfb_clock_sync_fff (double sps, float gain,
						       const std::vector<float> &taps,
						       unsigned int filter_size=32,
						       float init_phase=0,
						       float max_rate_deviation=1.5);

class gr_pfb_clock_sync_fff : public gr_block
{
 private:
  gr_pfb_clock_sync_fff (double sps, float gain,
			 const std::vector<float> &taps,
			 unsigned int filter_size,
			 float init_phase,
			 float max_rate_deviation);

 public:
  ~gr_pfb_clock_sync_fff ();

  void set_taps (const std::vector<float> &taps,
		 std::vector< std::vector<float> > &ourtaps,
		 std::vector<gr_fir_fff*> &ourfilter);

  std::vector<float> channel_taps(int channel);
  std::vector<float> diff_channel_taps(int channel);
  void print_taps();
  void print_diff_taps();
  void set_alpha(float alpha);
  void set_beta(float beta);
  void set_max_rate_deviation(float m);
};

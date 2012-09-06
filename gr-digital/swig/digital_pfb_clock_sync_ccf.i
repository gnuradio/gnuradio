/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(digital,pfb_clock_sync_ccf);

digital_pfb_clock_sync_ccf_sptr
digital_make_pfb_clock_sync_ccf(double sps, float loop_bw,
				const std::vector<float> &taps,
				unsigned int filter_size=32,
				float init_phase=0,
				float max_rate_deviation=1.5,
				int osps=1);

class digital_pfb_clock_sync_ccf : public gr_block
{
 public:
  void set_taps(const std::vector<float> &taps,
		std::vector< std::vector<float> > &ourtaps,
		std::vector<gr_fir_ccf*> &ourfilter);

  std::vector< std::vector<float> > get_taps();
  std::vector< std::vector<float> > get_diff_taps();
  std::vector<float> get_channel_taps(int channel);
  std::vector<float> get_diff_channel_taps(int channel);
  std::string get_taps_as_string();
  std::string get_diff_taps_as_string();

  void set_loop_bandwidth(float bw);
  void set_damping_factor(float df);
  void set_alpha(float alpha);
  void set_beta(float beta);
  void set_max_rate_deviation(float m);

  float get_loop_bandwidth() const;
  float get_damping_factor() const;
  float get_alpha() const;
  float get_beta() const;
  float get_clock_rate() const;
};

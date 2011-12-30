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

#include <gr_core_api.h>
#include <gr_top_block.h>
#include <gr_fractional_interpolator_cc.h>
#include <gr_sig_source_c.h>
#include <gr_fir_filter_ccc.h>
#include <gr_add_cc.h>
#include <gr_noise_source_c.h>
#include <gr_multiply_cc.h>

class gr_channel_model;
typedef boost::shared_ptr<gr_channel_model> gr_channel_model_sptr;


GR_CORE_API gr_channel_model_sptr gr_make_channel_model(double noise_voltage=0.0,
					    double frequency_offset=0.0,
					    double epsilon=1.0,
					    const std::vector<gr_complex> &taps=std::vector<gr_complex>(1, 1),
					    double noise_seed=3021);

/*!
 * \brief channel simulator
 * \ingroup misc_blk
 */
class GR_CORE_API gr_channel_model : public gr_hier_block2
{
 private:
  gr_channel_model(double noise_voltage,
		   double frequency_offset,
		   double epsilon,
		   const std::vector<gr_complex> &taps,
		   double noise_seed);

  friend GR_CORE_API gr_channel_model_sptr gr_make_channel_model(double noise_voltage,
						     double frequency_offset,
						     double epsilon,
						     const std::vector<gr_complex> &taps,
						     double noise_seed);
  
  gr_fractional_interpolator_cc_sptr d_timing_offset;
  gr_sig_source_c_sptr d_freq_offset;
  gr_fir_filter_ccc_sptr d_multipath;
  gr_add_cc_sptr d_noise_adder;
  gr_noise_source_c_sptr d_noise;
  gr_multiply_cc_sptr d_mixer_offset;
  
  std::vector<gr_complex> d_taps;
  
 public:
  void set_noise_voltage(double noise_voltage);
  void set_frequency_offset(double frequency_offset);
  void set_taps(const std::vector<gr_complex> &taps);
  void set_timing_offset(double epsilon);

  double noise_voltage() const;
  double frequency_offset() const;
  std::vector<gr_complex> taps() const;
  double timing_offset() const;
};

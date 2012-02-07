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

#include <gr_channel_model.h>
#include <gr_io_signature.h>
#include <gr_sig_source_f.h>
#include <iostream>

// Shared pointer constructor
gr_channel_model_sptr
gr_make_channel_model(double noise_voltage,
		      double frequency_offset,
		      double epsilon,
		      const std::vector<gr_complex> &taps,
		      double noise_seed)
{
  return gnuradio::get_initial_sptr(new gr_channel_model(noise_voltage,
							 frequency_offset,
							 epsilon,
							 taps,
							 noise_seed));
}

// Hierarchical block constructor
gr_channel_model::gr_channel_model(double noise_voltage,
				   double frequency_offset,
				   double epsilon,
				   const std::vector<gr_complex> &taps,
				   double noise_seed)
  : gr_hier_block2("gr_channel_model",
		   gr_make_io_signature(1, 1, sizeof(gr_complex)), 
		   gr_make_io_signature(1, 1, sizeof(gr_complex)))
{
  d_taps = taps;
  while(d_taps.size() < 2) {
    d_taps.push_back(0);
  }
  
  d_timing_offset = gr_make_fractional_interpolator_cc(0, epsilon);
  
  d_multipath = gr_make_fir_filter_ccc(1, d_taps);
  
  d_noise_adder = gr_make_add_cc();
  d_noise = gr_make_noise_source_c(GR_GAUSSIAN, noise_voltage, noise_seed);
  d_freq_offset = gr_make_sig_source_c(1, GR_SIN_WAVE, frequency_offset, 1.0, 0.0);
  d_mixer_offset = gr_make_multiply_cc();
  
  connect(self(), 0, d_timing_offset, 0);
  connect(d_timing_offset, 0, d_multipath, 0);
  connect(d_multipath, 0, d_mixer_offset, 0);
  connect(d_freq_offset, 0, d_mixer_offset, 1);
  connect(d_mixer_offset, 0, d_noise_adder, 1);
  connect(d_noise, 0, d_noise_adder, 0);
  connect(d_noise_adder, 0, self(), 0);
}

void
gr_channel_model::set_noise_voltage(double noise_voltage)
{
  d_noise->set_amplitude(noise_voltage);
}
   
void
gr_channel_model::set_frequency_offset(double frequency_offset)
{
  d_freq_offset->set_frequency(frequency_offset);
}
     
void
gr_channel_model::set_taps(const std::vector<gr_complex> &taps)
{
  d_taps = taps;
  while(d_taps.size() < 2) {
    d_taps.push_back(0);
  }
  d_multipath->set_taps(d_taps);
}

void
gr_channel_model::set_timing_offset(double epsilon)
{
  d_timing_offset->set_interp_ratio(epsilon);
}


double
gr_channel_model::noise_voltage() const
{
  return d_noise->amplitude();
}

double
gr_channel_model::frequency_offset() const
{
  return d_freq_offset->frequency();
}

std::vector<gr_complex>
gr_channel_model::taps() const
{
  return d_multipath->taps();
}

double
gr_channel_model::timing_offset() const
{
  return d_timing_offset->interp_ratio();
}

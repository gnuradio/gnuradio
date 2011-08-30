/* -*- c++ -*- */
/*
 * Copyright 2009,2011 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(digital,fll_band_edge_cc);

digital_fll_band_edge_cc_sptr digital_make_fll_band_edge_cc (float samps_per_sym,
							     float rolloff,
							     int filter_size,
							     float bandwidth);

class digital_fll_band_edge_cc : public gr_sync_block, public gri_control_loop
{
 private:
  digital_fll_band_edge_cc (float samps_per_sym, float rolloff,
			    int filter_size, float bandwidth);

 public:
  ~digital_fll_band_edge_cc ();

  void set_loop_bandwidth(float bw);
  void set_damping_factor(float df);
  void set_alpha(float alpha);
  void set_beta(float beta);
  void set_samples_per_symbol(float sps);
  void set_rolloff(float rolloff);
  void set_filter_size(int filter_size);
  void set_frequency(float freq);
  void set_phase(float phase);

  float get_loop_bandwidth() const;
  float get_damping_factor() const;
  float get_alpha() const;
  float get_beta() const;
  float get_samples_per_symbol() const;
  float get_rolloff() const;
  int get_filter_size() const;
  float get_frequency() const;
  float get_phase() const;

  void print_taps();
};

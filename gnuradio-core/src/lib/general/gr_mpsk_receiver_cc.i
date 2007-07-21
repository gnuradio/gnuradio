/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(gr,mpsk_receiver_cc);

gr_mpsk_receiver_cc_sptr gr_make_mpsk_receiver_cc (unsigned int M, float theta,
						   float alpha, float beta,
						   float fmin, float fmax,
						   float mu, float gain_mu, 
						   float omega, float gain_omega, float omega_rel);
class gr_mpsk_receiver_cc : public gr_block
{
 private:
  gr_mpsk_receiver_cc (unsigned int M,float theta,
				float alpha, float beta,
				float fmin, float fmax,
				float mu, float gain_mu, 
				float omega, float gain_omega, float omega_rel);
public:
  float mu() const { return d_mu;}
  float omega() const { return d_omega;}
  float gain_mu() const { return d_gain_mu;}
  float gain_omega() const { return d_gain_omega;}
  void set_mu (float mu) { d_mu = mu; }
  void set_omega (float omega) { 
    d_omega = omega;
    d_min_omega = omega*(1.0 - d_omega_rel);
    d_max_omega = omega*(1.0 + d_omega_rel);
  }
  void set_gain_mu (float gain_mu) { d_gain_mu = gain_mu; }
  void set_gain_omega (float gain_omega) { d_gain_omega = gain_omega; }
  float alpha() const { return d_alpha; }
  float beta() const { return d_beta; }
  float freq() const { return d_freq; }
  float phase() const { return d_phase; }
  void set_alpha(float alpha) { d_alpha = alpha; }
  void set_beta(float beta) { d_beta = beta; }
  void set_freq(float freq) { d_freq = freq; }
  void set_phase(float phase) { d_phase = phase; } 
};

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

GR_SWIG_BLOCK_MAGIC(gr,constellation_receiver_cb);

gr_constellation_receiver_cb_sptr gr_make_constellation_receiver_cb (gr_constellation_sptr constellation,
								     float alpha, float beta,
								     float fmin, float fmax);
class gr_constellation_receiver_cb : public gr_block
{
 private:
  gr_constellation_receiver_cb (gr_contellation_sptr constellation,
				float alpha, float beta,
				float fmin, float fmax);
public:
  float alpha() const { return d_alpha; }
  float beta() const { return d_beta; }
  float freq() const { return d_freq; }
  float phase() const { return d_phase; }
  void set_alpha(float alpha) { d_alpha = alpha; }
  void set_beta(float beta) { d_beta = beta; }
  void set_freq(float freq) { d_freq = freq; }
  void set_phase(float phase) { d_phase = phase; } 
};

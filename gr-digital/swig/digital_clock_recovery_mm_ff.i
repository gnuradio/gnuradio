/* -*- c++ -*- */
/*
 * Copyright 2004,2011 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(digital,clock_recovery_mm_ff);

digital_clock_recovery_mm_ff_sptr
digital_make_clock_recovery_mm_ff (float omega, float gain_omega,
				   float mu, float gain_mu,
				   float omega_relative_limit=0.001) throw(std::exception);

class digital_clock_recovery_mm_ff : public gr_sync_block
{
 private:
  digital_clock_recovery_mm_ff (float omega, float gain_omega,
				float mu, float gain_mu,
				float omega_relative_limit);

public:
  float mu() const;
  float omega() const;
  float gain_mu() const;
  float gain_omega() const;

  void set_gain_mu (float gain_mu);
  void set_gain_omega (float gain_omega);
  void set_mu (float omega);
  void set_omega (float omega);
};

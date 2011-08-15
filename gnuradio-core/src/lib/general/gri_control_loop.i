/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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


class gri_control_loop
{
 public:
  gri_control_loop(float loop_bw, float max_freq, float min_freq);
  virtual ~gri_control_loop();

  void update_gains();
  void advance_loop(float error);
  void phase_wrap();
  void frequency_limit();

  /*******************************************************************
    SET FUNCTIONS
  *******************************************************************/

  void set_loop_bandwidth(float bw);
  void set_damping_factor(float df);
  void set_alpha(float alpha);
  void set_beta(float beta);
  void set_frequency(float freq);
  void set_phase(float phase);

   
  /*******************************************************************
    GET FUNCTIONS
  *******************************************************************/

  float get_loop_bandwidth() const;
  float get_damping_factor() const;
  float get_alpha() const;
  float get_beta() const;
  float get_frequency() const;
  float get_phase() const;
};

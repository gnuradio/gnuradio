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

class microtune_xxxx_eval_board {
public:
  microtune_xxxx_eval_board (int which_pp = 0);
  virtual ~microtune_xxxx_eval_board ();

  //! is the eval board present?
  bool board_present_p ();

  /*!
   * \brief set RF and IF AGC control voltages ([0, 5] volts)
   */
  void set_RF_AGC_voltage (float volts);
  void set_IF_AGC_voltage (float volts);

  /*!
   * \brief set RF and IF AGC levels together (scale [0, 1000])
   *
   * This provides a simple linear interface for adjusting both
   * the RF and IF gain in consort.  This is the easy to use interface.
   * 0 corresponds to minimum gain. 1000 corresponds to maximum gain.
   */
  virtual void set_AGC (float value_0_1000) = 0;

  // returns actual freq or 0 if error (easier interface for SWIG)
  double set_RF_freq (double freq);

  /*!
   * \returns true iff PLL is locked
   */
  bool pll_locked_p ();
  
  /*!
   * \returns the output frequency (IF center freq) of the tuner in Hz.
   */
  double get_output_freq ();
};

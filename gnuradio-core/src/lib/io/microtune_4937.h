/* -*- c++ -*- */
/*
 * Copyright 2001,2003 Free Software Foundation, Inc.
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

#ifndef INCLUDED_MICROTUNE_4937_H
#define INCLUDED_MICROTUNE_4937_H

#include <gr_core_api.h>
#include <microtune_xxxx.h>

/*!
 * \brief class for controlling microtune 4937 tuner module
 * \ingroup hardware
 */
class GR_CORE_API microtune_4937 : public microtune_xxxx {
public:
  microtune_4937 (i2c_sptr i2c, int i2c_addr = 0x61);
  virtual ~microtune_4937 ();

  /*!
   * \brief select RF frequency to be tuned to output frequency.
   * \p freq is the requested frequency in Hz, \p actual_freq
   * is set to the actual frequency tuned.  It takes about 100 ms
   * for the PLL to settle.
   *
   * \returns true iff sucessful.
   */
  bool set_RF_freq (double freq, double *actual_freq);
  
  /*!
   * \returns true iff PLL is locked
   */
  bool pll_locked_p ();
  
  /*!
   * \returns the output frequency (IF center freq) of the tuner in Hz.
   */
  double get_output_freq ();

 private:

  i2c_sptr	d_i2c;
  int		d_i2c_addr;
  int		d_reference_divider;
  bool		d_fast_tuning_p;	/* if set, higher charge pump current:
					   faster tuning, worse phase noise
					   for distance < 10kHz to the carrier */
};

#endif /* INCLUDED_MICROTUNE_4937_H */

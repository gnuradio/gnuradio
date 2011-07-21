/* -*- c++ -*- */
/*
 * Copyright 2001,2003,2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_MICROTUNE_XXXX_H
#define INCLUDED_MICROTUNE_XXXX_H

#include <gr_core_api.h>
#include <boost/shared_ptr.hpp>

class i2c;
typedef boost::shared_ptr<i2c> i2c_sptr;

/*!
 * \brief abstract class for controlling microtune {4937,4702} tuner modules
 * \ingroup base
 */
class GR_CORE_API microtune_xxxx {
public:
  microtune_xxxx () {}
  virtual ~microtune_xxxx ();

  /*!
   * \brief select RF frequency to be tuned to output frequency.
   * \p freq is the requested frequency in Hz, \p actual_freq
   * is set to the actual frequency tuned.  It takes about 100 ms
   * for the PLL to settle.
   *
   * \returns true iff sucessful.
   */
  virtual bool set_RF_freq (double freq, double *actual_freq) = 0;
  
  // returns actual freq or 0 if error (easier interface for SWIG)
  double set_RF_freq (double freq);

  /*!
   * \returns true iff PLL is locked
   */
  virtual bool pll_locked_p () = 0;
  
  /*!
   * \returns the output frequency (IF center freq) of the tuner in Hz.
   */
  virtual double get_output_freq () = 0;

};

#endif /* INCLUDED_MICROTUNE_XXXX_H */

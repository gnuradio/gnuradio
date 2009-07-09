/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_USRP_TUNE_RESULT_H
#define INCLUDED_USRP_TUNE_RESULT_H

class usrp_tune_result
{
public:
  // RF frequency that corresponds to DC in the IF
  double baseband_freq;

  // frequency programmed into the DDC/DUC
  double dxc_freq;

  // residual frequency (typically < 0.01 Hz)
  double residual_freq;

  // is the spectrum inverted?
  bool inverted;

  usrp_tune_result(double baseband=0, double dxc=0, double residual=0, bool _inverted=false)
    : baseband_freq(baseband), dxc_freq(dxc),
      residual_freq(residual), inverted(_inverted) {}
};

#endif /* INCLUDED_USRP_TUNE_RESULT_H */

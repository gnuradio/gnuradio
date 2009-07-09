/* -*- c++ -*- */
//
// Copyright 2008,2009 Free Software Foundation, Inc.
// 
// This file is part of GNU Radio
// 
// GNU Radio is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either asversion 3, or (at your option)
// any later version.
// 
// GNU Radio is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with GNU Radio; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street,
// Boston, MA 02110-1301, USA.

#ifndef DB_DTT754_H
#define DB_DTT754_H

#include <usrp/db_base.h>
#include <boost/shared_ptr.hpp>

class db_dtt754 : public db_base
{
public:
  db_dtt754(usrp_basic_sptr usrp, int which);
  ~db_dtt754();
  
  float gain_min();
  float gain_max();
  float gain_db_per_step();
  bool  set_gain(float gain);

  double freq_min();
  double freq_max();
  struct freq_result_t set_freq(double target_freq);
  
  bool is_quadrature();
  bool spectrum_inverted();
  bool set_bw(float bw);

private:
  void _set_rfagc(float gain);
  void _set_ifagc(float gain);
  void _set_pga(float pga_gain);

  int d_i2c_addr;
  float d_bw, d_freq, d_IF, d_f_ref;
  bool d_inverted;
};

#endif

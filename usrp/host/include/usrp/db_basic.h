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

#ifndef DB_BASIC_H
#define DB_BASIC_H

#include <usrp/db_base.h>


/******************************************************************************/


class db_basic_tx : public db_base
{
public:
  db_basic_tx(usrp_basic_sptr usrp, int which);
  ~db_basic_tx();
  
  float gain_min();
  float gain_max();
  float gain_db_per_step();
  double freq_min();
  double freq_max();
  struct freq_result_t set_freq(double target_freq);
  bool  set_gain(float gain);
  bool  is_quadrature();
};


/******************************************************************************/


class db_basic_rx : public db_base
{
 public:
  db_basic_rx(usrp_basic_sptr usrp, int which, int subdev);
  ~db_basic_rx();
  
  float gain_min();
  float gain_max();
  float gain_db_per_step();
  double freq_min();
  double freq_max();
  struct freq_result_t set_freq(double target_freq);
  bool set_gain(float gain);
  bool is_quadrature();

private:
  int d_subdev;
};


/******************************************************************************/


class db_lf_rx : public db_basic_rx
{
 public:
  db_lf_rx(usrp_basic_sptr usrp, int which, int subdev);
  ~db_lf_rx();
  
  double freq_min();
  double freq_max();
};


/******************************************************************************/


class db_lf_tx : public db_basic_tx
{ 
 public:
  db_lf_tx(usrp_basic_sptr usrp, int which);
  ~db_lf_tx();
  
  double freq_min();
  double freq_max();
};


#endif

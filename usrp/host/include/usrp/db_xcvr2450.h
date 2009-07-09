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

#ifndef DB_XCVR2450_H
#define DB_XCVR2450_H

#include <usrp/db_base.h>
#include <boost/shared_ptr.hpp>

class xcvr2450;
typedef boost::shared_ptr<xcvr2450> xcvr2450_sptr;


/******************************************************************************/


class db_xcvr2450_base: public db_base
{
  /*
   * Abstract base class for all xcvr2450 boards.
   * 
   * Derive board specific subclasses from db_xcvr2450_base_{tx,rx}
   */
public:
  db_xcvr2450_base(usrp_basic_sptr usrp, int which);
  ~db_xcvr2450_base();
  struct freq_result_t set_freq(double target_freq);
  bool is_quadrature();
  double freq_min();
  double freq_max();

protected:
  xcvr2450_sptr d_xcvr;
  void shutdown_common();
};


/******************************************************************************/


class db_xcvr2450_tx : public db_xcvr2450_base
{
protected:
  void shutdown();

public:
  db_xcvr2450_tx(usrp_basic_sptr usrp, int which);
  ~db_xcvr2450_tx();

  float gain_min();
  float gain_max();
  float gain_db_per_step();
  bool  set_gain(float gain);
  bool  i_and_q_swapped();
};

class db_xcvr2450_rx : public db_xcvr2450_base
{
protected:
  void shutdown();

public:
  db_xcvr2450_rx(usrp_basic_sptr usrp, int which);
  ~db_xcvr2450_rx();

  float gain_min();
  float gain_max();
  float gain_db_per_step();
  bool  set_gain(float gain);
};



#endif

/* -*- c++ -*- */
//
// Copyright 2009 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DB_WBXNG_H
#define INCLUDED_DB_WBXNG_H

#include <usrp/db_base.h>
#include <cmath>

class adf4350;

class wbxng_base : public db_base
{
public:
  wbxng_base(usrp_basic_sptr usrp, int which);
  ~wbxng_base();

  struct freq_result_t set_freq(double freq);

  bool  is_quadrature();
  double freq_min();
  double freq_max();

protected:
  void _write_spi(std::string data);
  int _refclk_divisor();
  bool _get_locked();
  bool _set_pga(float pga_gain);

  bool d_first;
  int  d_spi_format;
  int  d_spi_enable;
  int  d_power_on;
  int  d_PD;

  boost::shared_ptr<adf4350> d_common;
};

// ----------------------------------------------------------------

class db_wbxng_tx : public wbxng_base
{
protected:
  void shutdown();

public:
  db_wbxng_tx(usrp_basic_sptr usrp, int which);
  ~db_wbxng_tx();

  float gain_min();
  float gain_max();
  float gain_db_per_step();

  bool set_auto_tr(bool on);
  bool set_enable(bool on);
  bool set_gain(float gain);
};

class db_wbxng_rx : public wbxng_base
{
protected:
  void shutdown();
  bool _set_attn(float attn);

public:
  db_wbxng_rx(usrp_basic_sptr usrp, int which);
  ~db_wbxng_rx();

  bool set_auto_tr(bool on);
  bool select_rx_antenna(int which_antenna);
  bool select_rx_antenna(const std::string &which_antenna);
  bool set_gain(float gain);

  float gain_min();
  float gain_max();
  float gain_db_per_step();
  bool i_and_q_swapped();
};

#endif /* INCLUDED_DB_WBXNG_H */

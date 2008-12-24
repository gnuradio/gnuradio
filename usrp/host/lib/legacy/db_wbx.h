/* -*- c++ -*- */
//
// Copyright 2008 Free Software Foundation, Inc.
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

#ifndef DB_WBX_H
#define DB_WBX_H

#include <db_base.h>
#include <boost/shared_ptr.hpp>


/*
  A few comments about the WBX boards:
  They are half-duplex.  I.e., transmit and receive are mutually exclusive.
  There is a single LO for both the Tx and Rx sides.
  The the shared control signals are hung off of the Rx side.
  The shared io controls are duplexed onto the Rx side pins.
  The wbx_high d'board always needs to be in 'auto_tr_mode'
*/


class wbx_base : public db_base
{
protected:
  void shutdown();

  /*
   * Abstract base class for all wbx boards.
   * 
   * Derive board specific subclasses from db_wbx_base_{tx,rx}
   */

public:
  wbx_base(usrp_basic_sptr usrp, int which);
  ~wbx_base();
  
  struct freq_result_t set_freq(double freq);
  float gain_min();
  float gain_max();
  float gain_db_per_step();
  bool set_gain(float gain);  
  bool is_quadrature();


protected:
  virtual bool _lock_detect();

  // FIXME: After testing, replace these with usrp_basic::common_write_io/oe
  bool _tx_write_oe(int value, int mask);
  bool _rx_write_oe(int value, int mask);
  bool _tx_write_io(int value, int mask);
  bool _rx_write_io(int value, int mask);
  virtual bool _rx_read_io();
  bool _tx_read_io();
  bool  _compute_regs(double freq);
  virtual double _refclk_freq();
  int _refclk_divisor();

  bool _set_pga(float pga_gain);

  bool d_first;
  int d_spi_format;
  int d_spi_enable;
  double d_lo_offset;
};


/****************************************************************************/


class wbx_base_tx : public wbx_base
{
public:
  wbx_base_tx(usrp_basic_sptr usrp, int which);
  ~wbx_base_tx();

  bool set_auto_tr(bool on);
  bool set_enable(bool on);
};


/****************************************************************************/


class wbx_base_rx : public wbx_base
{
public:
  wbx_base_rx(usrp_basic_sptr usrp, int which);
  ~wbx_base_rx();
  
  bool set_auto_tr(bool on);
  bool select_rx_antenna(int which_antenna);
  bool select_rx_antenna(const std::string &which_antenna);
  bool set_gain(float gain);
  bool i_and_q_swapped();
};


/****************************************************************************/


class _ADF410X_common
{
public:
  _ADF410X_common();
  virtual ~_ADF410X_common();
  
  bool _compute_regs(double freq, int &retR, int &retcontrol,
		     int &retN, double &retfreq);
  void _write_all(int R, int N, int control);
  void _write_R(int R);
  void _write_N(int N);
  void _write_func(int func);
  void _write_init(int init);
  int  _prescaler();
  virtual void _write_it(int v);
  virtual double _refclk_freq();
  virtual bool _rx_write_io(int value, int mask);
  virtual bool _lock_detect();

protected:
  virtual usrp_basic* usrp();

  int d_R_RSV, d_LDP, d_TEST, d_ABP;
  int d_N_RSV, d_CP_GAIN;
  int d_P, d_PD2, d_CP2, d_CP1, d_TC, d_FL;
  int d_CP3S, d_PDP, d_MUXOUT, d_PD1, d_CR;
  int d_R_DIV, d_A_DIV, d_B_DIV;
  int d_freq_mult;

  int d_spi_format;
  int d_spi_enable;
};


/****************************************************************************/


class _lo_common : public _ADF410X_common
{
public:
  _lo_common();
  ~_lo_common();

  double freq_min();
  double freq_max();
  
  void set_divider(int main_or_aux, int divisor);
  void set_divider(const std::string &main_or_aux, int divisor);

  struct freq_result_t set_freq(double freq);

protected:
  int d_R_DIV, d_P, d_CP2, d_CP1;
  int d_DIVSEL, d_DIV2, d_CPGAIN;
  int d_div, d_aux_div, d_main_div;
};
        

/****************************************************************************/


class db_wbx_lo_tx : public _lo_common, public wbx_base_tx
{
public:
  db_wbx_lo_tx(usrp_basic_sptr usrp, int which);
  ~db_wbx_lo_tx();

  float gain_min();
  float gain_max();
  float gain_db_per_step();
  bool  set_gain(float gain);

  double _refclk_freq();
  bool _rx_write_io(int value, int mask);
  bool _lock_detect();

protected:
  usrp_basic* usrp();
};
        

/****************************************************************************/


class db_wbx_lo_rx : public _lo_common, public  wbx_base_rx
{
public:
  db_wbx_lo_rx(usrp_basic_sptr usrp, int which);
  ~db_wbx_lo_rx();

  float gain_min();
  float gain_max();
  float gain_db_per_step();

  double _refclk_freq();
  bool _rx_write_io(int value, int mask);
  bool _lock_detect();

protected:
  usrp_basic* usrp();
};

#endif

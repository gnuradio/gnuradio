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

#ifndef DB_FLEXRF_H
#define DB_FLEXRF_H

#include <usrp/db_base.h>
#include <cmath>

//debug_using_gui = true                // Must be set to True or False
#define debug_using_gui false           // Must be set to True or False

class _AD4360_common;

class flexrf_base : public db_base
{
public:
  flexrf_base(usrp_basic_sptr usrp, int which, int _power_on=0);
  ~flexrf_base();

  struct freq_result_t set_freq(double freq);

  bool  is_quadrature();
  double freq_min();
  double freq_max();

protected:
  void _write_all(int R, int control, int N);
  void _write_control(int control);
  void _write_R(int R);
  void _write_N(int N);
  void _write_it(int v);
  bool _lock_detect();

  virtual bool _compute_regs(double freq, int &retR, int &retcontrol, 
			     int &retN, double &retfreq);
  int  _compute_control_reg();
  int _refclk_divisor();

  bool _set_pga(float pga_gain);

  int power_on() { return d_power_on; }
  int power_off() { return 0; }

  bool d_first;
  int  d_spi_format;
  int  d_spi_enable;
  int  d_power_on;
  int  d_PD;

  _AD4360_common *d_common;
};

// ----------------------------------------------------------------

class flexrf_base_tx : public flexrf_base
{
protected:
  void shutdown();

public:
  flexrf_base_tx(usrp_basic_sptr usrp, int which, int _power_on=0);
  ~flexrf_base_tx();

  // All RFX tx d'boards have fixed gain
  float gain_min();
  float gain_max();
  float gain_db_per_step();

  bool set_auto_tr(bool on);
  bool set_enable(bool on);
  bool set_gain(float gain);
};

class flexrf_base_rx : public flexrf_base
{
protected:
  void shutdown();

public:
  flexrf_base_rx(usrp_basic_sptr usrp, int which, int _power_on=0);
  ~flexrf_base_rx();
    
  bool set_auto_tr(bool on);
  bool select_rx_antenna(int which_antenna);
  bool select_rx_antenna(const std::string &which_antenna);
  bool set_gain(float gain);

};

// ----------------------------------------------------------------


class _AD4360_common
{
public:
  _AD4360_common();
  virtual ~_AD4360_common();

  virtual double freq_min() = 0;
  virtual double freq_max() = 0;

  bool _compute_regs(double refclk_freq, double freq, int &retR, 
		     int &retcontrol, int &retN, double &retfreq);
  int _compute_control_reg();
  virtual int _refclk_divisor();
  int _prescaler();

  void R_DIV(int div) { d_R_DIV = div; }

protected:
  int d_R_RSV, d_BSC, d_TEST, d_LDP, d_ABP, d_N_RSV, d_PL, d_MTLD;
  int d_CPG, d_CP3S, d_PDP, d_MUXOUT, d_CR, d_PC;

  // FIXME: d_PD might cause conflict from flexrf_base
  int d_A_DIV, d_B_DIV, d_R_DIV, d_P, d_PD, d_CP2, d_CP1, d_DIVSEL;
  int d_DIV2, d_CPGAIN, d_freq_mult;

};

//----------------------------------------------------------------------

class _2200_common : public _AD4360_common
{
 public:
  _2200_common();
  ~_2200_common() {}
 
  double freq_min();
  double freq_max();
};

//----------------------------------------------------------------------

class _2400_common : public _AD4360_common
{
 public:
  _2400_common();
  ~_2400_common() {}
 
  double freq_min();
  double freq_max();
};

//----------------------------------------------------------------------

class _1200_common : public _AD4360_common
{
public:
  _1200_common();
  ~_1200_common() {}

  double freq_min();
  double freq_max();
};

//-------------------------------------------------------------------------

class _1800_common : public _AD4360_common
{
 public:
  _1800_common();
  ~_1800_common() {}

  double freq_min();
  double freq_max();
};

//-------------------------------------------------------------------------

class _900_common : public _AD4360_common
{
public:
  _900_common();
  ~_900_common() {}
  
  double freq_min();
  double freq_max();
};

//-------------------------------------------------------------------------

class _400_common : public _AD4360_common
{
public:
  _400_common();
  ~_400_common() {}

  double freq_min();
  double freq_max();
};

class _400_tx : public _400_common
{
public:
  _400_tx();
  ~_400_tx() {}
};

class _400_rx : public _400_common
{
public:
  _400_rx();
  ~_400_rx() {}
};

//------------------------------------------------------------    

class db_flexrf_2200_tx : public flexrf_base_tx
{
 public:
  db_flexrf_2200_tx(usrp_basic_sptr usrp, int which);
  ~db_flexrf_2200_tx();

  // Wrapper calls to d_common functions
  bool _compute_regs(double freq, int &retR, int &retcontrol,
		     int &retN, double &retfreq);
};

class db_flexrf_2200_rx : public flexrf_base_rx
{
public:
  db_flexrf_2200_rx(usrp_basic_sptr usrp, int which);
  ~db_flexrf_2200_rx();
  
  float gain_min();
  float gain_max();
  float gain_db_per_step();
  bool i_and_q_swapped();

  bool _compute_regs(double freq, int &retR, int &retcontrol,
		     int &retN, double &retfreq);
};

//------------------------------------------------------------    

class db_flexrf_2400_tx : public flexrf_base_tx
{
 public:
  db_flexrf_2400_tx(usrp_basic_sptr usrp, int which);
  ~db_flexrf_2400_tx();

  // Wrapper calls to d_common functions
  bool _compute_regs(double freq, int &retR, int &retcontrol,
		     int &retN, double &retfreq);
};

class db_flexrf_2400_rx : public flexrf_base_rx
{
public:
  db_flexrf_2400_rx(usrp_basic_sptr usrp, int which);
  ~db_flexrf_2400_rx();
  
  float gain_min();
  float gain_max();
  float gain_db_per_step();
  bool i_and_q_swapped();

  bool _compute_regs(double freq, int &retR, int &retcontrol,
		     int &retN, double &retfreq);
};

//------------------------------------------------------------    

class db_flexrf_1200_tx : public flexrf_base_tx
{
public:
  db_flexrf_1200_tx(usrp_basic_sptr usrp, int which);
  ~db_flexrf_1200_tx();

  // Wrapper calls to d_common functions
  bool _compute_regs(double freq, int &retR, int &retcontrol,
		     int &retN, double &retfreq);
};

class db_flexrf_1200_rx : public flexrf_base_rx
{
public:
  db_flexrf_1200_rx(usrp_basic_sptr usrp, int which);
  ~db_flexrf_1200_rx();
  
  float gain_min();
  float gain_max();
  float gain_db_per_step();
  bool i_and_q_swapped();

  bool _compute_regs(double freq, int &retR, int &retcontrol,
		     int &retN, double &retfreq);
};

//------------------------------------------------------------    

class db_flexrf_1800_tx : public flexrf_base_tx
{
 public:
  db_flexrf_1800_tx(usrp_basic_sptr usrp, int which);
  ~db_flexrf_1800_tx();

  // Wrapper calls to d_common functions
  bool _compute_regs(double freq, int &retR, int &retcontrol,
		     int &retN, double &retfreq);
};

class db_flexrf_1800_rx : public flexrf_base_rx
{
public:
  db_flexrf_1800_rx(usrp_basic_sptr usrp, int which);
  ~db_flexrf_1800_rx();
  
  float gain_min();
  float gain_max();
  float gain_db_per_step();
  bool i_and_q_swapped();

  bool _compute_regs(double freq, int &retR, int &retcontrol,
		     int &retN, double &retfreq);
};

//------------------------------------------------------------    

class db_flexrf_900_tx : public flexrf_base_tx
{
 public:
  db_flexrf_900_tx(usrp_basic_sptr usrp, int which);
  ~db_flexrf_900_tx();

  // Wrapper calls to d_common functions
  bool _compute_regs(double freq, int &retR, int &retcontrol,
		     int &retN, double &retfreq);
};

class db_flexrf_900_rx : public flexrf_base_rx
{
public:
  db_flexrf_900_rx(usrp_basic_sptr usrp, int which);
  ~db_flexrf_900_rx();
  
  float gain_min();
  float gain_max();
  float gain_db_per_step();
  bool i_and_q_swapped();

  bool _compute_regs(double freq, int &retR, int &retcontrol,
		     int &retN, double &retfreq);
};


//------------------------------------------------------------    

class db_flexrf_400_tx : public flexrf_base_tx
{
 public:
  db_flexrf_400_tx(usrp_basic_sptr usrp, int which);
  ~db_flexrf_400_tx();

  // Wrapper calls to d_common functions
  bool _compute_regs(double freq, int &retR, int &retcontrol,
		     int &retN, double &retfreq);
};

class db_flexrf_400_rx : public flexrf_base_rx
{
public:
  db_flexrf_400_rx(usrp_basic_sptr usrp, int which);
  ~db_flexrf_400_rx();

  float gain_min();
  float gain_max();
  float gain_db_per_step();
  bool i_and_q_swapped();

  bool _compute_regs(double freq, int &retR, int &retcontrol,
		     int &retN, double &retfreq);
};

#endif

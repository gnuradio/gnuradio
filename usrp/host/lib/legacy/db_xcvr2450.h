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

#ifndef DB_XCVR2450_H
#define DB_XCVR2450_H

#include <db_base.h>
#include <boost/shared_ptr.hpp>

// TX IO Pins
#define HB_PA_OFF      (1 << 15)    // 5GHz PA, 1 = off, 0 = on
#define LB_PA_OFF      (1 << 14)    // 2.4GHz PA, 1 = off, 0 = on
#define ANTSEL_TX1_RX2 (1 << 13)    // 1 = Ant 1 to TX, Ant 2 to RX
#define ANTSEL_TX2_RX1 (1 << 12)    // 1 = Ant 2 to TX, Ant 1 to RX
#define TX_EN          (1 << 11)    // 1 = TX on, 0 = TX off
#define AD9515DIV      (1 << 4)     // 1 = Div  by 3, 0 = Div by 2

#define TX_OE_MASK HB_PA_OFF|LB_PA_OFF|ANTSEL_TX1_RX2|ANTSEL_TX2_RX1|TX_EN|AD9515DIV
#define TX_SAFE_IO HB_PA_OFF|LB_PA_OFF|ANTSEL_TX1_RX2|AD9515DIV

// RX IO Pins
#define LOCKDET (1 << 15)           // This is an INPUT!!!
#define EN      (1 << 14)
#define RX_EN   (1 << 13)           // 1 = RX on, 0 = RX off
#define RX_HP   (1 << 12)
#define RX_OE_MASK EN|RX_EN|RX_HP
#define RX_SAFE_IO EN

struct xcvr2450_key {
  std::string serial_no;
  int which;
};

class xcvr2450;
typedef boost::shared_ptr<xcvr2450> xcvr2450_sptr;

class xcvr2450
{
private:
  boost::weak_ptr<usrp_basic> d_weak_usrp;
  int d_which;

  int d_spi_format, d_spi_enable;
  
  int d_mimo, d_int_div, d_frac_div, d_highband, d_five_gig;
  int d_cp_current, d_ref_div, d_rssi_hbw;
  int d_txlpf_bw, d_rxlpf_bw, d_rxlpf_fine, d_rxvga_ser;
  int d_rssi_range, d_rssi_mode, d_rssi_mux;
  int d_rx_hp_pin, d_rx_hpf, d_rx_ant;
  int d_tx_ant, d_txvga_ser, d_tx_driver_lin;
  int d_tx_vga_lin, d_tx_upconv_lin, d_tx_bb_gain;
  int d_pabias_delay, d_pabias, rx_rf_gain, rx_bb_gain, d_txgain;
  int d_rx_rf_gain, d_rx_bb_gain;

  int d_reg_standby, d_reg_int_divider, d_reg_frac_divider, d_reg_bandselpll;
  int d_reg_cal, dsend_reg, d_reg_lpf, d_reg_rxrssi_ctrl, d_reg_txlin_gain;
  int d_reg_pabias, d_reg_rxgain, d_reg_txgain;

  int d_ad9515_div;

  void _set_rfagc(float gain);
  void _set_ifagc(float gain);
  void _set_pga(float pga_gain);

  usrp_basic_sptr usrp(){
    return usrp_basic_sptr(d_weak_usrp);    // throws bad_weak_ptr if d_usrp.use_count() == 0
  }

public:
  xcvr2450(usrp_basic_sptr usrp, int which);
  ~xcvr2450();

  bool operator==(xcvr2450_key x);

  void set_reg_standby();
  
  // Integer-Divider Ratio (3)
  void set_reg_int_divider();
  
  // Fractional-Divider Ratio (4)
  void set_reg_frac_divider();
  
  // Band Select and PLL (5)
  void set_reg_bandselpll();
  
  // Calibration (6)
  void set_reg_cal();

  // Lowpass Filter (7)
  void set_reg_lpf();
  
  // Rx Control/RSSI (8)
  void set_reg_rxrssi_ctrl();
  
  // Tx Linearity/Baseband Gain (9)
  void set_reg_txlin_gain();
  
  // PA Bias DAC (10)
  void set_reg_pabias();
  
  // Rx Gain (11)
  void set_reg_rxgain();
  
  // Tx Gain (12)
  void set_reg_txgain();
  
  // Send register write to SPI
  void send_reg(int v);

  void set_gpio();
  bool lock_detect();
  bool set_rx_gain(float gain);
  bool set_tx_gain(float gain);

  struct freq_result_t set_freq(double target_freq);
};


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
};


/******************************************************************************/


class db_xcvr2450_tx : public db_xcvr2450_base
{
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
public:
  db_xcvr2450_rx(usrp_basic_sptr usrp, int which);
  ~db_xcvr2450_rx();

  float gain_min();
  float gain_max();
  float gain_db_per_step();
  bool  set_gain(float gain);
};



#endif

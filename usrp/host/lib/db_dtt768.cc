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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <usrp/db_dtt768.h>
#include <db_base_impl.h>

int
control_byte_4()
{
  int C = 0;   // Charge Pump Current, no info on how to choose
  int R = 4;   // 125 kHz fref
  
  // int ATP = 7; // Disable internal AGC
  return (0x80 | C<<5 | R);
}

int
control_byte_5(float freq, int agcmode = 1)
{
  if(agcmode) {
    if(freq < 150e6) {
      return 0x3B;
    }
    else if(freq < 420e6) {
      return 0x7E;
    }
    else {
      return 0xB7;
    }
  }
  else {
    if(freq < 150e6) {
      return 0x39;
    }
    else if(freq < 420e6) {
      return 0x7C;
    }
    else {
      return 0xB5;
    }
  }
}
        
int
control_byte_6()
{
  int ATC = 0;   // AGC time constant = 100ms, 1 = 3S
  int IFE = 1;   // IF AGC amplifier enable
  int AT = 0;    // AGC control, ???
  
  return (ATC << 5 | IFE << 4 | AT);
}

int
control_byte_7()
{
  int SAS = 1;  // SAW Digital mode
  int AGD = 1;  // AGC disable
  int ADS = 0;  // AGC detector into ADC converter
  int T = 0;    // Test mode, undocumented
  return (SAS << 7 | AGD << 5 | ADS << 4 | T);
}

db_dtt768::db_dtt768(usrp_basic_sptr _usrp, int which)
  : db_base(_usrp, which)
{
  /*
   * Control custom DTT76803-based daughterboard.
   * 
   * @param usrp: instance of usrp.source_c
   * @param which: which side: 0 or 1 corresponding to RX_A or RX_B respectively
   * @type which: int
   */
  
  if(d_which == 0) {
    d_i2c_addr = 0x60;
  }
  else {
    d_i2c_addr = 0x62;
  }

  d_IF = 44e6;
        
  d_f_ref = 125e3;
  d_inverted = false;

  set_gain((gain_min() + gain_max()) / 2.0);

  bypass_adc_buffers(false);
}

db_dtt768::~db_dtt768()
{
}
  
float
db_dtt768::gain_min()
{
  return 0;
}

float
db_dtt768::gain_max()
{
  return 115;
}

float
db_dtt768::gain_db_per_step()
{
  return 1;
}

bool
db_dtt768::set_gain(float gain)
{
  assert(gain>=0 && gain<=115);

  float rfgain, ifgain, pgagain;
  if(gain > 60) {
    rfgain = 60;
    gain = gain - 60;
  }
  else {
    rfgain = gain;
    gain = 0;
  }
  
  if(gain > 35) {
    ifgain = 35;
    gain = gain - 35;
  }
  else {
    ifgain = gain;
    gain = 0;
  }
  pgagain = gain;
  
  _set_rfagc(rfgain);
  _set_ifagc(ifgain);
  _set_pga(pgagain);

  return true;
}

double
db_dtt768::freq_min()
{
  return 44e6;
}

double
db_dtt768::freq_max()
{
  return 900e6;
}

struct freq_result_t
db_dtt768::set_freq(double target_freq)
{
  /*
   * @returns (ok, actual_baseband_freq) where:
   * ok is True or False and indicates success or failure,
   * actual_baseband_freq is the RF frequency that corresponds to DC in the IF.
   */
  
  freq_result_t ret = {false, 0.0};

  if(target_freq < freq_min() || target_freq > freq_max()) {
    return ret;
  }
        
  double target_lo_freq = target_freq + d_IF;  // High side mixing

  int divisor = (int)(0.5+(target_lo_freq / d_f_ref));
  double actual_lo_freq = d_f_ref*divisor;
  
  if((divisor & ~0x7fff) != 0) {		// must be 15-bits or less
    return ret;
  }
  
  // build i2c command string
  std::vector<int> buf(6);
  buf[0] = (divisor >> 8) & 0xff;          // DB1
  buf[1] = divisor & 0xff;                 // DB2
  buf[2] = control_byte_4();
  buf[3] = control_byte_5(target_freq);
  buf[4] = control_byte_6();
  buf[5] = control_byte_7();

  bool ok = usrp()->write_i2c(d_i2c_addr, int_seq_to_str (buf));

  d_freq = actual_lo_freq - d_IF;
  
  ret.ok = ok;
  ret.baseband_freq = actual_lo_freq;

  return ret;

}
  
bool
db_dtt768::is_quadrature()
{
  /*
   * Return True if this board requires both I & Q analog channels.
   * 
   * This bit of info is useful when setting up the USRP Rx mux register.
   */
     
  return false;
}

bool
db_dtt768::spectrum_inverted()
{
  /*
   * The 43.75 MHz version is inverted
   */
  
  return d_inverted;
}

bool
db_dtt768::set_bw(float bw)
{
  /*
   * Choose the SAW filter bandwidth, either 7MHz or 8MHz)
   */

  d_bw = bw;
  set_freq(d_freq);

  return true; // FIXME: propagate set_freq result
}

void
db_dtt768::_set_rfagc(float gain)
{
  assert(gain <= 60 && gain >= 0);
  // FIXME this has a 0.5V step between gain = 60 and gain = 59.
  // Why are there two cases instead of a single linear case?
  float voltage;
  if(gain == 60) {
    voltage = 4;
  }
  else {
    voltage = gain/60.0 * 2.25 + 1.25;
  }
  
  int dacword = (int)(4096*voltage/1.22/3.3);    // 1.22 = opamp gain
    
  assert(dacword>=0 && dacword<4096);
  usrp()->write_aux_dac(d_which, 1, dacword);
}

void
db_dtt768::_set_ifagc(float gain)
{
  assert(gain <= 35 && gain >= 0);
  float voltage = gain/35.0 * 2.1 + 1.4;
  int dacword = (int)(4096*voltage/1.22/3.3);    // 1.22 = opamp gain

  assert(dacword>=0 && dacword<4096);
  usrp()->write_aux_dac(d_which, 0, dacword);
}

void
db_dtt768::_set_pga(float pga_gain)
{
  assert(pga_gain >=0 && pga_gain <=20);
  if(d_which == 0) {
    usrp()->set_pga (0, pga_gain);
  }
  else {
    usrp()->set_pga (2, pga_gain);
  }
}

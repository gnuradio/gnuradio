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

#include <usrp/db_dtt754.h>
#include <db_base_impl.h>

int
control_byte_1()
{
  int RS = 0;  // 0 = 166.66kHz reference
  int ATP = 7; // Disable internal AGC
  return (0x80 | ATP<<3 | RS);
}

int
control_byte_2()
{
  int STBY = 0;  // powered on
  int XTO = 1;   // turn off xtal out, which we don't have
  int ATC = 0;   // not clear exactly, possibly speeds up or slows down AGC, which we are not using
  
  int c = 0xc2 | ATC<<5 | STBY<<4 | XTO;
  return c;
}

int
bandswitch_byte(float freq, float bw)
{
  int P5, CP, BS;

  if(bw>7.5e6) {
    P5 = 1;
  }
  else {
    P5 = 0;
  }

  if(freq < 121e6) {
    CP = 0;
    BS = 1;
  }
  else if(freq < 141e6) {
    CP = 1;
    BS = 1;
  }
  else if(freq < 166e6) {
    CP = 2;
    BS = 1;
  }
  else if(freq < 182e6) {
    CP = 3;
    BS = 1;
  }
  else if(freq < 286e6) {
    CP = 0;
    BS = 2;
  }
  else if(freq < 386e6) {
    CP = 1;
    BS = 2;
  }
  else if(freq < 446e6) {
    CP = 2;
    BS = 2;
  }
  else if(freq < 466e6) {
    CP = 3;
    BS = 2;
  }
  else if(freq < 506e6) {
    CP = 0;
    BS = 8;
  }
  else if(freq < 761e6) {
    CP = 1;
    BS = 8;
  }
  else if(freq < 846e6) {
    CP = 2;
    BS = 8;
  }
  else { // limit is ~905 MHz
    CP = 3;
    BS = 8;
  }
  return (CP<<6 | P5 << 4 | BS);
}

db_dtt754::db_dtt754(usrp_basic_sptr _usrp, int which)
  : db_base(_usrp, which)
{
  /*
   * Control custom DTT75403-based daughterboard.
   * 
   * @param usrp: instance of usrp.source_c
   * @param which: which side: 0 or 1 corresponding to RX_A or RX_B respectively
   * @type which: int
   */

  // FIXME: DTT754 and DTT768 can probably inherit from a DTT class
  
  if(d_which == 0) {
    d_i2c_addr = 0x60;
  }
  else {
    d_i2c_addr = 0x62;
  }

  d_bw = 7e6;
  d_IF = 36e6;
        
  d_f_ref = 166.6666e3;
  d_inverted = false;

  set_gain((gain_min() + gain_max()) / 2.0);

  bypass_adc_buffers(false);
}

db_dtt754::~db_dtt754()
{
}
  
float
db_dtt754::gain_min()
{
  return 0;
}

float
db_dtt754::gain_max()
{
  return 115;
}

float
db_dtt754::gain_db_per_step()
{
  return 1;
}

bool
db_dtt754::set_gain(float gain)
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

  return true; // can't fail with the assert in place
}

double
db_dtt754::freq_min()
{
  return 44e6;
}

double
db_dtt754::freq_max()
{
  return 900e6;
}

struct freq_result_t
db_dtt754::set_freq(double target_freq)
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
  std::vector<int> buf(5);
  buf[0] = (divisor >> 8) & 0xff;          // DB1
  buf[1] = divisor & 0xff;                 // DB2
  buf[2] = control_byte_1();
  buf[3] = bandswitch_byte(actual_lo_freq, d_bw);
  buf[4] = control_byte_2();

  bool ok = usrp()->write_i2c(d_i2c_addr, int_seq_to_str (buf));

  d_freq = actual_lo_freq - d_IF;
        
  ret.ok = ok;
  ret.baseband_freq = actual_lo_freq;

  return ret;

}
  
bool
db_dtt754::is_quadrature()
{
  /*
   * Return True if this board requires both I & Q analog channels.
   * 
   * This bit of info is useful when setting up the USRP Rx mux register.
   */
     
  return false;
}

bool
db_dtt754::spectrum_inverted()
{
  /*
   * The 43.75 MHz version is inverted
   */
  
  return d_inverted;
}

bool
db_dtt754::set_bw(float bw)
{
  /*
   * Choose the SAW filter bandwidth, either 7MHz or 8MHz)
   */

  d_bw = bw;
  set_freq(d_freq);

  return true; // FIXME: propagate set_freq result
}

void
db_dtt754::_set_rfagc(float gain)
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
db_dtt754::_set_ifagc(float gain)
{
  assert(gain <= 35 && gain >= 0);
  float voltage = gain/35.0 * 2.1 + 1.4;
  int dacword = (int)(4096*voltage/1.22/3.3);    // 1.22 = opamp gain

  assert(dacword>=0 && dacword<4096);
  usrp()->write_aux_dac(d_which, 0, dacword);
}

void
db_dtt754::_set_pga(float pga_gain)
{
  assert(pga_gain >=0 && pga_gain <=20);
  if(d_which == 0) {
    usrp()->set_pga (0, pga_gain);
  }
  else {
    usrp()->set_pga (2, pga_gain);
  }
}

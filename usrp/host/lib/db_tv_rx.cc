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

#include <usrp/db_tv_rx.h>
#include <db_base_impl.h>

/*****************************************************************************/

int
control_byte_1(bool fast_tuning_p, int reference_divisor)
{
  int c = 0x88;
  if(fast_tuning_p) {
    c |= 0x40;
  }

  if(reference_divisor == 512) {
    c |= 0x3 << 1;
  }
  else if(reference_divisor == 640) {
    c |= 0x0 << 1;
  }
  else if(reference_divisor == 1024) {
    c |= 0x1 << 1;
  }
  else {
    assert(0);
  }

  return c;
}

int
control_byte_2(double target_freq, bool shutdown_tx_PGA)
{
  int c;
  if(target_freq < 158e6) {        // VHF low
    c = 0xa0;
  }
  else if(target_freq < 464e6) {   // VHF high
    c = 0x90;
  }
  else {                           // UHF
    c = 0x30;
  }

  if(shutdown_tx_PGA) {
    c |= 0x08;
  }

  return c;
}


/*****************************************************************************/


db_tv_rx::db_tv_rx(usrp_basic_sptr usrp, int which,
		   double first_IF, double second_IF)
  : db_base(usrp, which)
{
  // Handler for Tv Rx daughterboards.
  // 
  // @param usrp: instance of usrp.source_c
  // @param which: which side: 0, 1 corresponding to RX_A or RX_B respectively

  if(which == 0) {
    d_i2c_addr = 0x60;
  }
  else {
    d_i2c_addr = 0x61;
  }

  d_first_IF = first_IF;
  d_second_IF = second_IF;
  d_reference_divisor = 640;
  d_fast_tuning = false;
  d_inverted = false;                     // FIXME get rid of this
  
  set_gain((gain_min() + gain_max()) / 2.0);       // initialize gain

  bypass_adc_buffers(false);
}

db_tv_rx::~db_tv_rx()
{
}

// Gain setting
void
db_tv_rx::_set_rfagc(float gain)
{
  float voltage;

  assert(gain <= 60 && gain >= 0);
  // FIXME this has a 0.5V step between gain = 60 and gain = 59.
  // Why are there two cases instead of a single linear case?
  if(gain == 60) {
    voltage = 4;
  }
  else {
    voltage = gain/60.0 * 2.25 + 1.25;
  }
  int dacword = int(4096*voltage/1.22/3.3);    // 1.22 = opamp gain

  assert(dacword>=0 && dacword<4096);
  usrp()->write_aux_dac(d_which, 1, dacword);
}

void
db_tv_rx::_set_ifagc(float gain)
{
  float voltage;

  assert(gain <= 35 && gain >= 0);
  voltage = gain/35.0 * 2.1 + 1.4;
  int dacword = int(4096*voltage/1.22/3.3);    // 1.22 = opamp gain
  
  assert(dacword>=0 && dacword<4096);
  usrp()->write_aux_dac(d_which, 0, dacword);
}

void
db_tv_rx::_set_pga(float pga_gain)
{
  assert(pga_gain >=0 && pga_gain <=20);
  if(d_which == 0) {
    usrp()->set_pga(0, pga_gain);
  }
  else {
    usrp()->set_pga (2, pga_gain);
  }
}           

double
db_tv_rx::freq_min()
{
  return 50e6;
}

double
db_tv_rx::freq_max()
{
  return 860e6;
}

struct freq_result_t
db_tv_rx::set_freq(double target_freq)
{
  // Set the frequency.
  // 
  // @param freq:  target RF frequency in Hz
  // @type freq:   double
  // 
  // @returns (ok, actual_baseband_freq) where:
  //   ok is True or False and indicates success or failure,
  //   actual_baseband_freq is RF frequency that corresponds to DC in the IF.
  
  freq_result_t args = {false, 0};

  double fmin = freq_min();
  double fmax = freq_max();
  if((target_freq < fmin) || (target_freq > fmax)) {
    return args;
  }
  
  double target_lo_freq = target_freq + d_first_IF;    // High side mixing
  double f_ref = 4.0e6 / (double)(d_reference_divisor); // frequency steps

  int divisor = int((target_lo_freq + (f_ref * 4)) / (f_ref * 8));  
  double actual_lo_freq = (f_ref * 8 * divisor);
  double actual_freq = actual_lo_freq - d_first_IF;

  if((divisor & ~0x7fff) != 0) {		// must be 15-bits or less
    return args;
  }

  // build i2c command string
  std::vector<int> buf(4);
  buf[0] = (divisor >> 8) & 0xff;         // DB1
  buf[1] = divisor & 0xff;                // DB2
  buf[2] = control_byte_1(d_fast_tuning, d_reference_divisor);
  buf[3] = control_byte_2(actual_freq, true);

  args.ok = usrp()->write_i2c(d_i2c_addr, int_seq_to_str (buf));
  args.baseband_freq = actual_freq - d_second_IF;
  return args;
}

float
db_tv_rx::gain_min()
{
  return 0;
}

float
db_tv_rx::gain_max()
{
  return 115;
}

float
db_tv_rx::gain_db_per_step()
{
  return 1;
}

bool 
db_tv_rx::set_gain(float gain)
{
  // Set the gain.
  // 
  // @param gain:  gain in decibels
  // @returns True/False

  float rfgain, ifgain, pgagain;

  assert(gain>=0 && gain<=115);
  if(gain>60) {
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

bool 
db_tv_rx::is_quadrature()
{
  // Return True if this board requires both I & Q analog channels.  
  return false;
}

bool
db_tv_rx::spectrum_inverted() 
{
  // The 43.75 MHz version is inverted
  return d_inverted;
}

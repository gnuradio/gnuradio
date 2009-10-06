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

#include <usrp/db_dbs_rx.h>
#include <db_base_impl.h>
#include <cmath>
#include <cstdio>


/*****************************************************************************/


db_dbs_rx::db_dbs_rx(usrp_basic_sptr _usrp, int which)
  : db_base(_usrp, which)
{
  // Control DBS receiver based USRP daughterboard.
  // 
  // @param usrp: instance of usrp.source_c
  // @param which: which side: 0, 1 corresponding to RX_A or RX_B respectively

  usrp()->_write_oe(d_which, 0x0001, 0x0001);
  if(which == 0) {
    d_i2c_addr = 0x67;
  }
  else {
    d_i2c_addr = 0x65;
  }

  d_n = 950;
  d_div2 = 0;
  d_osc = 5;
  d_cp = 3;
  d_r = 4;
  d_r_int = 1;
  d_fdac = 127;
  d_m = 2;
  d_dl = 0;
  d_ade = 0;
  d_adl = 0;
  d_gc1 = 0;
  d_gc2 = 31;
  d_diag = 0;
  
  _enable_refclk(true);
  
  set_gain((gain_min() + gain_max()) / 2.0);       // initialize gain

  bypass_adc_buffers(true);
}

db_dbs_rx::~db_dbs_rx()
{
  shutdown();
}

void
db_dbs_rx::shutdown()
{
  if (!d_is_shutdown){
    d_is_shutdown = true;
    // do whatever there is to do to shutdown orderly
    _enable_refclk(false);
  }
}

void
db_dbs_rx::_write_reg (int regno, int v)
{
  //regno is in [0,5], v is value to write to register"""
  assert (0 <= regno && regno <= 5);
  std::vector<int> args(2);
  args[0] = regno;
  args[1] = v;
  usrp()->write_i2c (d_i2c_addr, int_seq_to_str (args));
}

void
db_dbs_rx::_write_regs (int starting_regno, const std::vector<int> &vals)
{
  // starting_regno is in [0,5],
  // vals is a seq of integers to write to consecutive registers"""

  //FIXME
  std::vector<int> args;
  args.push_back(starting_regno);
  args.insert(args.end(), vals.begin(), vals.end());
  usrp()->write_i2c (d_i2c_addr, int_seq_to_str (args));
}
        
std::vector<int>
db_dbs_rx::_read_status ()
{
  //If successful, return list of two ints: [status_info, filter_DAC]"""
  std::string s = usrp()->read_i2c (d_i2c_addr, 2);
  if(s.size() != 2) {
    std::vector<int> ret(0);
    return ret;
  }
  return str_to_int_seq (s);
}

void
db_dbs_rx::_send_reg(int regno)
{
  assert(0 <= regno && regno <= 5);
  if(regno == 0)
    _write_reg(0,(d_div2<<7) + (d_n>>8));
  if(regno == 1)
    _write_reg(1,d_n & 255);
  if(regno == 2)
    _write_reg(2,d_osc + (d_cp<<3) + (d_r_int<<5));
  if(regno == 3)
    _write_reg(3,d_fdac);
  if(regno == 4)
    _write_reg(4,d_m + (d_dl<<5) + (d_ade<<6) + (d_adl<<7));
  if(regno == 5)
    _write_reg(5,d_gc2 + (d_diag<<5));
}

// BW setting
void
db_dbs_rx::_set_m(int m)
{
  assert(m>0 && m<32);
  d_m = m;
  _send_reg(4);
}
  
void
db_dbs_rx::_set_fdac(int fdac)
{
  assert(fdac>=0 && fdac<128);
  d_fdac = fdac;
  _send_reg(3);
}

bool
db_dbs_rx::set_bw (float bw)
{
  if (bw < 1e6 || bw > 33e6) {
    fprintf(stderr, "db_dbs_rx::set_bw: bw (=%f) must be between 1e6 and 33e6 inclusive\n", bw);
    return false;
  }

  // struct bw_t ret = {0, 0, 0};
  int m_max, m_min, m_test, fdac_test;
  if(bw >= 4e6)
    m_max = int(std::min(31, (int)floor(_refclk_freq()/1e6)));
  else if(bw >= 2e6)      // Outside of Specs!
    m_max = int(std::min(31, (int)floor(_refclk_freq()/.5e6)));
  else      // Way outside of Specs!
    m_max = int(std::min(31, (int)floor(_refclk_freq()/.25e6)));
  
  m_min = int(ceil(_refclk_freq()/2.5e6));
  m_test = m_max;
  while(m_test >= m_min) {
    fdac_test = static_cast<int>(round(((bw * m_test / _refclk_freq())-4)/.145));
    if(fdac_test > 127)
      m_test = m_test - 1;
    else
      break;
  }

  if(m_test>=m_min && fdac_test>=0) {
    _set_m(m_test);
    _set_fdac(fdac_test);

    //ret.m = d_m;
    //ret.fdac = d_fdac;
    //ret.div = _refclk_freq()/d_m*(4+0.145*d_fdac);
  }
  else {
    fprintf(stderr, "db_dbs_rx::set_bw: failed\n");
    return false;
  }

  return true;
}

// Gain setting
void
db_dbs_rx::_set_dl(int dl)
{
  assert(dl == 0 || dl == 1);
  d_dl = dl;
  _send_reg(4);
}

void
db_dbs_rx::_set_gc2(int gc2)
{
  assert(gc2<32 && gc2>=0);
  d_gc2 = gc2;
  _send_reg(5);
}

void
db_dbs_rx::_set_gc1(int gc1)
{
  assert(gc1>=0 && gc1<4096);
  d_gc1 = gc1;
  usrp()->write_aux_dac(d_which, 0, gc1);
}

void
db_dbs_rx::_set_pga(int pga_gain)
{
  assert(pga_gain>=0 && pga_gain<=20);
  if(d_which == 0) {
    usrp()->set_pga (0, pga_gain);
    usrp()->set_pga (1, pga_gain);
  }
  else {
    usrp()->set_pga (2, pga_gain);
    usrp()->set_pga (3, pga_gain);
  }
}

float
db_dbs_rx::gain_min()
{
  return 0;
}

float
db_dbs_rx::gain_max()
{
  return 104;
}

float
db_dbs_rx::gain_db_per_step()
{
  return 1;
}

bool 
db_dbs_rx::set_gain(float gain)
{
  // Set the gain.
  // 
  // @param gain:  gain in decibels
  // @returns True/False

  if(!(gain>=0 && gain<105)) {
    throw std::runtime_error("gain out of range\n");
  }

  int gc1=0, gc2=0, dl=0, pga=0;

  if(gain < 56) {
    gc1 = int((-gain*1.85/56.0 + 2.6)*4096.0/3.3);
    gain = 0;
  }
  else {
    gc1 = 0;
    gain -= 56;
  }
   
  if(gain < 24) {
    gc2 = static_cast<int>(round(31.0 * (1-gain/24.0)));
    gain = 0;
  }
  else {
    gc2 = 0;
    gain -=24;
  }
  
  if(gain >= 4.58) {
    dl = 1;
    gain -= 4.58;
  }

  pga = gain;
  _set_gc1(gc1);
  _set_gc2(gc2);
  _set_dl(dl);
  _set_pga(pga);

  return true;
}

// Frequency setting
void
db_dbs_rx::_set_osc(int osc)
{
  assert(osc>=0 && osc<8);
  d_osc = osc;
  _send_reg(2);
}

void
db_dbs_rx::_set_cp(int cp)
{
  assert(cp>=0 && cp<4);
  d_cp = cp;
  _send_reg(2);
}

void
db_dbs_rx::_set_n(int n)
{
  assert(n>256 && n<32768);
  d_n = n;
  _send_reg(0);
  _send_reg(1);
}

void
db_dbs_rx::_set_div2(int div2)
{
  assert(div2 == 0 || div2 == 1);
  d_div2 = div2;
  _send_reg(0);
}

void
db_dbs_rx::_set_r(int r)
{
  assert(r>=0 && r<128);
  d_r = r;
  d_r_int = static_cast<int>(round(log10(r)/log10(2)) - 1);
  _send_reg(2);
}

// FIXME  How do we handle ADE and ADL properly?
void
db_dbs_rx::_set_ade(int ade)
{
  assert(ade == 0 || ade == 1);
  d_ade = ade;
  _send_reg(4);
}

double
db_dbs_rx::freq_min()
{
  return 500e6;
}

double
db_dbs_rx::freq_max()
{
  return 2.6e9;
}

struct freq_result_t
db_dbs_rx::set_freq(double freq)
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
  
  if(!(freq>=freq_min() && freq<=freq_max())) {
    return args;
  }
  
  double vcofreq;
  if(freq<1150e6) {
    _set_div2(0);
    vcofreq = 4 * freq;
  }
  else {
    _set_div2(1);
    vcofreq = 2 * freq;
  }
  
  _set_ade(1);
  int rmin = std::max(2, (int)(_refclk_freq()/2e6));
  int rmax = std::min(128, (int)(_refclk_freq()/500e3));
  int r = 2;
  int n = 0;
  int best_r = 2;
  int best_n = 0;
  int best_delta = 10e6;
  int delta;
  
  while(r <= rmax) {
    n = static_cast<int>(round(freq/(_refclk_freq()/r)));
    if(r<rmin || n<256) {
      r = r * 2;
      continue;
    }
    delta = (int)fabs(n*_refclk_freq()/r - freq);
    if(delta < 75e3) {
      best_r = r;
      best_n = n;
      break;
    }
    if(delta < best_delta*0.9) {
      best_r = r;
      best_n = n;
      best_delta = delta;
    }
    r = r * 2;
  }
  _set_r(best_r);

  _set_n(static_cast<int>(round(best_n)));
 
  int vco;
  if(vcofreq < 2433e6)
    vco = 0;
  else if(vcofreq < 2711e6)
    vco=1;
  else if(vcofreq < 3025e6)
    vco=2;
  else if(vcofreq < 3341e6)
    vco=3;
  else if(vcofreq < 3727e6)
    vco=4;
  else if(vcofreq < 4143e6)
    vco=5;
  else if(vcofreq < 4493e6)
    vco=6;
  else
    vco=7;
  
  _set_osc(vco);
  
  // Set CP current
  int adc_val = 0;
  std::vector<int> bytes(2);
  while(adc_val == 0 || adc_val == 7) {
    bytes = _read_status();
    adc_val = bytes[0] >> 2;
    if(adc_val == 0) {
      if(vco <= 0) {
	return args;
      }
      else {
	vco = vco - 1;
      }
    }
    else if(adc_val == 7) {
      if(vco >= 7) {
	return args;
      }
      else {
	vco = vco + 1;
      }
    }
    _set_osc(vco);
  }
  
  if(adc_val == 1 || adc_val == 2) {
    _set_cp(1);
  }
  else if(adc_val == 3 || adc_val == 4) {
    _set_cp(2);
  }
  else {
    _set_cp(3);
  }
  
  args.ok = true;
  args.baseband_freq = d_n * _refclk_freq() / d_r;
  return args;
}

int
db_dbs_rx::_refclk_divisor()
{
  //Return value to stick in REFCLK_DIVISOR register
  return 16;
}

bool 
db_dbs_rx::is_quadrature()
{
  // Return True if this board requires both I & Q analog channels.  
  return true;
}

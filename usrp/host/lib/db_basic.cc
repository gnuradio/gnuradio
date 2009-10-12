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

#include <usrp/db_basic.h>
#include <db_base_impl.h>

db_basic_tx::db_basic_tx(boost::shared_ptr<usrp_basic> usrp, int which)
  : db_base(usrp, which)
{
  // Handler for Basic Tx daughterboards.
  // 
  // @param usrp: instance of usrp.source_c
  // @param which: which side: 0 or 1 corresponding to TX_A or TX_B respectively

  set_gain((gain_min() + gain_max()) / 2);         // initialize gain
}

db_basic_tx::~db_basic_tx()
{
}

double 
db_basic_tx::freq_min() 
{
  return -90e9;
}

double 
db_basic_tx::freq_max() 
{
  return 90e9;
}

struct freq_result_t 
db_basic_tx::set_freq(double target_freq)
{
  // Set the frequency.
  // 
  // @param freq:  target RF frequency in Hz
  // @type freq:   double
  // 
  // @returns (ok, actual_baseband_freq) where:
  //   ok is True or False and indicates success or failure,
  //   actual_baseband_freq is the RF frequency that corresponds to DC in the IF.
  
  struct freq_result_t args = {false, 0};
  args.ok = true;
  args.baseband_freq = 0.0;
  return args;
}

float
db_basic_tx::gain_min()
{
  return usrp()->pga_min();
}

float
db_basic_tx::gain_max()
{
  return usrp()->pga_max();
}

float
db_basic_tx::gain_db_per_step()
{
  return usrp()->pga_db_per_step();
}

bool 
db_basic_tx::set_gain(float gain)
{
  // Set the gain.
  // 
  // @param gain:  gain in decibels
  // @returns True/False

  bool ok = usrp()->set_pga(d_which * 2 + 0, gain);
  ok = ok && usrp()->set_pga(d_which * 2 + 1, gain);
  return ok;
}

bool 
db_basic_tx::is_quadrature()
{
  // Return True if this board requires both I & Q analog channels.
  
  return true;
}


/******************************************************************************/


db_basic_rx::db_basic_rx(usrp_basic_sptr usrp, int which, int subdev)
  : db_base(usrp, which)
{
  // Handler for Basic Rx daughterboards.
  // 
  // @param usrp: instance of usrp.source_c
  // @param which: which side: 0 or 1 corresponding to TX_A or TX_B respectively
  // @param subdev: which analog i/o channel: 0 or 1
  // @type subdev: int
  
  d_subdev = subdev;
    
  bypass_adc_buffers(true);

  if(0) {       // Doing this would give us a different default than the historical values...
    set_gain(float(gain_min() + gain_max()) / 2.0);       // initialize gain
  }
}

db_basic_rx::~db_basic_rx()
{
}

double
db_basic_rx::freq_min() 
{
  return -90e9;
}

double
db_basic_rx::freq_max()
{
  return 90e9;
}

struct freq_result_t 
db_basic_rx::set_freq(double target_freq)
{
  // Set the frequency.
  // 
  // @param freq:  target RF frequency in Hz
  // @type freq:   double
  // 
  // @returns (ok, actual_baseband_freq) where:
  //   ok is True or False and indicates success or failure,
  //   actual_baseband_freq is the RF frequency that corresponds to DC in the IF.
  
  struct freq_result_t args = {true, 0.0};
  return args;
}

float
db_basic_rx::gain_min()
{
  return usrp()->pga_min();
}

float
db_basic_rx::gain_max()
{
  return usrp()->pga_max();
}

float
db_basic_rx::gain_db_per_step()
{
  return usrp()->pga_db_per_step();
}

bool 
db_basic_rx::set_gain(float gain)
{
  // Set the gain.
  // 
  // @param gain:  gain in decibels
  // @returns True/False
  
  return usrp()->set_pga(d_which * 2 + d_subdev, gain);
}

bool 
db_basic_rx::is_quadrature()
{
  // Return True if this board requires both I & Q analog channels.

  // This bit of info is useful when setting up the USRP Rx mux register.
  
  return (d_subdev == 2);
}



/******************************************************************************/


db_lf_tx::db_lf_tx(usrp_basic_sptr usrp, int which)
  : db_basic_tx(usrp, which)
{
  // Handler for Low Freq Tx daughterboards.
  //
  // @param usrp: instance of usrp.source_c
  // @param which: which side: 0 or 1 corresponding to RX_A or RX_B respectively
}

db_lf_tx::~db_lf_tx()
{
}

double 
db_lf_tx::freq_min() 
{
  return -32e6;
}

double 
db_lf_tx::freq_max()
{
  return 32e6;
}

/******************************************************************************/


db_lf_rx::db_lf_rx(usrp_basic_sptr usrp, int which, int subdev)
  : db_basic_rx(usrp, which, subdev)
{
  // Handler for Low Freq Rx daughterboards.
  //
  // @param usrp: instance of usrp.source_c
  // @param which: which side: 0 or 1 corresponding to RX_A or RX_B respectively
  // @param subdev: which analog i/o channel: 0 or 1
  // @type subdev: int
}

db_lf_rx::~db_lf_rx()
{
}

double
db_lf_rx::freq_min() 
{
  return 0.0;
}

double
db_lf_rx::freq_max() 
{
  return 32e6;
}



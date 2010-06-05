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
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <usrp/db_base.h>
#include <db_base_impl.h>

#if 0
tune_result::tune_result(double baseband, double dxc, double residual, bool inv)
  : ok(false), baseband_freq(baseband), dxc_freq(dxc), 
    residual_freq(residual), inverted(inv)
{
}

tune_result::~tune_result()
{ 
}
#endif


/*****************************************************************************/

db_base::db_base(usrp_basic_sptr usrp, int which)
  : d_is_shutdown(false), d_raw_usrp(usrp.get()), d_which(which), d_lo_offset(0.0)
{
}

db_base::~db_base()
{
  shutdown();
}

void
db_base::shutdown()
{
  if (!d_is_shutdown){
    d_is_shutdown = true;
    // do whatever there is to do to shutdown
  }
}

int 
db_base::dbid()
{
  return usrp()->daughterboard_id(d_which);
}

std::string 
db_base::name()
{
  return usrp_dbid_to_string(dbid());
}

std::string 
db_base::side_and_name()
{
  if(d_which == 0)
    return "A: " + name();
  else
    return "B: " + name();
}

// Function to bypass ADC buffers. Any board which is DC-coupled
// should bypass the buffers

bool
db_base::bypass_adc_buffers(bool bypass)
{
  //if(d_tx) {
  //  throw  std::runtime_error("TX Board has no adc buffers\n");
  //}

  bool ok = true;
  if(d_which==0) {
    ok &= usrp()->set_adc_buffer_bypass(0, bypass);
    ok &= usrp()->set_adc_buffer_bypass(1, bypass);
  }
  else {
    ok &= usrp()->set_adc_buffer_bypass(2, bypass);
    ok &= usrp()->set_adc_buffer_bypass(3, bypass);
  }
  return ok;
}

bool 
db_base::set_atr_mask(int v)
{
  // Set Auto T/R mask.
  return usrp()->write_atr_mask(d_which, v);
}

bool 
db_base::set_atr_txval(int v)
{
  // Set Auto T/R register value to be used when transmitting.
  return usrp()->write_atr_txval(d_which, v);
}
  
bool 
db_base::set_atr_rxval(int v)
{
  // Set Auto T/R register value to be used when receiving.
  return usrp()->write_atr_rxval(d_which, v);
}
  
bool 
db_base::set_atr_tx_delay(int v)
{
  // Set Auto T/R delay (in clock ticks) from when Tx fifo gets data to 
  // when T/R switches.
  return usrp()->write_atr_tx_delay(v);
}

bool 
db_base::set_atr_rx_delay(int v)
{
  // Set Auto T/R delay (in clock ticks) from when Tx fifo goes empty to 
  // when T/R switches.
  return usrp()->write_atr_rx_delay(v);
}

bool
db_base::i_and_q_swapped()
{
  // Return True if this is a quadrature device and (for RX) ADC 0 is Q
  // or (for TX) DAC 0 is Q
  return false;
}

bool 
db_base::spectrum_inverted()
{
  // Return True if the dboard gives an inverted spectrum
  
  return false;
}

bool
db_base::set_enable(bool on)
{
  // For tx daughterboards, this controls the transmitter enable.

  return true;	// default is nop
}

bool
db_base::set_auto_tr(bool on)
{
  // Enable automatic Transmit/Receive switching (ATR).
  // 
  // Should be overridden in subclasses that care.  This will typically
  // set the atr_mask, txval and rxval.

  return true;
}

bool
db_base::set_lo_offset(double offset)
{
  // Set how much LO is offset from requested frequency

  d_lo_offset = offset;
  return true;
}

bool
db_base::select_rx_antenna(int which_antenna)
{
  // Specify which antenna port to use for reception.
  // Should be overriden by daughterboards that care.

  return which_antenna == 0;
}

bool
db_base::select_rx_antenna(const std::string &which_antenna)
{
  // Specify which antenna port to use for reception.
  // Should be overriden by daughterboards that care.

  return which_antenna == "";
}


// Reference Clock section
//
// Control whether a reference clock is sent to the daughterboards,
// and what frequency
//
// Bit 7  -- 1 turns on refclk, 0 allows IO use
// Bits 6:0 Divider value
//
    
double
db_base::_refclk_freq() 
{
  return usrp()->fpga_master_clock_freq() / _refclk_divisor();
}

void 
db_base::_enable_refclk(bool enable)
{
  int CLOCK_OUT = 1;   // Clock is on lowest bit
  int REFCLK_ENABLE = 0x80;
  int REFCLK_DIVISOR_MASK = 0x7f;

  if(enable) {
    usrp()->_write_oe(d_which, CLOCK_OUT, CLOCK_OUT); // output enable
    usrp()->write_refclk(d_which, (_refclk_divisor() & REFCLK_DIVISOR_MASK) | REFCLK_ENABLE);
  }
  else {
    usrp()->write_refclk(d_which, 0);
  }
}

int 
db_base::_refclk_divisor()
{
  // Return value to stick in REFCLK_DIVISOR register
  throw std::runtime_error("_refclk_divisor() called from base class\n");;
}

bool
db_base::set_bw(float bw)
{
  // Set baseband bandwidth (board specific)
  // Should be overriden by boards that implement variable IF filtering (e.g., DBSRX)
  return false;
}

std::ostream &operator<<(std::ostream &os, db_base &x)
{
  os << x.side_and_name();
  return os;
}

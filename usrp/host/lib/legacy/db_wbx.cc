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

#include <db_wbx.h>
#include <fpga_regs_standard.h>
#include <fpga_regs_common.h>
#include <usrp_prims.h>
#include <usrp_spi_defs.h>
#include <stdexcept>
#include <cmath>

// d'board i/o pin defs

// TX IO Pins
#define TX_POWER        (1 << 0)  // TX Side Power
#define RX_TXN          (1 << 1)  // T/R antenna switch for TX/RX port
#define TX_ENB_MIX      (1 << 2)  // Enable IQ mixer
#define TX_ENB_VGA      (1 << 3)

// RX IO Pins
#define RX2_RX1N        (1 << 0)  // antenna switch between RX2 and TX/RX port
#define RXENABLE        (1 << 1)  // enables mixer
#define PLL_LOCK_DETECT (1 << 2)  // Muxout pin from PLL -- MUST BE INPUT
#define MReset          (1 << 3)  // NB6L239 Master Reset, asserted low
#define SELA0           (1 << 4)  // NB6L239 SelA0
#define SELA1           (1 << 5)  // NB6L239 SelA1
#define SELB0           (1 << 6)  // NB6L239 SelB0
#define SELB1           (1 << 7)  // NB6L239 SelB1
#define PLL_ENABLE      (1 << 8)  // CE Pin on PLL
#define AUX_SCLK        (1 << 9)  // ALT SPI SCLK
#define AUX_SDO         (1 << 10) // ALT SPI SDO
#define AUX_SEN         (1 << 11) // ALT SPI SEN


wbx_base::wbx_base(usrp_basic_sptr usrp, int which)
  : db_base(usrp, which)
{
  /*
   * @param usrp: instance of usrp.source_c
   * @param which: which side: 0 or 1 corresponding to side A or B respectively
   * @type which: int
   */

  d_first = true;
  d_spi_format = SPI_FMT_MSB | SPI_FMT_HDR_0;

  // FIXME -- the write reg functions don't work with 0xffff for masks
  _rx_write_oe(int(PLL_ENABLE|MReset|SELA0|SELA1|SELB0|SELB1|RX2_RX1N|RXENABLE), 0x7fff);
  _rx_write_io((PLL_ENABLE|MReset|0|RXENABLE), (PLL_ENABLE|MReset|RX2_RX1N|RXENABLE));

  _tx_write_oe((TX_POWER|RX_TXN|TX_ENB_MIX|TX_ENB_VGA), 0x7fff);
  _tx_write_io((0|RX_TXN), (TX_POWER|RX_TXN|TX_ENB_MIX|TX_ENB_VGA));  // TX off, TR switch set to RX

  if(d_which == 0) {
    d_spi_enable = SPI_ENABLE_RX_A;
  }
  else {
    d_spi_enable = SPI_ENABLE_RX_B;
  }

  set_auto_tr(false);
        
}

wbx_base::~wbx_base()
{        
  shutdown();
}


void
wbx_base::shutdown()
{
  if (!d_is_shutdown){
    d_is_shutdown = true;
    // do whatever there is to do to shutdown

    write_io(d_which, d_power_off, POWER_UP);   // turn off power to board
    _write_oe(d_which, 0, 0xffff);   // turn off all outputs
    set_auto_tr(false); // disable auto transmit
  }
}

bool
wbx_base::_lock_detect()
{
  /*
   * @returns: the value of the VCO/PLL lock detect bit.
   * @rtype: 0 or 1
   */

  if(_rx_read_io() & PLL_LOCK_DETECT) {
    return true;
  }
  else {     // Give it a second chance
    if(_rx_read_io() & PLL_LOCK_DETECT) {
      return true;
    }
    else {
      return false;
    }
  }
}

bool 
wbx_base::_tx_write_oe(int value, int mask)
{
  int reg = (d_which == 0 ? FR_OE_0 : FR_OE_2);
  return d_usrp->_write_fpga_reg(reg, ((mask & 0xffff) << 16) | (value & 0xffff));
}

bool 
wbx_base::_rx_write_oe(int value, int mask)
{
  int reg = (d_which == 0 ? FR_OE_1 : FR_OE_3);
  return d_usrp->_write_fpga_reg(reg, ((mask & 0xffff) << 16) | (value & 0xffff));
}

bool
wbx_base::_tx_write_io(int value, int mask)
{
  int reg = (d_which == 0 ? FR_IO_0 : FR_IO_2);
  return d_usrp->_write_fpga_reg(reg, ((mask & 0xffff) << 16) | (value & 0xffff));
}

bool
wbx_base::_rx_write_io(int value, int mask)
{
  int reg = (d_which == 0 ? FR_IO_1 : FR_IO_3);
  return d_usrp->_write_fpga_reg(reg, ((mask & 0xffff) << 16) | (value & 0xffff));
}

bool
wbx_base::_rx_read_io()
{
  int reg = (d_which == 0 ? FR_RB_IO_RX_A_IO_TX_A : FR_RB_IO_RX_B_IO_TX_B);
  int t = d_usrp->_read_fpga_reg(reg);
  return (t >> 16) & 0xffff;
}

bool
wbx_base::_tx_read_io()
{
  int reg = (d_which == 0 ? FR_RB_IO_RX_A_IO_TX_A : FR_RB_IO_RX_B_IO_TX_B);
  int t = d_usrp->_read_fpga_reg(reg);
  return (t & 0xffff);
}

bool
wbx_base::_compute_regs(double freq)
{
  /*
   * Determine values of registers, along with actual freq.
   * 
   * @param freq: target frequency in Hz
   * @type freq: double
   * @returns: (R, N, func, init, actual_freq)
   * @rtype: tuple(int, int, int, int, double)
   * 
   * Override this in derived classes.
   */
  throw std::runtime_error("_compute_regs called from base class\n");
}

double
wbx_base::_refclk_freq()
{
  return (double)(d_usrp->fpga_master_clock_freq())/_refclk_divisor();
}

int
wbx_base::_refclk_divisor()
{
  /*
   * Return value to stick in REFCLK_DIVISOR register
   */
  return 1;
}

struct freq_result_t
wbx_base::set_freq(double freq)
{
  /*
   * @returns (ok, actual_baseband_freq) where:
   * ok is True or False and indicates success or failure,
   * actual_baseband_freq is the RF frequency that corresponds to DC in the IF.
   */
  throw std::runtime_error("set_freq called from base class\n");
}

float
wbx_base::gain_min()
{
  throw std::runtime_error("gain_min called from base class\n");
}

float
wbx_base::gain_max()
{
  throw std::runtime_error("gain_max called from base class\n");
}

float
wbx_base::gain_db_per_step()
{
  throw std::runtime_error("gain_db_per_step called from base class\n");
}

bool
wbx_base::set_gain(float gain)
{
  /*
   * Set the gain.
   * 
   * @param gain:  gain in decibels
   * @returns True/False
   */
  throw std::runtime_error("set_gain called from base class\n");
}

bool
wbx_base::_set_pga(float pga_gain)
{
  bool ok;
  if(d_which == 0) {
    ok  = d_usrp->set_pga(0, pga_gain);
    ok |= d_usrp->set_pga(1, pga_gain);
  }
  else {
    ok  = d_usrp->set_pga(2, pga_gain);
    ok |= d_usrp->set_pga(3, pga_gain);
  }
  return ok;
}

bool
wbx_base::is_quadrature()
{
  /*
   * Return True if this board requires both I & Q analog channels.
   * 
   * This bit of info is useful when setting up the USRP Rx mux register.
   */
  return true;
}


/****************************************************************************/


wbx_base_tx::wbx_base_tx(usrp_basic_sptr usrp, int which)
  : wbx_base(usrp, which)
{        
  /*
   * @param usrp: instance of usrp.sink_c
   * @param which: 0 or 1 corresponding to side TX_A or TX_B respectively.
   */
  
  // power up the transmit side, NO -- but set antenna to receive
  d_usrp->write_io(d_which, (TX_POWER), (TX_POWER|RX_TXN));
  d_lo_offset = 0e6;
  
  // Gain is not set by the PGA, but the PGA must be set at max gain in the TX
  _set_pga(d_usrp->pga_max());
}

wbx_base_tx::~wbx_base_tx()
{
  // Power down and leave the T/R switch in the R position
  d_usrp->write_io(d_which, (RX_TXN), (TX_POWER|RX_TXN|TX_ENB_MIX|TX_ENB_VGA));
}

void
wbx_base_tx::set_auto_tr(bool on)
{
  if(on) {
    set_atr_mask (RX_TXN);
    set_atr_txval(0);
    set_atr_rxval(RX_TXN);
  }
  else {
    set_atr_mask (0);
    set_atr_txval(0);
    set_atr_rxval(0);
  }
}

void
wbx_base_tx::set_enable(bool on)
{
  /*
   * Enable transmitter if on is True
   */

  int mask = RX_TXN|TX_ENB_MIX|TX_ENB_VGA;
  //printf("HERE!!!!\n");
  if(on) {
    d_usrp->write_io(d_which, TX_ENB_MIX|TX_ENB_VGA, mask);
  }
  else {
    d_usrp->write_io(d_which, RX_TXN, mask);
  }
}

void
wbx_base_tx::set_lo_offset(double offset)
{
  /*
   * Set amount by which LO is offset from requested tuning frequency.
   * 
   * @param offset: offset in Hz
   */
  
  d_lo_offset = offset;
}

double
wbx_base_tx::lo_offset()
{
  /*
   * Get amount by which LO is offset from requested tuning frequency.
   * 
   * @returns Offset in Hz
   */

  return d_lo_offset;
}


/****************************************************************************/


wbx_base_rx::wbx_base_rx(usrp_basic_sptr usrp, int which)
  : wbx_base(usrp, which)
{
  /*
   * @param usrp: instance of usrp.source_c
   * @param which: 0 or 1 corresponding to side RX_A or RX_B respectively.
   */
  
  // set up for RX on TX/RX port
  select_rx_antenna("TX/RX");
  
  bypass_adc_buffers(true);

  d_lo_offset = 0.0;
}

wbx_base_rx::~wbx_base_rx()
{
  // Power down
  d_usrp->write_io(d_which, 0, (RXENABLE));
}
  
void
wbx_base_rx::set_auto_tr(bool on)
{
  if(on) {
    // FIXME: where does ENABLE come from?
    //set_atr_mask (ENABLE);
    set_atr_txval(     0);
    //set_atr_rxval(ENABLE);
  }
  else {
    set_atr_mask (0);
    set_atr_txval(0);
    set_atr_rxval(0);
  }
}

void
wbx_base_rx::select_rx_antenna(int which_antenna)
{
  /*
   * Specify which antenna port to use for reception.
   * @param which_antenna: either 'TX/RX' or 'RX2'
   */
  
  if(which_antenna == 0) {
    d_usrp->write_io(d_which, 0,        RX2_RX1N);
  }
  else if(which_antenna == 1) {
    d_usrp->write_io(d_which, RX2_RX1N, RX2_RX1N);
  }
  else {
    throw std::invalid_argument("which_antenna must be either 'TX/RX' or 'RX2'\n");
  }
}

void
wbx_base_rx::select_rx_antenna(const std::string &which_antenna)
{
  if(which_antenna == "TX/RX") {
    select_rx_antenna(0);
  }
  else if(which_antenna == "RX2") {
    select_rx_antenna(1);
  }
  else {
    throw std::invalid_argument("which_antenna must be either 'TX/RX' or 'RX2'\n");
  }
}

bool
wbx_base_rx::set_gain(float gain)
{
  /*
   * Set the gain.
   * 
   * @param gain:  gain in decibels
   * @returns True/False
   */
  
  float pga_gain, agc_gain;
  float maxgain = gain_max() - d_usrp->pga_max();
  float mingain = gain_min();
  if(gain > maxgain) {
    pga_gain = gain-maxgain;
    assert(pga_gain <= d_usrp->pga_max());
    agc_gain = maxgain;
  }
  else {
    pga_gain = 0;
    agc_gain = gain;
  }
   
  float V_maxgain = .2;
  float V_mingain = 1.2;
  float V_fullscale = 3.3;
  float dac_value = (agc_gain*(V_maxgain-V_mingain)/(maxgain-mingain) + V_mingain)*4096/V_fullscale;

  assert(dac_value>=0 && dac_value<4096);

  return d_usrp->write_aux_dac(d_which, 0, (int)(dac_value)) && _set_pga((int)(pga_gain));
}

void
wbx_base_rx::set_lo_offset(double offset)
{
  /*
   * Set amount by which LO is offset from requested tuning frequency.
   * 
   * @param offset: offset in Hz
   */
  d_lo_offset = offset;
}

double
wbx_base_rx::lo_offset()
{
  /*
   * Get amount by which LO is offset from requested tuning frequency.
   * 
   * @returns Offset in Hz
   */
  return d_lo_offset;
}

bool
wbx_base_rx::i_and_q_swapped()
{
  /*
   * Return True if this is a quadrature device and ADC 0 is Q.
   */
  return false;
}


/****************************************************************************/

_ADF410X_common::_ADF410X_common()
{
  // R-Register Common Values
  d_R_RSV = 0;    // bits 23,22,21
  d_LDP = 1;      // bit 20     Lock detect in 5 cycles
  d_TEST = 0;     // bit 19,18  Normal
  d_ABP = 0;      // bit 17,16  2.9ns
    
  // N-Register Common Values
  d_N_RSV = 0;    // 23,22
  d_CP_GAIN = 0;  // 21
    
  // Function Register Common Values
  d_P = 0;        // bits 23,22    0 = 8/9, 1 = 16/17, 2 = 32/33, 3 = 64/65
  d_PD2 = 0;      // bit  21       Normal operation
  d_CP2 = 4;      // bits 20,19,18 CP Gain = 5mA
  d_CP1 = 4;      // bits 17,16,15 CP Gain = 5mA
  d_TC = 0;       // bits 14-11    PFD Timeout
  d_FL = 0;       // bit 10,9      Fastlock Disabled
  d_CP3S = 0;     // bit 8         CP Enabled
  d_PDP = 0;      // bit 7         Phase detector polarity, Positive=1
  d_MUXOUT = 1;   // bits 6:4      Digital Lock Detect
  d_PD1 = 0;      // bit 3         Normal operation
  d_CR = 0;       // bit 2         Normal operation
}

_ADF410X_common::~_ADF410X_common()
{
}

bool 
_ADF410X_common::_compute_regs(double freq, int &retR, int &retcontrol, 
			       int &retN, double &retfreq)
{
  /*
   * Determine values of R, control, and N registers, along with actual freq.
   * 
   * @param freq: target frequency in Hz
   * @type freq: double
   * @returns: (R, N, control, actual_freq)
   * @rtype: tuple(int, int, int, double)
   */
  
  //  Band-specific N-Register Values
  double phdet_freq = _refclk_freq()/d_R_DIV;
  printf("phdet_freq = %f\n", phdet_freq);

  double desired_n = round(freq*d_freq_mult/phdet_freq);
  printf("desired_n %f\n", desired_n);

  double actual_freq = desired_n * phdet_freq;
  printf("actual freq %f\n", actual_freq);

  double B = floor(desired_n/_prescaler());
  double A = desired_n - _prescaler()*B;
  printf("A %f B %f\n", A, B);

  d_B_DIV = int(B);    // bits 20:8;
  d_A_DIV = int(A);    // bit 6:2;

  if(d_B_DIV < d_A_DIV) {
    retR = 0;
    retN = 0;
    retcontrol = 0;
    retfreq = 0;
    return false;
  }

  retR = (d_R_RSV<<21) | (d_LDP<<20) | (d_TEST<<18) |
    (d_ABP<<16) | (d_R_DIV<<2);
        
  retN = (d_N_RSV<<22) | (d_CP_GAIN<<21) | (d_B_DIV<<8) | (d_A_DIV<<2);

  retcontrol = (d_P<<22) | (d_PD2<<21) | (d_CP2<<18) | (d_CP1<<15) | 
    (d_TC<<11) | (d_FL<<9) | (d_CP3S<<8) | (d_PDP<<7) |
    (d_MUXOUT<<4) | (d_PD1<<3) | (d_CR<<2);
  
  retfreq = actual_freq/d_freq_mult;
  
  return true;
}

void 
_ADF410X_common::_write_all(int R, int N, int control)
{
  /*
   * Write all PLL registers:
   *   R counter latch,
   *   N counter latch,
   *   Function latch,
   *   Initialization latch
   * 
   * Adds 10ms delay between writing control and N if this is first call.
   * This is the required power-up sequence.
   * 
   * @param R: 24-bit R counter latch
   * @type R: int
   * @param N: 24-bit N counter latch
   * @type N: int
   * @param control: 24-bit control latch
   * @type control: int
   */
  static bool first = true;

  timespec t;
  t.tv_sec = 0;
  t.tv_nsec = 10000000;
  
  _write_R(R);
  _write_func(control);
  _write_init(control);
  if(first) {
    //time.sleep(0.010);
    nanosleep(&t, NULL);
    first = false;
  }
  _write_N(N);
}

void
_ADF410X_common::_write_R(int R)
{
  _write_it((R & ~0x3) | 0);
}

void
_ADF410X_common::_write_N(int N)
{
  _write_it((N & ~0x3) | 1);
}

void
_ADF410X_common::_write_func(int func)
{
  _write_it((func & ~0x3) | 2);
}

void
_ADF410X_common::_write_init(int init)
{
  _write_it((init & ~0x3) | 3);
}

void
_ADF410X_common::_write_it(int v)
{
  char c[3];
  c[0] = (char)((v >> 16) & 0xff);
  c[1] = (char)((v >>  8) & 0xff);
  c[2] = (char)((v & 0xff));
  std::string s(c, 3);
  //d_usrp->_write_spi(0, d_spi_enable, d_spi_format, s);
  usrp()->_write_spi(0, d_spi_enable, d_spi_format, s);
}

int
_ADF410X_common::_prescaler()
{
  if(d_P == 0) {
    return 8;
  }
  else if(d_P == 1) {
    return 16;
  }
  else if(d_P == 2) {
    return 32;
  }
  else if(d_P == 3) {
    return 64;
  }
  else {
    throw std::invalid_argument("Prescaler out of range\n");
  }
}

double
_ADF410X_common::_refclk_freq()
{
  throw std::runtime_error("_refclk_freq called from base class.");
}

bool
_ADF410X_common::_rx_write_io(int value, int mask)
{
  throw std::runtime_error("_rx_write_io called from base class.");
}

bool
_ADF410X_common::_lock_detect()
{
  throw std::runtime_error("_lock_detect called from base class.");
}

usrp_basic* 
_ADF410X_common::usrp()
{
  throw std::runtime_error("usrp() called from base class.");
}


/****************************************************************************/


_lo_common::_lo_common()
  : _ADF410X_common()
{
  // Band-specific R-Register Values
  d_R_DIV = 4;    // bits 15:2
  
  // Band-specific C-Register values
  d_P = 0;        // bits 23,22   0 = Div by 8/9
  d_CP2 = 4;      // bits 19:17
  d_CP1 = 4;      // bits 16:14
  
  // Band specifc N-Register Values
  d_DIVSEL = 0;   // bit 23
  d_DIV2 = 0;     // bit 22
  d_CPGAIN = 0;   // bit 21
  d_freq_mult = 1;
  
  d_div = 1;
  d_aux_div = 2;
  d_main_div = 0;
}

_lo_common::~_lo_common()
{
}

double
_lo_common::freq_min()
{
  return 50e6;
}

double
_lo_common::freq_max()
{
  return 1000e6;
}

void
_lo_common::set_divider(int main_or_aux, int divisor)
{
  if(main_or_aux == 0) {
    if((divisor != 1) || (divisor != 2) || (divisor != 4) || (divisor != 8)) {
      throw std::invalid_argument("Main Divider Must be 1, 2, 4, or 8\n");
    }
    d_main_div = (int)(log10(divisor)/log10(2.0));
  }
  else if(main_or_aux == 1) {
    if((divisor != 2) || (divisor != 4) || (divisor != 8) || (divisor != 16)) {
      throw std::invalid_argument("Aux Divider Must be 2, 4, 8 or 16\n");
    }
    d_main_div = (int)(log10(divisor/2.0)/log10(2.0));
  }   
  else {
    throw std::invalid_argument("main_or_aux must be 'main' or 'aux'\n");
  }
  
  int vala = d_main_div*SELA0;
  int valb = d_aux_div*SELB0;
  int mask = SELA0|SELA1|SELB0|SELB1;
  
  _rx_write_io((vala | valb), mask);
}

void
_lo_common::set_divider(const std::string &main_or_aux, int divisor)
{
  if(main_or_aux == "main") {
    set_divider(0, divisor);
  }
  else if(main_or_aux == "aux") {
    set_divider(1, divisor);
  }
  else {
    throw std::invalid_argument("main_or_aux must be 'main' or 'aux'\n");
  }
}

struct freq_result_t
_lo_common::set_freq(double freq)
{
  struct freq_result_t ret;
  
  if(freq < 20e6 or freq > 1200e6) {
    throw std::invalid_argument("Requested frequency out of range\n");
  }

  int div = 1;
  double lo_freq = freq * 2;
  while((lo_freq < 1e9) && (div < 8)) {
    div = div * 2;
    lo_freq = lo_freq * 2;
  }
  
  printf("For RF freq of %f, we set DIV=%d and LO Freq=%f\n", freq, div, lo_freq);

  set_divider("main", div);
  set_divider("aux", div*2);
  
  int R, N, control;
  double actual_freq;
  _compute_regs(lo_freq, R, N, control, actual_freq);
  
  printf("R %d N %d control %d actual freq %f\n", R, N, control, actual_freq);
  if(R==0) {
    ret.ok = false;
    ret.baseband_freq = 0.0;
    return ret;
  }
  _write_all(R, N, control);
  
  ret.ok = _lock_detect();
  ret.baseband_freq = actual_freq/div/2;
  return ret;
}
        

/****************************************************************************/


db_wbx_lo_tx::db_wbx_lo_tx(usrp_basic_sptr usrp, int which)
  : _lo_common(),
    wbx_base_tx(usrp, which)
{
}

db_wbx_lo_tx::~db_wbx_lo_tx()
{
}

float
db_wbx_lo_tx::gain_min()
{
  return -56.0;
}

float
db_wbx_lo_tx::gain_max()
{
  return 0.0;
}

float
db_wbx_lo_tx::gain_db_per_step()
{
  return 0.1;
}

bool
db_wbx_lo_tx::set_gain(float gain)
{
  /*
   * Set the gain.
   * 
   * @param gain:  gain in decibels
   * @returns True/False
   */

  float txvga_gain;
  float maxgain = gain_max();
  float mingain = gain_min();
  if(gain > maxgain) {
    txvga_gain = maxgain;
  }
  else if(gain < mingain) {
    txvga_gain = mingain;
  }
  else {
    txvga_gain = gain;
  }

  float V_maxgain = 1.4;
  float V_mingain = 0.1;
  float V_fullscale = 3.3;
  float dac_value = ((txvga_gain-mingain)*(V_maxgain-V_mingain)/
		     (maxgain-mingain) + V_mingain)*4096/V_fullscale;

  assert(dac_value>=0 && dac_value<4096);
  printf("DAC value %f\n", dac_value);

  return d_usrp->write_aux_dac(d_which, 1, (int)(dac_value));
}

double
db_wbx_lo_tx::_refclk_freq()
{
  return wbx_base::_refclk_freq();
}

bool
db_wbx_lo_tx::_rx_write_io(int value, int mask)
{
  return wbx_base::_rx_write_io(value, mask);
}

bool
db_wbx_lo_tx::_lock_detect()
{
  return wbx_base::_lock_detect();
}

usrp_basic* 
db_wbx_lo_tx::usrp()
{
  return d_usrp;
}


/****************************************************************************/


db_wbx_lo_rx::db_wbx_lo_rx(usrp_basic_sptr usrp, int which)
  : _lo_common(),
    wbx_base_rx(usrp, which)    
{
}

db_wbx_lo_rx::~db_wbx_lo_rx()
{
}

float
db_wbx_lo_rx::gain_min()
{
  return d_usrp->pga_min();
}

float
db_wbx_lo_rx::gain_max()
{
  return d_usrp->pga_max() + 45;
}

float
db_wbx_lo_rx::gain_db_per_step()
{
  return 0.05;
}

double
db_wbx_lo_rx::_refclk_freq()
{
  return wbx_base::_refclk_freq();
}

bool
db_wbx_lo_rx::_rx_write_io(int value, int mask)
{
  return wbx_base::_rx_write_io(value, mask);
}

bool
db_wbx_lo_rx::_lock_detect()
{
  return wbx_base::_lock_detect();
}

usrp_basic* 
db_wbx_lo_rx::usrp()
{
  return d_usrp;
}

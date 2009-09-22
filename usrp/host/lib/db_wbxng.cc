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

#include <usrp/db_wbxng.h>
#include <usrp/db_wbxng_adf4350.h>
#include <db_base_impl.h>
#include <stdio.h>

// d'board i/o pin defs
// Tx and Rx have shared defs, but different i/o regs
#define ENABLE_5        (1 << 7)         // enables 5.0V power supply
#define ENABLE_33       (1 << 6)         // enables 3.3V supply
#define RX_TXN          (1 << 5)         // Tx only: T/R antenna switch for TX/RX port
#define RX2_RX1N        (1 << 5)         // Rx only: antenna switch between RX2 and TX/RX port
#define RXBB_EN         (1 << 4)
#define TXMOD_EN        (1 << 4)
#define PLL_CE          (1 << 3)
#define PLL_PDBRF       (1 << 2)
#define PLL_MUXOUT      (1 << 1)
#define PLL_LOCK_DETECT (1 << 0)

wbxng_base::wbxng_base(usrp_basic_sptr _usrp, int which, int _power_on)
  : db_base(_usrp, which), d_power_on(_power_on)
{
  /*
    @param usrp: instance of usrp.source_c
    @param which: which side: 0 or 1 corresponding to side A or B respectively
    @type which: int
  */

  usrp()->_write_oe(d_which, 0, 0xffff);   // turn off all outputs

  d_first = true;
  d_spi_format = SPI_FMT_MSB | SPI_FMT_HDR_0;

  _enable_refclk(false);                // disable refclk

  set_auto_tr(false);
}

wbxng_base::~wbxng_base()
{
  delete d_common;
}

void
wbxng_base::_write_all(int R, int control, int N)
{
  /*
    Write R counter latch, control latch and N counter latch to VCO.
    
    Adds 10ms delay between writing control and N if this is first call.
    This is the required power-up sequence.
    
    @param R: 24-bit R counter latch
    @type R: int
    @param control: 24-bit control latch
    @type control: int
    @param N: 24-bit N counter latch
    @type N: int
  */
  timespec t;
  t.tv_sec = 0;
  t.tv_nsec = 10000000;

  /*
  _write_R(R);
  _write_control(control);
  if(d_first) {
    //time.sleep(0.010);
    nanosleep(&t, NULL);
    d_first = false;
  }
  _write_N(N);
  */
}

void
wbxng_base::_write_control(int control)
{
  //_write_it((control & ~0x3) | 0);
}

void
wbxng_base::_write_R(int R)
{
  //_write_it((R & ~0x3) | 1);
}

void
wbxng_base::_write_N(int N)
{
  //_write_it((N & ~0x3) | 2);
}

void
wbxng_base::_write_it(int v)
{
  char s[3];
  s[0] = (char)((v >> 16) & 0xff);
  s[1] = (char)((v >>  8) & 0xff);
  s[2] = (char)(v & 0xff);
  std::string str(s, 3);
  //usrp()->_write_spi(0, d_spi_enable, d_spi_format, str);
}
        
bool
wbxng_base::_lock_detect()
{
  /*
    @returns: the value of the VCO/PLL lock detect bit.
    @rtype: 0 or 1
  */
  
  if(d_common->_get_locked()){
    return true;
  }
  else {      // Give it a second chance
    return false;
    /*
    // FIXME: make portable sleep
    timespec t;
    t.tv_sec = 0;
    t.tv_nsec = 100000000;
    nanosleep(&t, NULL);
    
    if(usrp()->read_io(d_which) & PLL_LOCK_DETECT) {
      return true;
    }
    else {
      return false;
    }
    */
  }
 
  throw std::runtime_error("_lock_detect called from wbxng_base\n");
}

/*
bool
wbxng_base::_compute_regs(double freq, int &retR, int &retcontrol,
			   int &retN, double &retfreq)
{
  **COMMENT**
    Determine values of R, control, and N registers, along with actual freq.
    
    @param freq: target frequency in Hz
    @type freq: float
    @returns: (R, control, N, actual_freq)
    @rtype: tuple(int, int, int, float)
    
    Override this in derived classes.
  **COMMENT**
  
  //raise NotImplementedError;
  throw std::runtime_error("_compute_regs called from wbxng_base\n");
}
*/

int
wbxng_base::_compute_control_reg()
{
  throw std::runtime_error("_compute_control_reg called from wbxng_base\n");
  //return d_common->_compute_control_reg();
}

int
wbxng_base::_refclk_divisor()
{
  throw std::runtime_error("_refclk_divisor called from wbxng_base\n");
  //return d_common->_refclk_divisor();
}

double
wbxng_base::_refclk_freq()
{
  throw std::runtime_error("_refclk_divisor called from wbxng_base\n");
  // *** TODO *** Magic Number 64e6?
  //return 64e6/_refclk_divisor();
}

struct freq_result_t
wbxng_base::set_freq(double freq)
{
  /*
    @returns (ok, actual_baseband_freq) where:
    ok is True or False and indicates success or failure,
    actual_baseband_freq is the RF frequency that corresponds to DC in the IF.
  */

  freq_t int_freq = freq_t(freq);
  bool ok = d_common->_set_freq(int_freq*2);
  double freq_result = (double) d_common->_get_freq()/2.0;
  struct freq_result_t args = {ok, freq_result};

  /* Wait before reading Lock Detect*/
  timespec t;
  t.tv_sec = 0;
  t.tv_nsec = 10000000;
  nanosleep(&t, NULL);

  fprintf(stderr,"Setting WBXNG frequency, requested %d, obtained %f, lock_detect %d\n", 
          int_freq, freq_result, _lock_detect());

  // Offsetting the LO helps get the Tx carrier leakage out of the way.
  // This also ensures that on Rx, we're not getting hosed by the
  // FPGA's DC removal loop's time constant.  We were seeing a
  // problem when running with discontinuous transmission.
  // Offsetting the LO made the problem go away.
  //freq += d_lo_offset;
  
  //int R, control, N;
  //double actual_freq;
  //_compute_regs(freq, R, control, N, actual_freq);

  //if(R==0) {
  //  return args;
  //}
   
  //_write_all(R, control, N);
  //args.ok = _lock_detect();
  //args.baseband_freq = actual_freq;
  return args;
}

bool
wbxng_base::_set_pga(float pga_gain)
{
  /*
  if(d_which == 0) {
    usrp()->set_pga(0, pga_gain);
    usrp()->set_pga(1, pga_gain);
  }
  else {
    usrp()->set_pga(2, pga_gain);
    usrp()->set_pga(3, pga_gain);
  }
  */
  return true;
}

bool
wbxng_base::is_quadrature()
{
  /*
    Return True if this board requires both I & Q analog channels.
    
    This bit of info is useful when setting up the USRP Rx mux register.
  */
  return true;
}

double
wbxng_base::freq_min()
{
  return (double) d_common->_get_min_freq()/2.0;
}

double
wbxng_base::freq_max()
{
  return (double) d_common->_get_max_freq()/2.0;
}

// ----------------------------------------------------------------

wbxng_base_tx::wbxng_base_tx(usrp_basic_sptr _usrp, int which, int _power_on)
  : wbxng_base(_usrp, which, _power_on)
{
  /*
    @param usrp: instance of usrp.sink_c
    @param which: 0 or 1 corresponding to side TX_A or TX_B respectively.
  */
  
  if(which == 0) {
    d_spi_enable = SPI_ENABLE_TX_A;
  }
  else {
    d_spi_enable = SPI_ENABLE_TX_B;
  }

  d_common = new adf4350(_usrp, d_which, d_spi_enable);
  
  // power up the transmit side, but don't enable the mixer
  usrp()->_write_oe(d_which,(RX_TXN|TXMOD_EN|ENABLE_33|ENABLE_5), (RX_TXN|TXMOD_EN|ENABLE_33|ENABLE_5));
  usrp()->write_io(d_which, (power_on()|RX_TXN|TXMOD_EN|ENABLE_33|ENABLE_5), (RX_TXN|TXMOD_EN|ENABLE_33|ENABLE_5));
  fprintf(stderr,"Setting WBXNG TXMOD on"); 
  //set_lo_offset(4e6);

  //set_gain((gain_min() + gain_max()) / 2.0);  // initialize gain
}

wbxng_base_tx::~wbxng_base_tx()
{
  shutdown();
}


void
wbxng_base_tx::shutdown()
{
  // fprintf(stderr, "wbxng_base_tx::shutdown  d_is_shutdown = %d\n", d_is_shutdown);

  if (!d_is_shutdown){
    d_is_shutdown = true;
    // do whatever there is to do to shutdown

    // Power down and leave the T/R switch in the R position
    usrp()->write_io(d_which, (power_off()|RX_TXN), (RX_TXN|ENABLE_33|ENABLE_5));

    // Power down VCO/PLL
    d_common->_enable(false);
  
    /*
    _write_control(_compute_control_reg());
    */
    _enable_refclk(false);                       // turn off refclk
    set_auto_tr(false);
  }
}

bool
wbxng_base_tx::set_auto_tr(bool on)
{
  bool ok = true;
  if(on) {
    ok &= set_atr_mask (RX_TXN | ENABLE_33 | ENABLE_5);
    ok &= set_atr_txval(0      | ENABLE_33 | ENABLE_5);
    ok &= set_atr_rxval(RX_TXN | 0);
  }
  else {
    ok &= set_atr_mask (0);
    ok &= set_atr_txval(0);
    ok &= set_atr_rxval(0);
  }
  return ok;
}

bool
wbxng_base_tx::set_enable(bool on)
{
  /*
    Enable transmitter if on is true
  */

  int v;
  int mask = RX_TXN | ENABLE_5 | ENABLE_33;
  if(on) {
    v = ENABLE_5 | ENABLE_33;
  }
  else {
    v = RX_TXN;
  }
  return usrp()->write_io(d_which, v, mask);
}

float
wbxng_base_tx::gain_min()
{
  return usrp()->pga_max();
}

float
wbxng_base_tx::gain_max()
{
  return usrp()->pga_max();
}

float
wbxng_base_tx::gain_db_per_step()
{
  return 1;
}

bool
wbxng_base_tx::set_gain(float gain)
{
  /*
    Set the gain.
    
    @param gain:  gain in decibels
    @returns True/False
  */
  return _set_pga(usrp()->pga_max());
}


/**************************************************************************/


wbxng_base_rx::wbxng_base_rx(usrp_basic_sptr _usrp, int which, int _power_on)
  : wbxng_base(_usrp, which, _power_on)
{
  /*
    @param usrp: instance of usrp.source_c
    @param which: 0 or 1 corresponding to side RX_A or RX_B respectively.
  */

  if(which == 0) {
    d_spi_enable = SPI_ENABLE_RX_A;
  }
  else {
    d_spi_enable = SPI_ENABLE_RX_B;
  }

  d_common = new adf4350(_usrp, d_which, d_spi_enable);

  usrp()->_write_oe(d_which, (RX2_RX1N|RXBB_EN|ENABLE_33|ENABLE_5), (RX2_RX1N|RXBB_EN|ENABLE_33|ENABLE_5));
  usrp()->write_io(d_which,  (power_on()|RX2_RX1N|RXBB_EN|ENABLE_33|ENABLE_5), (RX2_RX1N|RXBB_EN|ENABLE_33|ENABLE_5));
  fprintf(stderr,"Setting WBXNG RXBB on"); 
  
  // set up for RX on TX/RX port
  select_rx_antenna("TX/RX");
  
  bypass_adc_buffers(true);

  /*  
  set_lo_offset(-4e6);
  */
}

wbxng_base_rx::~wbxng_base_rx()
{
  shutdown();
}

void
wbxng_base_rx::shutdown()
{
  // fprintf(stderr, "wbxng_base_rx::shutdown  d_is_shutdown = %d\n", d_is_shutdown);

  if (!d_is_shutdown){
    d_is_shutdown = true;
    // do whatever there is to do to shutdown

    // Power down
    usrp()->common_write_io(C_RX, d_which, power_off(), (ENABLE_33|ENABLE_5));

    // Power down VCO/PLL
    d_common->_enable(false);

    // fprintf(stderr, "wbxng_base_rx::shutdown  before _write_control\n");
    //_write_control(_compute_control_reg());

    // fprintf(stderr, "wbxng_base_rx::shutdown  before _enable_refclk\n");
    _enable_refclk(false);                       // turn off refclk

    // fprintf(stderr, "wbxng_base_rx::shutdown  before set_auto_tr\n");
    set_auto_tr(false);

    // fprintf(stderr, "wbxng_base_rx::shutdown  after set_auto_tr\n");
  }
}

bool
wbxng_base_rx::set_auto_tr(bool on)
{
  bool ok = true;
  if(on) {
    ok &= set_atr_mask (ENABLE_33|ENABLE_5);
    ok &= set_atr_txval(     0);
    ok &= set_atr_rxval(ENABLE_33|ENABLE_5);
  }
  else {
    ok &= set_atr_mask (0);
    ok &= set_atr_txval(0);
    ok &= set_atr_rxval(0);
  }
  return true;
}

bool
wbxng_base_rx::select_rx_antenna(int which_antenna)
{
  /*
    Specify which antenna port to use for reception.
    @param which_antenna: either 'TX/RX' or 'RX2'
  */

  if(which_antenna == 0) {
    usrp()->write_io(d_which, 0,RX2_RX1N);
  }
  else if(which_antenna == 1) {
    usrp()->write_io(d_which, RX2_RX1N, RX2_RX1N);
  }
  else {
    return false;
    // throw std::invalid_argument("which_antenna must be either 'TX/RX' or 'RX2'\n");
  }
  return true;
}

bool
wbxng_base_rx::select_rx_antenna(const std::string &which_antenna)
{
  /*
    Specify which antenna port to use for reception.
    @param which_antenna: either 'TX/RX' or 'RX2'
  */

  
  if(which_antenna == "TX/RX") {
    usrp()->write_io(d_which, 0, RX2_RX1N);
  }
  else if(which_antenna == "RX2") {
    usrp()->write_io(d_which, RX2_RX1N, RX2_RX1N);
  }
  else {
    // throw std::invalid_argument("which_antenna must be either 'TX/RX' or 'RX2'\n");
    return false;
  }
  
  return true;
}

bool
wbxng_base_rx::set_gain(float gain)
{
  /*
    Set the gain.
    
    @param gain:  gain in decibels
    @returns True/False
  */
  
  /*
  // clamp gain
  gain = std::max(gain_min(), std::min(gain, gain_max()));

  float pga_gain, agc_gain;
  float V_maxgain, V_mingain, V_fullscale, dac_value;

  float maxgain = gain_max() - usrp()->pga_max();
  float mingain = gain_min();
  if(gain > maxgain) {
    pga_gain = gain-maxgain;
    assert(pga_gain <= usrp()->pga_max());
    agc_gain = maxgain;
  }
  else {
    pga_gain = 0;
    agc_gain = gain;
  }
  
  V_maxgain = .2;
  V_mingain = 1.2;
  V_fullscale = 3.3;
  dac_value = (agc_gain*(V_maxgain-V_mingain)/(maxgain-mingain) + V_mingain)*4096/V_fullscale;

  assert(dac_value>=0 && dac_value<4096);

  return (usrp()->write_aux_dac(d_which, 0, int(dac_value))
	  && _set_pga(int(pga_gain)));
  */
  return false;
}

// ----------------------------------------------------------------

db_wbxng_tx::db_wbxng_tx(usrp_basic_sptr usrp, int which)
  : wbxng_base_tx(usrp, which)
{
}

db_wbxng_tx::~db_wbxng_tx()
{
}

/*
bool
db_wbxng_tx::_compute_regs(double freq, int &retR, int &retcontrol,
				 int &retN, double &retfreq)
{
  return d_common->_compute_regs(_refclk_freq(), freq, retR,
				 retcontrol, retN, retfreq);
}
*/


db_wbxng_rx::db_wbxng_rx(usrp_basic_sptr usrp, int which)
  : wbxng_base_rx(usrp, which)
{
  set_gain((gain_min() + gain_max()) / 2.0);  // initialize gain
}

db_wbxng_rx::~db_wbxng_rx()
{
}

float
db_wbxng_rx::gain_min()
{
  return usrp()->pga_min();
}

float
db_wbxng_rx::gain_max()
{
  return usrp()->pga_max()+70;
}

float
db_wbxng_rx::gain_db_per_step()
{
  return 0.05;
}


bool
db_wbxng_rx::i_and_q_swapped()
{
  return true;
}

/*
bool
db_wbxng_rx::_compute_regs(double freq, int &retR, int &retcontrol,
				 int &retN, double &retfreq)
{
  return d_common->_compute_regs(_refclk_freq(), freq, retR,
				 retcontrol, retN, retfreq);
}
*/


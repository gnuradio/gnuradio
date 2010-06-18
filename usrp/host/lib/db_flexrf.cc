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

#include <usrp/db_flexrf.h>
#include <db_base_impl.h>

// d'board i/o pin defs
// Tx and Rx have shared defs, but different i/o regs
#define AUX_RXAGC (1 << 8)
#define POWER_UP  (1 << 7)         // enables power supply
#define RX_TXN    (1 << 6)         // Tx only: T/R antenna switch for TX/RX port
#define RX2_RX1N  (1 << 6)         // Rx only: antenna switch between RX2 and TX/RX port
#define ENABLE    (1 << 5)         // enables mixer
#define AUX_SEN   (1 << 4)
#define AUX_SCLK  (1 << 3)
#define PLL_LOCK_DETECT (1 << 2)
#define AUX_SDO   (1 << 1)
#define CLOCK_OUT (1 << 0)

flexrf_base::flexrf_base(usrp_basic_sptr _usrp, int which, int _power_on)
  : db_base(_usrp, which), d_power_on(_power_on)
{
  /*
    @param usrp: instance of usrp.source_c
    @param which: which side: 0 or 1 corresponding to side A or B respectively
    @type which: int
  */

  d_first = true;
  d_spi_format = SPI_FMT_MSB | SPI_FMT_HDR_0;

  usrp()->_write_oe(d_which, 0, 0xffff);   // turn off all outputs
  _enable_refclk(false);                // disable refclk

  set_auto_tr(false);
}

flexrf_base::~flexrf_base()
{
  delete d_common;
}

void
flexrf_base::_write_all(int R, int control, int N)
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

  _write_R(R);
  _write_control(control);
  if(d_first) {
    //time.sleep(0.010);
    nanosleep(&t, NULL);
    d_first = false;
  }
  _write_N(N);
}

void
flexrf_base::_write_control(int control)
{
  _write_it((control & ~0x3) | 0);
}

void
flexrf_base::_write_R(int R)
{
  _write_it((R & ~0x3) | 1);
}

void
flexrf_base::_write_N(int N)
{
  _write_it((N & ~0x3) | 2);
}

void
flexrf_base::_write_it(int v)
{
  char s[3];
  s[0] = (char)((v >> 16) & 0xff);
  s[1] = (char)((v >>  8) & 0xff);
  s[2] = (char)(v & 0xff);
  std::string str(s, 3);
  usrp()->_write_spi(0, d_spi_enable, d_spi_format, str);
}
        
bool
flexrf_base::_lock_detect()
{
  /*
    @returns: the value of the VCO/PLL lock detect bit.
    @rtype: 0 or 1
  */
  if(usrp()->read_io(d_which) & PLL_LOCK_DETECT) {
    return true;
  }
  else {      // Give it a second chance
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
  }
}

bool
flexrf_base::_compute_regs(double freq, int &retR, int &retcontrol,
			   int &retN, double &retfreq)
{
  /*
    Determine values of R, control, and N registers, along with actual freq.
    
    @param freq: target frequency in Hz
    @type freq: float
    @returns: (R, control, N, actual_freq)
    @rtype: tuple(int, int, int, float)
    
    Override this in derived classes.
  */
  
  //raise NotImplementedError;
  throw std::runtime_error("_compute_regs called from flexrf_base\n");
}

int
flexrf_base::_compute_control_reg()
{
  return d_common->_compute_control_reg();
}

int
flexrf_base::_refclk_divisor()
{
  return d_common->_refclk_divisor();
}

struct freq_result_t
flexrf_base::set_freq(double freq)
{
  /*
    @returns (ok, actual_baseband_freq) where:
    ok is True or False and indicates success or failure,
    actual_baseband_freq is the RF frequency that corresponds to DC in the IF.
  */

  struct freq_result_t args = {false, 0};

  // Offsetting the LO helps get the Tx carrier leakage out of the way.
  // This also ensures that on Rx, we're not getting hosed by the
  // FPGA's DC removal loop's time constant.  We were seeing a
  // problem when running with discontinuous transmission.
  // Offsetting the LO made the problem go away.
  freq += d_lo_offset;
  
  int R, control, N;
  double actual_freq;
  _compute_regs(freq, R, control, N, actual_freq);

  if(R==0) {
    return args;
  }
   
  _write_all(R, control, N);
  args.ok = _lock_detect();
  args.baseband_freq = actual_freq;
  return args;
}

bool
flexrf_base::_set_pga(float pga_gain)
{
  if(d_which == 0) {
    usrp()->set_pga(0, pga_gain);
    usrp()->set_pga(1, pga_gain);
  }
  else {
    usrp()->set_pga(2, pga_gain);
    usrp()->set_pga(3, pga_gain);
  }
  return true;
}

bool
flexrf_base::is_quadrature()
{
  /*
    Return True if this board requires both I & Q analog channels.
    
    This bit of info is useful when setting up the USRP Rx mux register.
  */
  return true;
}

double
flexrf_base::freq_min()
{
  return d_common->freq_min();
}

double
flexrf_base::freq_max()
{
  return d_common->freq_max();
}

// ----------------------------------------------------------------

flexrf_base_tx::flexrf_base_tx(usrp_basic_sptr _usrp, int which, int _power_on)
  : flexrf_base(_usrp, which, _power_on)
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
  
  // power up the transmit side, but don't enable the mixer
  usrp()->_write_oe(d_which,(POWER_UP|RX_TXN|ENABLE), 0xffff);
  usrp()->write_io(d_which, (power_on()|RX_TXN), (POWER_UP|RX_TXN|ENABLE));
  set_lo_offset(4e6);

  set_gain((gain_min() + gain_max()) / 2.0);  // initialize gain
}

flexrf_base_tx::~flexrf_base_tx()
{
  shutdown();
}


void
flexrf_base_tx::shutdown()
{
  // fprintf(stderr, "flexrf_base_tx::shutdown  d_is_shutdown = %d\n", d_is_shutdown);

  if (!d_is_shutdown){
    d_is_shutdown = true;
    // do whatever there is to do to shutdown

    // Power down and leave the T/R switch in the R position
    usrp()->write_io(d_which, (power_off()|RX_TXN), (POWER_UP|RX_TXN|ENABLE));

    // Power down VCO/PLL
    d_PD = 3;
  
    _write_control(_compute_control_reg());
    _enable_refclk(false);                       // turn off refclk
    set_auto_tr(false);
  }
}

bool
flexrf_base_tx::set_auto_tr(bool on)
{
  bool ok = true;
  if(on) {
    ok &= set_atr_mask (RX_TXN | ENABLE);
    ok &= set_atr_txval(0      | ENABLE);
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
flexrf_base_tx::set_enable(bool on)
{
  /*
    Enable transmitter if on is true
  */

  int v;
  int mask = RX_TXN | ENABLE;
  if(on) {
    v = ENABLE;
  }
  else {
    v = RX_TXN;
  }
  return usrp()->write_io(d_which, v, mask);
}

float
flexrf_base_tx::gain_min()
{
  return usrp()->pga_max();
}

float
flexrf_base_tx::gain_max()
{
  return usrp()->pga_max();
}

float
flexrf_base_tx::gain_db_per_step()
{
  return 1;
}

bool
flexrf_base_tx::set_gain(float gain)
{
  /*
    Set the gain.
    
    @param gain:  gain in decibels
    @returns True/False
  */
  return _set_pga(usrp()->pga_max());
}


/**************************************************************************/


flexrf_base_rx::flexrf_base_rx(usrp_basic_sptr _usrp, int which, int _power_on)
  : flexrf_base(_usrp, which, _power_on)
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

  usrp()->_write_oe(d_which, (POWER_UP|RX2_RX1N|ENABLE), 0xffff);
  usrp()->write_io(d_which,  (power_on()|RX2_RX1N|ENABLE), 
		   (POWER_UP|RX2_RX1N|ENABLE));
  
  // set up for RX on TX/RX port
  select_rx_antenna("TX/RX");
  
  bypass_adc_buffers(true);

  set_lo_offset(-4e6);
}

flexrf_base_rx::~flexrf_base_rx()
{
  shutdown();
}

void
flexrf_base_rx::shutdown()
{
  // fprintf(stderr, "flexrf_base_rx::shutdown  d_is_shutdown = %d\n", d_is_shutdown);

  if (!d_is_shutdown){
    d_is_shutdown = true;
    // do whatever there is to do to shutdown

    // Power down
    usrp()->common_write_io(C_RX, d_which, power_off(), (POWER_UP|ENABLE));

    // Power down VCO/PLL
    d_PD = 3;
  

    // fprintf(stderr, "flexrf_base_rx::shutdown  before _write_control\n");
    _write_control(_compute_control_reg());

    // fprintf(stderr, "flexrf_base_rx::shutdown  before _enable_refclk\n");
    _enable_refclk(false);                       // turn off refclk

    // fprintf(stderr, "flexrf_base_rx::shutdown  before set_auto_tr\n");
    set_auto_tr(false);

    // fprintf(stderr, "flexrf_base_rx::shutdown  after set_auto_tr\n");
  }
}

bool
flexrf_base_rx::set_auto_tr(bool on)
{
  bool ok = true;
  if(on) {
    ok &= set_atr_mask (ENABLE);
    ok &= set_atr_txval(     0);
    ok &= set_atr_rxval(ENABLE);
  }
  else {
    ok &= set_atr_mask (0);
    ok &= set_atr_txval(0);
    ok &= set_atr_rxval(0);
  }
  return true;
}

bool
flexrf_base_rx::select_rx_antenna(int which_antenna)
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
flexrf_base_rx::select_rx_antenna(const std::string &which_antenna)
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
flexrf_base_rx::set_gain(float gain)
{
  /*
    Set the gain.
    
    @param gain:  gain in decibels
    @returns True/False
  */
  
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
}

// ----------------------------------------------------------------


_AD4360_common::_AD4360_common()
{
  // R-Register Common Values
  d_R_RSV = 0;  // bits 23,22
  d_BSC   = 3;  // bits 21,20 Div by 8 to be safe
  d_TEST  = 0;  // bit 19
  d_LDP   = 1;  // bit 18
  d_ABP   = 0;  // bit 17,16   3ns

  // N-Register Common Values
  d_N_RSV = 0;  // bit 7
        
  // Control Register Common Values
  d_PD    = 0;  // bits 21,20   Normal operation
  d_PL    = 0;  // bits 13,12   11mA
  d_MTLD  = 1;  // bit 11       enabled
  d_CPG   = 0;  // bit 10       CP setting 1
  d_CP3S  = 0;  // bit 9        Normal
  d_PDP   = 1;  // bit 8        Positive
  d_MUXOUT = 1; // bits 7:5     Digital Lock Detect
  d_CR    = 0;  // bit 4        Normal
  d_PC    = 1;  // bits 3,2     Core power 10mA
}

_AD4360_common::~_AD4360_common()
{
}

bool
_AD4360_common::_compute_regs(double refclk_freq, double freq, int &retR, 
			      int &retcontrol, int &retN, double &retfreq)
{
  /*
    Determine values of R, control, and N registers, along with actual freq.
    
    @param freq: target frequency in Hz
    @type freq: float
    @returns: (R, control, N, actual_freq)
    @rtype: tuple(int, int, int, float)
  */
  
  //  Band-specific N-Register Values
  //float phdet_freq = _refclk_freq()/d_R_DIV;
  double phdet_freq = refclk_freq/d_R_DIV;
  double desired_n = round(freq*d_freq_mult/phdet_freq);
  double actual_freq = desired_n * phdet_freq;
  int B = floor(desired_n/_prescaler());
  int A = desired_n - _prescaler()*B;
  d_B_DIV = int(B);    // bits 20:8
  d_A_DIV = int(A);    // bit 6:2

  //assert db_B_DIV >= db_A_DIV
  if(d_B_DIV < d_A_DIV) {
    retR = 0;
    retcontrol = 0;
    retN = 0;
    retfreq = 0;
    return false;
  }

  int R = (d_R_RSV<<22) | (d_BSC<<20) | (d_TEST<<19) | 
    (d_LDP<<18) | (d_ABP<<16) | (d_R_DIV<<2);
  
  int control = _compute_control_reg();

  int N = (d_DIVSEL<<23) | (d_DIV2<<22) | (d_CPGAIN<<21) | 
    (d_B_DIV<<8) | (d_N_RSV<<7) | (d_A_DIV<<2);

  retR = R;
  retcontrol = control;
  retN = N;
  retfreq = actual_freq/d_freq_mult;
  return true;
}

int
_AD4360_common::_compute_control_reg()
{
  int control = (d_P<<22) | (d_PD<<20) | (d_CP2<<17) | (d_CP1<<14)
    | (d_PL<<12) | (d_MTLD<<11) | (d_CPG<<10) | (d_CP3S<<9) | (d_PDP<<8)
    | (d_MUXOUT<<5) | (d_CR<<4) | (d_PC<<2);
  
  return control;
}

int
_AD4360_common::_refclk_divisor()
{
  /*
    Return value to stick in REFCLK_DIVISOR register
  */
  return 1;
}
    
int
_AD4360_common::_prescaler()
{
  if(d_P == 0) {
    return 8;
  }
  else if(d_P == 1) {
    return 16;
  }
  else {
    return 32;
  }
}

//----------------------------------------------------------------------

_2200_common::_2200_common()
  : _AD4360_common()
{
  // Band-specific R-Register Values
  d_R_DIV = 16;  // bits 15:2
   
  // Band-specific C-Register values
  d_P = 1;        // bits 23,22   Div by 16/17
  d_CP2 = 7;      // bits 19:17
  d_CP1 = 7;      // bits 16:14

  // Band specifc N-Register Values
  d_DIVSEL = 0;   // bit 23
  d_DIV2 = 0;     // bit 22
  d_CPGAIN = 0;   // bit 21
  d_freq_mult = 1;
}

double
_2200_common::freq_min()
{
  return 2000e6;
}

double
_2200_common::freq_max()
{
  return 2400e6;
}

//----------------------------------------------------------------------

_2400_common::_2400_common()
  : _AD4360_common()
{
  // Band-specific R-Register Values
  d_R_DIV = 16;  // bits 15:2
   
  // Band-specific C-Register values
  d_P = 1;        // bits 23,22   Div by 16/17
  d_CP2 = 7;      // bits 19:17
  d_CP1 = 7;      // bits 16:14

  // Band specifc N-Register Values
  d_DIVSEL = 0;   // bit 23
  d_DIV2 = 0;     // bit 22
  d_CPGAIN = 0;   // bit 21
  d_freq_mult = 1;
}

double
_2400_common::freq_min()
{
  return 2300e6;
}

double
_2400_common::freq_max()
{
  return 2900e6;
}

//----------------------------------------------------------------------

_1200_common::_1200_common()
  : _AD4360_common()
{
  // Band-specific R-Register Values
  d_R_DIV = 16;  // bits 15:2  DIV by 16 for a 1 MHz phase detector freq
   
  // Band-specific C-Register values
  d_P = 1;        // bits 23,22   Div by 16/17
  d_CP2 = 7;      // bits 19:17   1.25 mA
  d_CP1 = 7;      // bits 16:14   1.25 mA
  
  // Band specifc N-Register Values
  d_DIVSEL = 0;   // bit 23
  d_DIV2 = 1;     // bit 22
  d_CPGAIN = 0;   // bit 21
  d_freq_mult = 2;
}

double 
_1200_common::freq_min()
{
  return 1150e6;
}

double 
_1200_common::freq_max()
{
  return 1450e6;
}

//-------------------------------------------------------------------------

_1800_common::_1800_common()
  : _AD4360_common()
{
  // Band-specific R-Register Values
  d_R_DIV = 16;  // bits 15:2  DIV by 16 for a 1 MHz phase detector freq
    
  // Band-specific C-Register values
  d_P = 1;        // bits 23,22   Div by 16/17
  d_CP2 = 7;      // bits 19:17   1.25 mA
  d_CP1 = 7;      // bits 16:14   1.25 mA
  
  // Band specifc N-Register Values
  d_DIVSEL = 0;   // bit 23
  d_DIV2 = 0;     // bit 22
  d_freq_mult = 1;
  d_CPGAIN = 0;   // bit 21
}

double 
_1800_common::freq_min()
{
  return 1500e6;
}

double 
_1800_common::freq_max()
{
  return 2100e6;
}

//-------------------------------------------------------------------------

_900_common::_900_common()
  : _AD4360_common()
{
  // Band-specific R-Register Values
  d_R_DIV = 16;  // bits 15:2  DIV by 16 for a 1 MHz phase detector freq
   
  // Band-specific C-Register values
  d_P = 1;        // bits 23,22   Div by 16/17
  d_CP2 = 7;      // bits 19:17   1.25 mA
  d_CP1 = 7;      // bits 16:14   1.25 mA
  
  // Band specifc N-Register Values
  d_DIVSEL = 0;   // bit 23
  d_DIV2 = 1;     // bit 22
  d_freq_mult = 2;
  d_CPGAIN = 0;   // bit 21
}

double
_900_common::freq_min()
{
  return 750e6;
}

double
_900_common::freq_max()
{
  return 1050e6;
}

//-------------------------------------------------------------------------

_400_common::_400_common()
  : _AD4360_common()
{
  // Band-specific R-Register Values
  d_R_DIV = 16;   // bits 15:2 
   
  // Band-specific C-Register values
  d_P = 0;        // bits 23,22   Div by 8/9
  d_CP2 = 7;      // bits 19:17   1.25 mA
  d_CP1 = 7;      // bits 16:14   1.25 mA
  
  // Band specifc N-Register Values  These are different for TX/RX
  d_DIVSEL = 0;   // bit 23
  d_freq_mult = 2;
  
  d_CPGAIN = 0;   // bit 21
}

double 
_400_common::freq_min()
{
  return 400e6;
}  

double 
_400_common::freq_max()
{
  return 500e6;
}  

_400_tx::_400_tx()
  : _400_common()
{
  d_DIV2 = 1;     // bit 22
}

_400_rx::_400_rx()
  : _400_common()
{
  d_DIV2 = 0;    // bit 22   // RX side has built-in DIV2 in AD8348
}

//------------------------------------------------------------    

db_flexrf_2200_tx::db_flexrf_2200_tx(usrp_basic_sptr usrp, int which)
  : flexrf_base_tx(usrp, which)
{
  d_common = new _2200_common();
}

db_flexrf_2200_tx::~db_flexrf_2200_tx()
{
}

bool
db_flexrf_2200_tx::_compute_regs(double freq, int &retR, int &retcontrol,
				 int &retN, double &retfreq)
{
  return d_common->_compute_regs(_refclk_freq(), freq, retR,
				 retcontrol, retN, retfreq);
}



db_flexrf_2200_rx::db_flexrf_2200_rx(usrp_basic_sptr usrp, int which)
  : flexrf_base_rx(usrp, which)
{
  d_common = new _2200_common();
  set_gain((gain_min() + gain_max()) / 2.0);  // initialize gain
}

db_flexrf_2200_rx::~db_flexrf_2200_rx()
{
}

float
db_flexrf_2200_rx::gain_min()
{
  return usrp()->pga_min();
}

float
db_flexrf_2200_rx::gain_max()
{
  return usrp()->pga_max()+70;
}

float
db_flexrf_2200_rx::gain_db_per_step()
{
  return 0.05;
}


bool
db_flexrf_2200_rx::i_and_q_swapped()
{
  return true;
}

bool
db_flexrf_2200_rx::_compute_regs(double freq, int &retR, int &retcontrol,
				 int &retN, double &retfreq)
{
  return d_common->_compute_regs(_refclk_freq(), freq, retR,
				 retcontrol, retN, retfreq);
}

//------------------------------------------------------------    

db_flexrf_2400_tx::db_flexrf_2400_tx(usrp_basic_sptr usrp, int which)
  : flexrf_base_tx(usrp, which)
{
  d_common = new _2400_common();
}

db_flexrf_2400_tx::~db_flexrf_2400_tx()
{
}

bool
db_flexrf_2400_tx::_compute_regs(double freq, int &retR, int &retcontrol,
				 int &retN, double &retfreq)
{
  return d_common->_compute_regs(_refclk_freq(), freq, retR,
				 retcontrol, retN, retfreq);
}



db_flexrf_2400_rx::db_flexrf_2400_rx(usrp_basic_sptr usrp, int which)
  : flexrf_base_rx(usrp, which)
{
  d_common = new _2400_common();
  set_gain((gain_min() + gain_max()) / 2.0);  // initialize gain
}

db_flexrf_2400_rx::~db_flexrf_2400_rx()
{
}

float
db_flexrf_2400_rx::gain_min()
{
  return usrp()->pga_min();
}

float
db_flexrf_2400_rx::gain_max()
{
  return usrp()->pga_max()+70;
}

float
db_flexrf_2400_rx::gain_db_per_step()
{
  return 0.05;
}


bool
db_flexrf_2400_rx::i_and_q_swapped()
{
  return true;
}

bool
db_flexrf_2400_rx::_compute_regs(double freq, int &retR, int &retcontrol,
				 int &retN, double &retfreq)
{
  return d_common->_compute_regs(_refclk_freq(), freq, retR,
				 retcontrol, retN, retfreq);
}

//------------------------------------------------------------    


db_flexrf_1200_tx::db_flexrf_1200_tx(usrp_basic_sptr usrp, int which)
  : flexrf_base_tx(usrp, which)
{
  d_common = new _1200_common();
}

db_flexrf_1200_tx::~db_flexrf_1200_tx()
{
}

bool
db_flexrf_1200_tx::_compute_regs(double freq, int &retR, int &retcontrol,
				 int &retN, double &retfreq)
{
  return d_common->_compute_regs(_refclk_freq(), freq, retR,
				 retcontrol, retN, retfreq);
}




db_flexrf_1200_rx::db_flexrf_1200_rx(usrp_basic_sptr usrp, int which)
  : flexrf_base_rx(usrp, which)
{
  d_common = new _1200_common();
  set_gain((gain_min() + gain_max()) / 2.0);  // initialize gain
}

db_flexrf_1200_rx::~db_flexrf_1200_rx()
{
}

float
db_flexrf_1200_rx::gain_min()
{
  return usrp()->pga_min();
}

float
db_flexrf_1200_rx::gain_max()
{
  return usrp()->pga_max()+70;
}

float
db_flexrf_1200_rx::gain_db_per_step()
{
  return 0.05;
}

bool
db_flexrf_1200_rx::i_and_q_swapped()
{
  return true;
}

bool
db_flexrf_1200_rx::_compute_regs(double freq, int &retR, int &retcontrol,
				 int &retN, double &retfreq)
{
  return d_common->_compute_regs(_refclk_freq(), freq, retR,
				 retcontrol, retN, retfreq);
}


//------------------------------------------------------------    


db_flexrf_1800_tx::db_flexrf_1800_tx(usrp_basic_sptr usrp, int which)
  : flexrf_base_tx(usrp, which)
{
  d_common = new _1800_common();
}

db_flexrf_1800_tx::~db_flexrf_1800_tx()
{
}

bool
db_flexrf_1800_tx::_compute_regs(double freq, int &retR, int &retcontrol,
				 int &retN, double &retfreq)
{
  return d_common->_compute_regs(_refclk_freq(), freq, retR,
				 retcontrol, retN, retfreq);
}



db_flexrf_1800_rx::db_flexrf_1800_rx(usrp_basic_sptr usrp, int which)
  : flexrf_base_rx(usrp, which)
{
  d_common = new _1800_common();
  set_gain((gain_min() + gain_max()) / 2.0);  // initialize gain
}

db_flexrf_1800_rx::~db_flexrf_1800_rx()
{
}


float
db_flexrf_1800_rx::gain_min()
{
  return usrp()->pga_min();
}

float
db_flexrf_1800_rx::gain_max()
{
  return usrp()->pga_max()+70;
}

float
db_flexrf_1800_rx::gain_db_per_step()
{
  return 0.05;
}

bool
db_flexrf_1800_rx::i_and_q_swapped()
{
  return true;
}

bool
db_flexrf_1800_rx::_compute_regs(double freq, int &retR, int &retcontrol,
				 int &retN, double &retfreq)
{
  return d_common->_compute_regs(_refclk_freq(), freq, retR,
				 retcontrol, retN, retfreq);
}


//------------------------------------------------------------    


db_flexrf_900_tx::db_flexrf_900_tx(usrp_basic_sptr usrp, int which)
  : flexrf_base_tx(usrp, which)
{
  d_common = new _900_common();
}

db_flexrf_900_tx::~db_flexrf_900_tx()
{
}

bool
db_flexrf_900_tx::_compute_regs(double freq, int &retR, int &retcontrol,
				int &retN, double &retfreq)
{
  return d_common->_compute_regs(_refclk_freq(), freq, retR,
				 retcontrol, retN, retfreq);
}


db_flexrf_900_rx::db_flexrf_900_rx(usrp_basic_sptr usrp, int which)
  : flexrf_base_rx(usrp, which)
{
  d_common = new _900_common();
  set_gain((gain_min() + gain_max()) / 2.0);  // initialize gain
}

db_flexrf_900_rx::~db_flexrf_900_rx()
{
}

float
db_flexrf_900_rx::gain_min()
{
  return usrp()->pga_min();
}

float
db_flexrf_900_rx::gain_max()
{
  return usrp()->pga_max()+70;
}

float
db_flexrf_900_rx::gain_db_per_step()
{
  return 0.05;
}

bool
db_flexrf_900_rx::i_and_q_swapped()
{
  return true;
}

bool
db_flexrf_900_rx::_compute_regs(double freq, int &retR, int &retcontrol,
				int &retN, double &retfreq)
{
  return d_common->_compute_regs(_refclk_freq(), freq, retR,
				 retcontrol, retN, retfreq);
}

//------------------------------------------------------------    


db_flexrf_400_tx::db_flexrf_400_tx(usrp_basic_sptr usrp, int which)
  : flexrf_base_tx(usrp, which, POWER_UP)
{
  d_common = new _400_tx();
}

db_flexrf_400_tx::~db_flexrf_400_tx()
{
}

bool
db_flexrf_400_tx::_compute_regs(double freq, int &retR, int &retcontrol,
				int &retN, double &retfreq)
{
  return d_common->_compute_regs(_refclk_freq(), freq, retR,
				 retcontrol, retN, retfreq);
}



db_flexrf_400_rx::db_flexrf_400_rx(usrp_basic_sptr usrp, int which)
  : flexrf_base_rx(usrp, which, POWER_UP)
{
  d_common = new _400_rx();
  set_gain((gain_min() + gain_max()) / 2.0);  // initialize gain
}

db_flexrf_400_rx::~db_flexrf_400_rx()
{
}

float
db_flexrf_400_rx::gain_min()
{
  return usrp()->pga_min();
}

float
db_flexrf_400_rx::gain_max()
{
  return usrp()->pga_max()+45;
}

float

db_flexrf_400_rx::gain_db_per_step()
{
  return 0.035;
}


bool
db_flexrf_400_rx::i_and_q_swapped()
{
  return true;
}

bool
db_flexrf_400_rx::_compute_regs(double freq, int &retR, int &retcontrol,
				int &retN, double &retfreq)
{
  return d_common->_compute_regs(_refclk_freq(), freq, retR,
				 retcontrol, retN, retfreq);
}


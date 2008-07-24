/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <usrp_standard.h>

#include "usrp_prims.h"
#include "fpga_regs_common.h"
#include "fpga_regs_standard.h"
#include <stdexcept>
#include <assert.h>
#include <math.h>
#include <ad9862.h>


static const int OLD_CAPS_VAL = 0xaa55ff77;
static const int DEFAULT_CAPS_VAL = ((2 << bmFR_RB_CAPS_NDUC_SHIFT)
				     | (2 << bmFR_RB_CAPS_NDDC_SHIFT)
				     | bmFR_RB_CAPS_RX_HAS_HALFBAND);

// #define USE_FPGA_TX_CORDIC


using namespace ad9862;

#define NELEM(x) (sizeof (x) / sizeof (x[0]))


static unsigned int
compute_freq_control_word_fpga (double master_freq, double target_freq,
				double *actual_freq, bool verbose)
{
  static const int NBITS = 14;
  
  int	v = (int) rint (target_freq / master_freq * pow (2.0, 32.0));

  if (0)
    v = (v >> (32 - NBITS)) << (32 - NBITS);	// keep only top NBITS

  *actual_freq = v * master_freq / pow (2.0, 32.0);

  if (verbose)
    fprintf (stderr,
	     "compute_freq_control_word_fpga: target = %g  actual = %g  delta = %g\n",
	     target_freq, *actual_freq, *actual_freq - target_freq);

  return (unsigned int) v;
}

// The 9862 uses an unsigned 24-bit frequency tuning word and 
// a separate register to control the sign.

static unsigned int
compute_freq_control_word_9862 (double master_freq, double target_freq,
				double *actual_freq, bool verbose)
{
  double sign = 1.0;

  if (target_freq < 0)
    sign = -1.0;

  int	v = (int) rint (fabs (target_freq) / master_freq * pow (2.0, 24.0));
  *actual_freq = v * master_freq / pow (2.0, 24.0) * sign;

  if (verbose)
    fprintf (stderr,
     "compute_freq_control_word_9862: target = %g  actual = %g  delta = %g  v = %8d\n",
     target_freq, *actual_freq, *actual_freq - target_freq, v);

  return (unsigned int) v;
}

// ----------------------------------------------------------------

usrp_standard_common::usrp_standard_common(usrp_basic *parent)
{
  // read new FPGA capability register
  if (!parent->_read_fpga_reg(FR_RB_CAPS, &d_fpga_caps)){
    fprintf (stderr, "usrp_standard_common: failed to read FPGA cap register.\n");
    throw std::runtime_error ("usrp_standard_common::ctor");
  }
  // If we don't have the cap register, set the value to what it would
  // have had if we did have one ;)
  if (d_fpga_caps == OLD_CAPS_VAL)
    d_fpga_caps = DEFAULT_CAPS_VAL;

  if (0){
    fprintf(stdout, "has_rx_halfband = %d\n", has_rx_halfband());
    fprintf(stdout, "nddcs           = %d\n", nddcs());
    fprintf(stdout, "has_tx_halfband = %d\n", has_tx_halfband());
    fprintf(stdout, "nducs           = %d\n", nducs());
  }
}

bool
usrp_standard_common::has_rx_halfband() const
{
  return (d_fpga_caps & bmFR_RB_CAPS_RX_HAS_HALFBAND) ? true : false;
}

int
usrp_standard_common::nddcs() const
{
  return (d_fpga_caps & bmFR_RB_CAPS_NDDC_MASK) >> bmFR_RB_CAPS_NDDC_SHIFT;
}

bool
usrp_standard_common::has_tx_halfband() const
{
  return (d_fpga_caps & bmFR_RB_CAPS_TX_HAS_HALFBAND) ? true : false;
}

int
usrp_standard_common::nducs() const
{
  return (d_fpga_caps & bmFR_RB_CAPS_NDUC_MASK) >> bmFR_RB_CAPS_NDUC_SHIFT;
}

// ----------------------------------------------------------------

static int 
real_rx_mux_value (int mux, int nchan)
{
  if (mux != -1)
    return mux;

  return 0x32103210;
}

usrp_standard_rx::usrp_standard_rx (int which_board,
				    unsigned int decim_rate,
				    int nchan, int mux, int mode,
				    int fusb_block_size, int fusb_nblocks,
				    const std::string fpga_filename,
				    const std::string firmware_filename
				    )
  : usrp_basic_rx (which_board, fusb_block_size, fusb_nblocks,
		   fpga_filename, firmware_filename),
    usrp_standard_common(this),
    d_nchan (1), d_sw_mux (0x0), d_hw_mux (0x0)
{
  if (!set_format(make_format())){
    fprintf (stderr, "usrp_standard_rx: set_format failed\n");
    throw std::runtime_error ("usrp_standard_rx::ctor");
  }
  if (!set_nchannels (nchan)){
    fprintf (stderr, "usrp_standard_rx: set_nchannels failed\n");
    throw std::runtime_error ("usrp_standard_rx::ctor");
  }
  if (!set_decim_rate (decim_rate)){
    fprintf (stderr, "usrp_standard_rx: set_decim_rate failed\n");
    throw std::runtime_error ("usrp_standard_rx::ctor");
  }
  if (!set_mux (real_rx_mux_value (mux, nchan))){
    fprintf (stderr, "usrp_standard_rx: set_mux failed\n");
    throw std::runtime_error ("usrp_standard_rx::ctor");
  }
  if (!set_fpga_mode (mode)){
    fprintf (stderr, "usrp_standard_rx: set_fpga_mode failed\n");
    throw std::runtime_error ("usrp_standard_rx::ctor");
  }

  for (int i = 0; i < MAX_CHAN; i++){
    set_rx_freq(i, 0);
    set_ddc_phase(i, 0);
  }
}

usrp_standard_rx::~usrp_standard_rx ()
{
  // fprintf(stderr, "\nusrp_standard_rx: dtor\n");
}

bool
usrp_standard_rx::start ()
{
  if (!usrp_basic_rx::start ())
    return false;

  // add our code here

  return true;
}

bool
usrp_standard_rx::stop ()
{
  bool ok = usrp_basic_rx::stop ();

  // add our code here

  return ok;
}

usrp_standard_rx *
usrp_standard_rx::make (int which_board,
			unsigned int decim_rate,
			int nchan, int mux, int mode,
			int fusb_block_size, int fusb_nblocks,
			const std::string fpga_filename,
			const std::string firmware_filename
			)
{
  usrp_standard_rx *u = 0;
  
  try {
    u = new usrp_standard_rx (which_board, decim_rate,
			      nchan, mux, mode,
			      fusb_block_size, fusb_nblocks,
			      fpga_filename, firmware_filename);
    return u;
  }
  catch (...){
    delete u;
    return 0;
  }

  return u;
}

bool
usrp_standard_rx::set_decim_rate(unsigned int rate)
{
  if (has_rx_halfband()){
    if ((rate & 0x1) || rate < 4 || rate > 256){
      fprintf (stderr, "usrp_standard_rx::set_decim_rate: rate must be EVEN and in [4, 256]\n");
      return false;
    }
  }
  else {
    if (rate < 4 || rate > 128){
      fprintf (stderr, "usrp_standard_rx::set_decim_rate: rate must be in [4, 128]\n");
      return false;
    }
  }

  d_decim_rate = rate;
  set_usb_data_rate ((adc_rate () / rate * nchannels ())
		     * (2 * sizeof (short)));

  bool s = disable_rx ();
  int v = has_rx_halfband() ? d_decim_rate/2 - 1 : d_decim_rate - 1;
  bool ok = _write_fpga_reg (FR_DECIM_RATE, v);
  restore_rx (s);
  return ok;
}

bool usrp_standard_rx::set_nchannels (int nchan)
{
  if (!(nchan == 1 || nchan == 2 || nchan == 4))
    return false;

  if (nchan > nddcs())
    return false;

  d_nchan = nchan;

  return write_hw_mux_reg ();
}


// map software mux value to hw mux value
//
// Software mux value:
//
//    3                   2                   1                       
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +-------+-------+-------+-------+-------+-------+-------+-------+
// |   Q3  |   I3  |   Q2  |   I2  |   Q1  |   I1  |   Q0  |   I0  |
// +-------+-------+-------+-------+-------+-------+-------+-------+
//
// Each 4-bit I field is either 0,1,2,3
// Each 4-bit Q field is either 0,1,2,3 or 0xf (input is const zero)
// All Q's must be 0xf or none of them may be 0xf
//
//
// Hardware mux value:
//
//    3                   2                   1                       
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +-----------------------+-------+-------+-------+-------+-+-----+
// |      must be zero     | Q3| I3| Q2| I2| Q1| I1| Q0| I0|Z| NCH |
// +-----------------------+-------+-------+-------+-------+-+-----+


static bool
map_sw_mux_to_hw_mux (int sw_mux, int *hw_mux_ptr)
{
  // confirm that all I's are either 0,1,2,3 

  for (int i = 0; i < 8; i += 2){
    int t = (sw_mux >> (4 * i)) & 0xf;
    if (!(0 <= t && t <= 3))
      return false;
  }

  // confirm that all Q's are either 0,1,2,3 or 0xf

  for (int i = 1; i < 8; i += 2){
    int t = (sw_mux >> (4 * i)) & 0xf;
    if (!(t == 0xf || (0 <= t && t <= 3)))
      return false;
  }

  // confirm that all Q inputs are 0xf (const zero input),
  // or none of them are 0xf

  int q_and = 1;
  int q_or =  0;

  for (int i = 0; i < 4; i++){
    int qx_is_0xf = ((sw_mux >> (8 * i + 4)) & 0xf) == 0xf;
    q_and &= qx_is_0xf;
    q_or  |= qx_is_0xf;
  }

  if (q_and || !q_or){		// OK
    int hw_mux_value = 0;

    for (int i = 0; i < 8; i++){
      int t = (sw_mux >> (4 * i)) & 0x3;
      hw_mux_value |= t << (2 * i + 4);
    }

    if (q_and)
      hw_mux_value |= 0x8;	// all Q's zero

    *hw_mux_ptr = hw_mux_value;
    return true;
  }
  else
    return false;
}

bool
usrp_standard_rx::set_mux (int mux)
{
  if (!map_sw_mux_to_hw_mux (mux, &d_hw_mux))
    return false;

  // fprintf (stderr, "sw_mux = 0x%08x  hw_mux = 0x%08x\n", mux, d_hw_mux);

  d_sw_mux = mux;
  return write_hw_mux_reg ();
}

bool
usrp_standard_rx::write_hw_mux_reg ()
{
  bool s = disable_rx ();
  bool ok = _write_fpga_reg (FR_RX_MUX, d_hw_mux | d_nchan);
  restore_rx (s);
  return ok;
}


bool
usrp_standard_rx::set_rx_freq (int channel, double freq)
{
  if (channel < 0 || channel > MAX_CHAN)
    return false;

  unsigned int v =
    compute_freq_control_word_fpga (adc_freq(),
				    freq, &d_rx_freq[channel],
				    d_verbose);

  return _write_fpga_reg (FR_RX_FREQ_0 + channel, v);
}

unsigned int
usrp_standard_rx::decim_rate () const { return d_decim_rate; }

int
usrp_standard_rx::nchannels () const { return d_nchan; }

int
usrp_standard_rx::mux () const { return d_sw_mux; }

double 
usrp_standard_rx::rx_freq (int channel) const
{
  if (channel < 0 || channel >= MAX_CHAN)
    return 0;

  return d_rx_freq[channel];
}

bool
usrp_standard_rx::set_fpga_mode (int mode)
{
  return _write_fpga_reg (FR_MODE, mode);
}

bool
usrp_standard_rx::set_ddc_phase(int channel, int phase)
{
  if (channel < 0 || channel >= MAX_CHAN)
    return false;

  return _write_fpga_reg(FR_RX_PHASE_0 + channel, phase);
}


// To avoid quiet failures, check for things that our code cares about.

static bool
rx_format_is_valid(unsigned int format)
{
  int width =  usrp_standard_rx::format_width(format);
  int want_q = usrp_standard_rx::format_want_q(format);

  if (!(width == 8 || width == 16))	// FIXME add other widths when valid
    return false;

  if (!want_q)		// FIXME remove check when the rest of the code can handle I only
    return false;

  return true;
}

bool
usrp_standard_rx::set_format(unsigned int format)
{
  if (!rx_format_is_valid(format))
    return false;

  return _write_fpga_reg(FR_RX_FORMAT, format);
}

unsigned int
usrp_standard_rx::format() const
{
  return d_fpga_shadows[FR_RX_FORMAT];
}

// ----------------------------------------------------------------

unsigned int 
usrp_standard_rx::make_format(int width, int shift, bool want_q, bool bypass_halfband)
{
  unsigned int format = 
    (((width << bmFR_RX_FORMAT_WIDTH_SHIFT) & bmFR_RX_FORMAT_WIDTH_MASK)
     | (shift << bmFR_RX_FORMAT_SHIFT_SHIFT) & bmFR_RX_FORMAT_SHIFT_MASK);

  if (want_q)
    format |= bmFR_RX_FORMAT_WANT_Q;
  if (bypass_halfband)
    format |= bmFR_RX_FORMAT_BYPASS_HB;

  return format;
}

int
usrp_standard_rx::format_width(unsigned int format)
{
  return (format & bmFR_RX_FORMAT_WIDTH_MASK) >> bmFR_RX_FORMAT_WIDTH_SHIFT;
}

int
usrp_standard_rx::format_shift(unsigned int format)
{
  return (format & bmFR_RX_FORMAT_SHIFT_MASK) >> bmFR_RX_FORMAT_SHIFT_SHIFT;
}

bool
usrp_standard_rx::format_want_q(unsigned int format)
{
  return (format & bmFR_RX_FORMAT_WANT_Q) != 0;
}

bool
usrp_standard_rx::format_bypass_halfband(unsigned int format)
{
  return (format & bmFR_RX_FORMAT_BYPASS_HB) != 0;
}

//////////////////////////////////////////////////////////////////


// tx data is timed to CLKOUT1 (64 MHz)
// interpolate 4x
// fine modulator enabled


static unsigned char tx_regs_use_nco[] = {
  REG_TX_IF,		(TX_IF_USE_CLKOUT1
			 | TX_IF_I_FIRST
			 | TX_IF_2S_COMP
			 | TX_IF_INTERLEAVED),
  REG_TX_DIGITAL,	(TX_DIGITAL_2_DATA_PATHS
			 | TX_DIGITAL_INTERPOLATE_4X)
};


static int
real_tx_mux_value (int mux, int nchan)
{
  if (mux != -1)
    return mux;

  switch (nchan){
  case 1:
    return 0x0098;
  case 2:
    return 0xba98;
  default:
    assert (0);
  }
}

usrp_standard_tx::usrp_standard_tx (int which_board,
				    unsigned int interp_rate,
				    int nchan, int mux,
				    int fusb_block_size, int fusb_nblocks,
				    const std::string fpga_filename,
				    const std::string firmware_filename
				    )
  : usrp_basic_tx (which_board, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename),
    usrp_standard_common(this),
    d_sw_mux (0x8), d_hw_mux (0x81)
{
  if (!usrp_9862_write_many_all (d_udh, tx_regs_use_nco, sizeof (tx_regs_use_nco))){
    fprintf (stderr, "usrp_standard_tx: failed to init AD9862 TX regs\n");
    throw std::runtime_error ("usrp_standard_tx::ctor");
  }
  if (!set_nchannels (nchan)){
    fprintf (stderr, "usrp_standard_tx: set_nchannels failed\n");
    throw std::runtime_error ("usrp_standard_tx::ctor");
  }
  if (!set_interp_rate (interp_rate)){
    fprintf (stderr, "usrp_standard_tx: set_interp_rate failed\n");
    throw std::runtime_error ("usrp_standard_tx::ctor");
  }
  if (!set_mux (real_tx_mux_value (mux, nchan))){
    fprintf (stderr, "usrp_standard_tx: set_mux failed\n");
    throw std::runtime_error ("usrp_standard_tx::ctor");
  }

  for (int i = 0; i < MAX_CHAN; i++){
    d_tx_modulator_shadow[i] = (TX_MODULATOR_DISABLE_NCO
				| TX_MODULATOR_COARSE_MODULATION_NONE);
    d_coarse_mod[i] = CM_OFF;
    set_tx_freq (i, 0);
  }
}

usrp_standard_tx::~usrp_standard_tx ()
{
  // fprintf(stderr, "\nusrp_standard_tx: dtor\n");
}

bool
usrp_standard_tx::start ()
{
  if (!usrp_basic_tx::start ())
    return false;

  // add our code here

  return true;
}

bool
usrp_standard_tx::stop ()
{
  bool ok = usrp_basic_tx::stop ();

  // add our code here

  return ok;
}

usrp_standard_tx *
usrp_standard_tx::make (int which_board,
			unsigned int interp_rate,
			int nchan, int mux,
			int fusb_block_size, int fusb_nblocks,
			const std::string fpga_filename,
			const std::string firmware_filename
			)
{
  usrp_standard_tx *u = 0;
  
  try {
    u = new usrp_standard_tx (which_board, interp_rate, nchan, mux,
			      fusb_block_size, fusb_nblocks,
			      fpga_filename, firmware_filename);
    return u;
  }
  catch (...){
    delete u;
    return 0;
  }

  return u;
}

bool
usrp_standard_tx::set_interp_rate (unsigned int rate)
{
  // fprintf (stderr, "usrp_standard_tx::set_interp_rate\n");

  if ((rate & 0x3) || rate < 4 || rate > 512){
    fprintf (stderr, "usrp_standard_tx::set_interp_rate: rate must be in [4, 512] and a multiple of 4.\n");
    return false;
  }

  d_interp_rate = rate;
  set_usb_data_rate ((dac_rate () / rate * nchannels ())
		     * (2 * sizeof (short)));

  // We're using the interp by 4 feature of the 9862 so that we can
  // use its fine modulator.  Thus, we reduce the FPGA's interpolation rate
  // by a factor of 4.

  bool s = disable_tx ();
  bool ok = _write_fpga_reg (FR_INTERP_RATE, d_interp_rate/4 - 1);
  restore_tx (s);
  return ok;
}

bool
usrp_standard_tx::set_nchannels (int nchan)
{
  if (!(nchan == 1 || nchan == 2))
    return false;

  if (nchan > nducs())
    return false;

  d_nchan = nchan;
  return write_hw_mux_reg ();
}

bool
usrp_standard_tx::set_mux (int mux)
{
  d_sw_mux = mux;
  d_hw_mux = mux << 4;
  return write_hw_mux_reg ();
}

bool
usrp_standard_tx::write_hw_mux_reg ()
{
  bool s = disable_tx ();
  bool ok = _write_fpga_reg (FR_TX_MUX, d_hw_mux | d_nchan);
  restore_tx (s);
  return ok;
}

#ifdef USE_FPGA_TX_CORDIC

bool
usrp_standard_tx::set_tx_freq (int channel, double freq)
{
  if (channel < 0 || channel >= MAX_CHAN)
    return false;

  // This assumes we're running the 4x on-chip interpolator.

  unsigned int v =
    compute_freq_control_word_fpga (dac_freq () / 4,
				    freq, &d_tx_freq[channel],
				    d_verbose);

  return _write_fpga_reg (FR_TX_FREQ_0 + channel, v);
}


#else

bool
usrp_standard_tx::set_tx_freq (int channel, double freq)
{
  if (channel < 0 || channel >= MAX_CHAN)
    return false;

  // split freq into fine and coarse components

  coarse_mod_t	cm;
  double	coarse;

  assert (dac_freq () == 128000000);

  if (freq < -44e6)		// too low
    return false;
  else if (freq < -24e6){	// [-44, -24)
    cm = CM_NEG_FDAC_OVER_4;
    coarse = -dac_freq () / 4;
  }
  else if (freq < -8e6){	// [-24, -8)
    cm = CM_NEG_FDAC_OVER_8;
    coarse = -dac_freq () / 8;
  }
  else if (freq < 8e6){		// [-8, 8)
    cm = CM_OFF;
    coarse = 0;
  }
  else if (freq < 24e6){	// [8, 24)
    cm = CM_POS_FDAC_OVER_8;
    coarse = dac_freq () / 8;
  }
  else if (freq <= 44e6){	// [24, 44]
    cm = CM_POS_FDAC_OVER_4;
    coarse = dac_freq () / 4;
  }
  else				// too high
    return false;


  set_coarse_modulator (channel, cm);	// set bits in d_tx_modulator_shadow

  double fine = freq - coarse;


  // Compute fine tuning word...
  // This assumes we're running the 4x on-chip interpolator.
  // (This is required to use the fine modulator.)

  unsigned int v =
    compute_freq_control_word_9862 (dac_freq () / 4,
				    fine, &d_tx_freq[channel], d_verbose);

  d_tx_freq[channel] += coarse;		// adjust actual
  
  unsigned char high, mid, low;

  high = (v >> 16) & 0xff;
  mid =  (v >>  8) & 0xff;
  low =  (v >>  0) & 0xff;

  bool ok = true;

  // write the fine tuning word
  ok &= _write_9862 (channel, REG_TX_NCO_FTW_23_16, high);
  ok &= _write_9862 (channel, REG_TX_NCO_FTW_15_8,  mid);
  ok &= _write_9862 (channel, REG_TX_NCO_FTW_7_0,   low);


  d_tx_modulator_shadow[channel] |= TX_MODULATOR_ENABLE_NCO;

  if (fine < 0)
    d_tx_modulator_shadow[channel] |= TX_MODULATOR_NEG_FINE_TUNE;
  else
    d_tx_modulator_shadow[channel] &= ~TX_MODULATOR_NEG_FINE_TUNE;

  ok &=_write_9862 (channel, REG_TX_MODULATOR, d_tx_modulator_shadow[channel]);

  return ok;
}
#endif

bool
usrp_standard_tx::set_coarse_modulator (int channel, coarse_mod_t cm)
{
  if (channel < 0 || channel >= MAX_CHAN)
    return false;

  switch (cm){
  case CM_NEG_FDAC_OVER_4:
    d_tx_modulator_shadow[channel] &= ~TX_MODULATOR_CM_MASK;
    d_tx_modulator_shadow[channel] |= TX_MODULATOR_COARSE_MODULATION_F_OVER_4;
    d_tx_modulator_shadow[channel] |= TX_MODULATOR_NEG_COARSE_TUNE;
    break;

  case CM_NEG_FDAC_OVER_8:
    d_tx_modulator_shadow[channel] &= ~TX_MODULATOR_CM_MASK;
    d_tx_modulator_shadow[channel] |= TX_MODULATOR_COARSE_MODULATION_F_OVER_8;
    d_tx_modulator_shadow[channel] |= TX_MODULATOR_NEG_COARSE_TUNE;
    break;

  case CM_OFF:
    d_tx_modulator_shadow[channel] &= ~TX_MODULATOR_CM_MASK;
    break;

  case CM_POS_FDAC_OVER_8:
    d_tx_modulator_shadow[channel] &= ~TX_MODULATOR_CM_MASK;
    d_tx_modulator_shadow[channel] |= TX_MODULATOR_COARSE_MODULATION_F_OVER_8;
    break;

  case CM_POS_FDAC_OVER_4:
    d_tx_modulator_shadow[channel] &= ~TX_MODULATOR_CM_MASK;
    d_tx_modulator_shadow[channel] |= TX_MODULATOR_COARSE_MODULATION_F_OVER_4;
    break;

  default:
    return false;
  }

  d_coarse_mod[channel] = cm;
  return true;
}

unsigned int
usrp_standard_tx::interp_rate () const { return d_interp_rate; }

int
usrp_standard_tx::nchannels () const { return d_nchan; }

int
usrp_standard_tx::mux () const { return d_sw_mux; }

double
usrp_standard_tx::tx_freq (int channel) const
{
  if (channel < 0 || channel >= MAX_CHAN)
    return 0;

  return d_tx_freq[channel];
}

usrp_standard_tx::coarse_mod_t
usrp_standard_tx::coarse_modulator (int channel) const
{
  if (channel < 0 || channel >= MAX_CHAN)
    return CM_OFF;

  return d_coarse_mod[channel];
}

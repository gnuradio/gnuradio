/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2009 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <usrp/usrp_standard.h>

#include "usrp/usrp_prims.h"
#include "fpga_regs_common.h"
#include "fpga_regs_standard.h"
#include <stdexcept>
#include <assert.h>
#include <math.h>
#include <ad9862.h>
#include <cstdio>


static const int OLD_CAPS_VAL = 0xaa55ff77;
static const int DEFAULT_CAPS_VAL = ((2 << bmFR_RB_CAPS_NDUC_SHIFT)
				     | (2 << bmFR_RB_CAPS_NDDC_SHIFT)
				     | bmFR_RB_CAPS_RX_HAS_HALFBAND);

// #define USE_FPGA_TX_CORDIC


using namespace ad9862;

#define NELEM(x) (sizeof (x) / sizeof (x[0]))


void
usrp_standard_common::calc_dxc_freq(double target_freq, double baseband_freq, double fs,
				    double *dxc_freq, bool *inverted)
{
  /*
    Calculate the frequency to use for setting the digital up or down converter.
    
    @param target_freq: desired RF frequency (Hz)
    @param baseband_freq: the RF frequency that corresponds to DC in the IF.
    @param fs: converter sample rate
    
    @returns: 2-tuple (ddc_freq, inverted) where ddc_freq is the value
      for the ddc and inverted is True if we're operating in an inverted
      Nyquist zone.
  */

#if 0
    printf("calc_dxc_freq:\n");
    printf("  target   = %f\n", target_freq);
    printf("  baseband = %f\n", baseband_freq);
    printf("  fs       = %f\n", fs);
#endif

  double delta = target_freq - baseband_freq;
    
  if(delta >= 0) {
    while(delta > fs) {
      delta -= fs;
    }
    if(delta <= fs/2) {		// non-inverted region
      *dxc_freq = -delta;	
      *inverted = false;
    }
    else {     			// inverted region
      *dxc_freq = delta - fs;
      *inverted = true;
    }
  }
  else {
    while(delta < -fs) {
      delta += fs;
    }
    if(delta >= -fs/2) {
      *dxc_freq = -delta;	// non-inverted region
      *inverted = false;
    }
    else {			// inverted region
      *dxc_freq = delta + fs;
      *inverted = true;
    }
  }

#if 0
    printf("  dxc_freq  = %f\n", *dxc_freq);
    printf("  inverted  = %s\n", *inverted ? "true" : "false");
#endif
}


/* 
 * Real lambda expressions would be _so_ much easier...
 */
class dxc_control {
public:
  virtual bool is_tx() = 0;
  virtual bool set_dxc_freq(double dxc_freq) = 0;
  virtual double dxc_freq() = 0;
};

class ddc_control : public dxc_control {
  usrp_standard_rx     *d_u;
  int			d_chan;

public:
  ddc_control(usrp_standard_rx *u, int chan)
    : d_u(u), d_chan(chan) {}
  
  bool is_tx(){ return false; }
  bool set_dxc_freq(double dxc_freq){ return d_u->set_rx_freq(d_chan, dxc_freq); }
  double dxc_freq(){ return d_u->rx_freq(d_chan); }
};

class duc_control : public dxc_control {
  usrp_standard_tx     *d_u;
  int			d_chan;

public:
  duc_control(usrp_standard_tx *u, int chan)
    : d_u(u), d_chan(chan) {}
  
  bool is_tx(){ return true; }
  bool set_dxc_freq(double dxc_freq){ return d_u->set_tx_freq(d_chan, dxc_freq); }
  double dxc_freq() { return d_u->tx_freq(d_chan); }
};


/*!
 * \brief Tune such that target_frequency ends up at DC in the complex baseband
 *
 * \param db		the daughterboard to use
 * \param target_freq	the center frequency we want at baseband (DC)
 * \param fs 		the sample rate
 * \param dxc		DDC or DUC access and control object
 * \param[out] result	details of what we did
 *
 * \returns true iff operation was successful
 *
 * Tuning is a two step process.  First we ask the front-end to
 * tune as close to the desired frequency as it can.  Then we use
 * the result of that operation and our target_frequency to
 * determine the value for the digital down converter.
 */
static bool
tune_a_helper(db_base_sptr db, double target_freq, double fs,
	      dxc_control &dxc, usrp_tune_result *result)
{
  bool inverted = false;
  double dxc_freq;
  double actual_dxc_freq;

  // Ask the d'board to tune as closely as it can to target_freq
#if 0
  bool ok = db->set_freq(target_freq, &result->baseband_freq);
#else
  bool ok;
  {
    freq_result_t fr = db->set_freq(target_freq);
    ok = fr.ok;
    result->baseband_freq = fr.baseband_freq;
  }
#endif

  // Calculate the DDC setting that will downconvert the baseband from the
  // daughterboard to our target frequency.
  usrp_standard_common::calc_dxc_freq(target_freq, result->baseband_freq, fs,
				      &dxc_freq, &inverted);

  // If the spectrum is inverted, and the daughterboard doesn't do
  // quadrature downconversion, we can fix the inversion by flipping the
  // sign of the dxc_freq...  (This only happens using the basic_rx board)
  
  if(db->spectrum_inverted())
    inverted = !inverted;
  
  if(inverted && !db->is_quadrature()){
    dxc_freq = -dxc_freq;
    inverted = !inverted;
  }
  
  if (dxc.is_tx() && !db->i_and_q_swapped())		// down conversion versus up conversion
    dxc_freq = -dxc_freq;

  ok &= dxc.set_dxc_freq(dxc_freq);
  actual_dxc_freq = dxc.dxc_freq();
  
  result->dxc_freq = dxc_freq;
  result->residual_freq = dxc_freq - actual_dxc_freq;
  result->inverted = inverted;
  return ok;
}


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

usrp_standard_rx_sptr
usrp_standard_rx::make (int which_board,
			unsigned int decim_rate,
			int nchan, int mux, int mode,
			int fusb_block_size, int fusb_nblocks,
			const std::string fpga_filename,
			const std::string firmware_filename
			)
{
  try {
    usrp_standard_rx_sptr u = 
      usrp_standard_rx_sptr(new usrp_standard_rx(which_board, decim_rate,
						 nchan, mux, mode,
						 fusb_block_size, fusb_nblocks,
						 fpga_filename, firmware_filename));
    u->init_db(u);
    return u;
  }
  catch (...){
    return usrp_standard_rx_sptr();
  }
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

int
usrp_standard_rx::determine_rx_mux_value(const usrp_subdev_spec &ss)
{
  /*
    Determine appropriate Rx mux value as a function of the subdevice choosen and the
    characteristics of the respective daughterboard.
    
    @param u:           instance of USRP source
    @param subdev_spec: return value from subdev option parser.  
    @type  subdev_spec: (side, subdev), where side is 0 or 1 and subdev is 0 or 1
    @returns:           the Rx mux value
  
    Figure out which A/D's to connect to the DDC.
    
    Each daughterboard consists of 1 or 2 subdevices.  (At this time,
    all but the Basic Rx have a single subdevice.  The Basic Rx
    has two independent channels, treated as separate subdevices).
    subdevice 0 of a daughterboard may use 1 or 2 A/D's.  We determine this
    by checking the is_quadrature() method.  If subdevice 0 uses only a single
    A/D, it's possible that the daughterboard has a second subdevice, subdevice 1,
    and it uses the second A/D.
    
    If the card uses only a single A/D, we wire a zero into the DDC Q input.
    
    (side, 0) says connect only the A/D's used by subdevice 0 to the DDC.
    (side, 1) says connect only the A/D's used by subdevice 1 to the DDC.
  */

  struct truth_table_element
  {
    int          d_side;
    int 	 d_uses;
    bool         d_swap_iq;
    unsigned int d_mux_val;

    truth_table_element(int side, unsigned int uses, bool swap_iq, unsigned int mux_val=0)
      : d_side(side), d_uses(uses), d_swap_iq(swap_iq), d_mux_val(mux_val){}
      
    bool operator==(const truth_table_element &in)
    {
      return (d_side == in.d_side && d_uses == in.d_uses && d_swap_iq == in.d_swap_iq);
    }

    unsigned int mux_val() { return d_mux_val; }
  };


  if (!is_valid(ss))
    throw std::invalid_argument("subdev_spec");


  // This is a tuple of length 1 or 2 containing the subdevice
  // classes for the selected side.
  std::vector<db_base_sptr> db = this->db(ss.side);
  
  unsigned int uses;

  // compute bitmasks of used A/D's
  
  if(db[ss.subdev]->is_quadrature())
    uses = 0x3;               // uses A/D 0 and 1
  else if (ss.subdev == 0)
    uses = 0x1;               // uses A/D 0 only
  else if(ss.subdev == 1)
    uses = 0x2;               // uses A/D 1 only
  else
    uses = 0x0;               // uses no A/D (doesn't exist)
  
  if(uses == 0){
    throw std::runtime_error("Determine RX Mux Error");
  }
  
  bool swap_iq = db[ss.subdev]->i_and_q_swapped();
  
  truth_table_element truth_table[8] = {
    // (side, uses, swap_iq) : mux_val
    truth_table_element(0, 0x1, false, 0xf0f0f0f0),
    truth_table_element(0, 0x2, false, 0xf0f0f0f1),
    truth_table_element(0, 0x3, false, 0x00000010),
    truth_table_element(0, 0x3, true,  0x00000001),
    truth_table_element(1, 0x1, false, 0xf0f0f0f2),
    truth_table_element(1, 0x2, false, 0xf0f0f0f3),
    truth_table_element(1, 0x3, false, 0x00000032),
    truth_table_element(1, 0x3, true,  0x00000023)
  };
  size_t nelements = sizeof(truth_table)/sizeof(truth_table[0]);
  
  truth_table_element target(ss.side, uses, swap_iq, 0);
  
  size_t i;
  for(i = 0; i < nelements; i++){
    if (truth_table[i] == target)
      return truth_table[i].mux_val();
  }
  throw std::runtime_error("internal error");
}

int
usrp_standard_rx::determine_rx_mux_value(const usrp_subdev_spec &ss_a, const usrp_subdev_spec &ss_b)
{
  std::vector<db_base_sptr> db_a = this->db(ss_a.side);
  std::vector<db_base_sptr> db_b = this->db(ss_b.side);
  if (db_a[ss_a.subdev]->is_quadrature() != db_b[ss_b.subdev]->is_quadrature()){
    throw std::runtime_error("Cannot compute dual mux when mixing quadrature and non-quadrature subdevices");
  }
  int mux_a = determine_rx_mux_value(ss_a);
  int mux_b = determine_rx_mux_value(ss_b);
  //move the lower byte of the mux b into the second byte of the mux a
  return ((mux_b & 0xff) << 8) | (mux_a & 0xffff00ff);
}

bool
usrp_standard_rx::set_rx_freq (int channel, double freq)
{
  if (channel < 0 || channel > MAX_CHAN)
    return false;

  unsigned int v =
    compute_freq_control_word_fpga (adc_rate(),
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
     | ((shift << bmFR_RX_FORMAT_SHIFT_SHIFT) & bmFR_RX_FORMAT_SHIFT_MASK));

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

bool
usrp_standard_rx::tune(int chan, db_base_sptr db, double target_freq, usrp_tune_result *result)
{
  ddc_control dxc(this, chan);
  return tune_a_helper(db, target_freq, converter_rate(), dxc, result);
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

usrp_standard_tx_sptr
usrp_standard_tx::make (int which_board,
			unsigned int interp_rate,
			int nchan, int mux,
			int fusb_block_size, int fusb_nblocks,
			const std::string fpga_filename,
			const std::string firmware_filename
			)
{
  try {
    usrp_standard_tx_sptr u  = 
      usrp_standard_tx_sptr(new usrp_standard_tx(which_board, interp_rate, nchan, mux,
						 fusb_block_size, fusb_nblocks,
						 fpga_filename, firmware_filename));
    u->init_db(u);
    return u;
  }
  catch (...){
    return usrp_standard_tx_sptr();
  }
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

int
usrp_standard_tx::determine_tx_mux_value(const usrp_subdev_spec &ss)
{
  /*
    Determine appropriate Tx mux value as a function of the subdevice choosen.

    @param u:           instance of USRP source
    @param subdev_spec: return value from subdev option parser.  
    @type  subdev_spec: (side, subdev), where side is 0 or 1 and subdev is 0
    @returns:           the Rx mux value
  
    This is simpler than the rx case.  Either you want to talk
    to side A or side B.  If you want to talk to both sides at once,
    determine the value manually.
  */

  if (!is_valid(ss))
    throw std::invalid_argument("subdev_spec");

  std::vector<db_base_sptr> db = this->db(ss.side);
  
  if(db[ss.subdev]->i_and_q_swapped()) {
    unsigned int mask[2] = {0x0089, 0x8900};
    return mask[ss.side];
  }
  else {
    unsigned int mask[2] = {0x0098, 0x9800};
    return mask[ss.side];
  }
}

int
usrp_standard_tx::determine_tx_mux_value(const usrp_subdev_spec &ss_a, const usrp_subdev_spec &ss_b)
{
  if (ss_a.side == ss_b.side && ss_a.subdev == ss_b.subdev){
    throw std::runtime_error("Cannot compute dual mux, repeated subdevice");
  }
  int mux_a = determine_tx_mux_value(ss_a);
  //Get the mux b:
  //	DAC0 becomes DAC2
  //	DAC1 becomes DAC3
  unsigned int mask[2] = {0x0022, 0x2200};
  int mux_b = determine_tx_mux_value(ss_b) + mask[ss_b.side];
  return mux_b | mux_a;
}

#ifdef USE_FPGA_TX_CORDIC

bool
usrp_standard_tx::set_tx_freq (int channel, double freq)
{
  if (channel < 0 || channel >= MAX_CHAN)
    return false;

  // This assumes we're running the 4x on-chip interpolator.

  unsigned int v =
    compute_freq_control_word_fpga (dac_rate () / 4,
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

  double coarse_freq_1 = dac_rate () / 8; // First coarse frequency
  double coarse_freq_2 = dac_rate () / 4; // Second coarse frequency
  double coarse_limit_1 = coarse_freq_1 / 2; // Midpoint of [0 , freq1] range
  double coarse_limit_2 = (coarse_freq_1 + coarse_freq_2) / 2; // Midpoint of [freq1 , freq2] range
  double high_limit = (double)44e6/128e6*dac_rate (); // Highest meaningful frequency

  if (freq < -high_limit)		// too low
    return false;
  else if (freq < -coarse_limit_2){	// For 64MHz: [-44, -24)
    cm = CM_NEG_FDAC_OVER_4;
    coarse = -coarse_freq_2;
  }
  else if (freq < -coarse_limit_1){	// For 64MHz: [-24, -8)
    cm = CM_NEG_FDAC_OVER_8;
    coarse = -coarse_freq_1;
  }
  else if (freq < coarse_limit_1){		// For 64MHz: [-8, 8)
    cm = CM_OFF;
    coarse = 0;
  }
  else if (freq < coarse_limit_2){	// For 64MHz: [8, 24)
    cm = CM_POS_FDAC_OVER_8;
    coarse = coarse_freq_1;
  }
  else if (freq <= high_limit){	// For 64MHz: [24, 44]
    cm = CM_POS_FDAC_OVER_4;
    coarse = coarse_freq_2;
  }
  else				// too high
    return false;


  set_coarse_modulator (channel, cm);	// set bits in d_tx_modulator_shadow

  double fine = freq - coarse;


  // Compute fine tuning word...
  // This assumes we're running the 4x on-chip interpolator.
  // (This is required to use the fine modulator.)

  unsigned int v =
    compute_freq_control_word_9862 (dac_rate () / 4,
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

bool
usrp_standard_tx::tune(int chan, db_base_sptr db, double target_freq, usrp_tune_result *result)
{
  duc_control dxc(this, chan);
  return tune_a_helper(db, target_freq, converter_rate(), dxc, result);
}

/* -*- c++ -*- */
/*
 * Copyright 2003,2004 Free Software Foundation, Inc.
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

#include "usrp_basic.h"
#include "usrp_prims.h"
#include "usrp_interfaces.h"
#include "fpga_regs_common.h"
#include "fusb.h"
#include <usb.h>
#include <stdexcept>
#include <assert.h>
#include <math.h>
#include <ad9862.h>
#include <string.h>

using namespace ad9862;

#define NELEM(x) (sizeof (x) / sizeof (x[0]))

// These set the buffer size used for each end point using the fast
// usb interface.  The kernel ends up locking down this much memory.

static const int FUSB_BUFFER_SIZE = fusb_sysconfig::default_buffer_size();
static const int FUSB_BLOCK_SIZE = fusb_sysconfig::max_block_size();
static const int FUSB_NBLOCKS    = FUSB_BUFFER_SIZE / FUSB_BLOCK_SIZE;


static const double POLLING_INTERVAL = 0.1;	// seconds

////////////////////////////////////////////////////////////////

static struct usb_dev_handle *
open_rx_interface (struct usb_device *dev)
{
  struct usb_dev_handle *udh = usrp_open_rx_interface (dev);
  if (udh == 0){
    fprintf (stderr, "usrp_basic_rx: can't open rx interface\n");
    usb_strerror ();
  }
  return udh;
}

static struct usb_dev_handle *
open_tx_interface (struct usb_device *dev)
{
  struct usb_dev_handle *udh = usrp_open_tx_interface (dev);
  if (udh == 0){
    fprintf (stderr, "usrp_basic_tx: can't open tx interface\n");
    usb_strerror ();
  }
  return udh;
}


//////////////////////////////////////////////////////////////////
//
//			usrp_basic
//
////////////////////////////////////////////////////////////////


// Given:
//   CLKIN = 64 MHz
//   CLKSEL pin = high 
//
// These settings give us:
//   CLKOUT1 = CLKIN = 64 MHz
//   CLKOUT2 = CLKIN = 64 MHz
//   ADC is clocked at  64 MHz
//   DAC is clocked at 128 MHz

static unsigned char common_regs[] = {
  REG_GENERAL,		0,
  REG_DLL,		(DLL_DISABLE_INTERNAL_XTAL_OSC
			 | DLL_MULT_2X
			 | DLL_FAST),
  REG_CLKOUT,		CLKOUT2_EQ_DLL_OVER_2,
  REG_AUX_ADC_CLK,	AUX_ADC_CLK_CLK_OVER_4
};


usrp_basic::usrp_basic (int which_board, 
			struct usb_dev_handle *
			open_interface (struct usb_device *dev),
			const std::string fpga_filename,
			const std::string firmware_filename)
  : d_udh (0),
    d_usb_data_rate (16000000),	// SWAG, see below
    d_bytes_per_poll ((int) (POLLING_INTERVAL * d_usb_data_rate)),
    d_verbose (false)
{
  /*
   * SWAG: Scientific Wild Ass Guess.
   *
   * d_usb_data_rate is used only to determine how often to poll for over- and under-runs.
   * We defualt it to 1/2  of our best case.  Classes derived from usrp_basic (e.g., 
   * usrp_standard_tx and usrp_standard_rx) call set_usb_data_rate() to tell us the
   * actual rate.  This doesn't change our throughput, that's determined by the signal
   * processing code in the FPGA (which we know nothing about), and the system limits
   * determined by libusb, fusb_*, and the underlying drivers.
   */
  memset (d_fpga_shadows, 0, sizeof (d_fpga_shadows));

  usrp_one_time_init ();

  if (!usrp_load_standard_bits (which_board, false, fpga_filename, firmware_filename))
    throw std::runtime_error ("usrp_basic/usrp_load_standard_bits");

  struct usb_device *dev = usrp_find_device (which_board);
  if (dev == 0){
    fprintf (stderr, "usrp_basic: can't find usrp[%d]\n", which_board);
    throw std::runtime_error ("usrp_basic/usrp_find_device");
  }

  if (!(usrp_usrp_p(dev) && usrp_hw_rev(dev) >= 1)){
    fprintf (stderr, "usrp_basic: sorry, this code only works with USRP revs >= 1\n");
    throw std::runtime_error ("usrp_basic/bad_rev");
  }

  if ((d_udh = open_interface (dev)) == 0)
    throw std::runtime_error ("usrp_basic/open_interface");

  // initialize registers that are common to rx and tx

  if (!usrp_9862_write_many_all (d_udh, common_regs, sizeof (common_regs))){
    fprintf (stderr, "usrp_basic: failed to init common AD9862 regs\n");
    throw std::runtime_error ("usrp_basic/init_9862");
  }

  _write_fpga_reg (FR_MODE, 0);		// ensure we're in normal mode
  _write_fpga_reg (FR_DEBUG_EN, 0);	// disable debug outputs
}

usrp_basic::~usrp_basic ()
{
  if (d_udh)
    usb_close (d_udh);
}

bool
usrp_basic::start ()
{
  return true;		// nop
}

bool
usrp_basic::stop ()
{
  return true;		// nop
}

void
usrp_basic::set_usb_data_rate (int usb_data_rate)
{
  d_usb_data_rate = usb_data_rate;
  d_bytes_per_poll = (int) (usb_data_rate * POLLING_INTERVAL);
}

bool
usrp_basic::write_aux_dac (int slot, int which_dac, int value)
{
  return usrp_write_aux_dac (d_udh, slot, which_dac, value);
}

bool
usrp_basic::read_aux_adc (int slot, int which_adc, int *value)
{
  return usrp_read_aux_adc (d_udh, slot, which_adc, value);
}

int
usrp_basic::read_aux_adc (int slot, int which_adc)
{
  int	value;
  if (!read_aux_adc (slot, which_adc, &value))
    return READ_FAILED;

  return value;
}

bool
usrp_basic::write_eeprom (int i2c_addr, int eeprom_offset, const std::string buf)
{
  return usrp_eeprom_write (d_udh, i2c_addr, eeprom_offset, buf.data (), buf.size ());
}

std::string
usrp_basic::read_eeprom (int i2c_addr, int eeprom_offset, int len)
{
  if (len <= 0)
    return "";

  char buf[len];

  if (!usrp_eeprom_read (d_udh, i2c_addr, eeprom_offset, buf, len))
    return "";

  return std::string (buf, len);
}

bool
usrp_basic::write_i2c (int i2c_addr, const std::string buf)
{
  return usrp_i2c_write (d_udh, i2c_addr, buf.data (), buf.size ());
}

std::string
usrp_basic::read_i2c (int i2c_addr, int len)
{
  if (len <= 0)
    return "";

  char buf[len];

  if (!usrp_i2c_read (d_udh, i2c_addr, buf, len))
    return "";

  return std::string (buf, len);
}

std::string
usrp_basic::serial_number()
{
  return usrp_serial_number(d_udh);
}

// ----------------------------------------------------------------

bool
usrp_basic::set_adc_offset (int which, int offset)
{
  if (which < 0 || which > 3)
    return false;

  return _write_fpga_reg (FR_ADC_OFFSET_0 + which, offset);
}

bool
usrp_basic::set_dac_offset (int which, int offset, int offset_pin)
{
  if (which < 0 || which > 3)
    return false;

  int which_codec = which >> 1;
  int tx_a = (which & 0x1) == 0;
  int lo = ((offset & 0x3) << 6) | (offset_pin & 0x1);
  int hi = (offset >> 2);
  bool ok;

  if (tx_a){
    ok =  _write_9862 (which_codec, REG_TX_A_OFFSET_LO, lo);
    ok &= _write_9862 (which_codec, REG_TX_A_OFFSET_HI, hi);
  }
  else {
    ok =  _write_9862 (which_codec, REG_TX_B_OFFSET_LO, lo);
    ok &= _write_9862 (which_codec, REG_TX_B_OFFSET_HI, hi);
  }
  return ok;
}

bool
usrp_basic::set_adc_buffer_bypass (int which, bool bypass)
{
  if (which < 0 || which > 3)
    return false;

  int codec = which >> 1;
  int reg = (which & 1) == 0 ? REG_RX_A : REG_RX_B;

  unsigned char cur_rx;
  unsigned char cur_pwr_dn;

  // If the input buffer is bypassed, we need to power it down too.

  bool ok = _read_9862 (codec, reg, &cur_rx);
  ok &= _read_9862 (codec, REG_RX_PWR_DN, &cur_pwr_dn);
  if (!ok)
    return false;

  if (bypass){
    cur_rx |= RX_X_BYPASS_INPUT_BUFFER;
    cur_pwr_dn |= ((which & 1) == 0) ? RX_PWR_DN_BUF_A : RX_PWR_DN_BUF_B;
  }
  else {
    cur_rx &= ~RX_X_BYPASS_INPUT_BUFFER;
    cur_pwr_dn &= ~(((which & 1) == 0) ? RX_PWR_DN_BUF_A : RX_PWR_DN_BUF_B);
  }

  ok &= _write_9862 (codec, reg, cur_rx);
  ok &= _write_9862 (codec, REG_RX_PWR_DN, cur_pwr_dn);
  return ok;
}

// ----------------------------------------------------------------

bool
usrp_basic::_write_fpga_reg (int regno, int value)
{
  if (d_verbose){
    fprintf (stdout, "_write_fpga_reg(%3d, 0x%08x)\n", regno, value);
    fflush (stdout);
  }

  if (regno >= 0 && regno < MAX_REGS)
    d_fpga_shadows[regno] = value;

  return usrp_write_fpga_reg (d_udh, regno, value);
}

bool
usrp_basic::_write_fpga_reg_masked (int regno, int value, int mask)
{
  //Only use this for registers who actually use a mask in the verilog firmware, like FR_RX_MASTER_SLAVE
  //value is a 16 bits value and mask is a 16 bits mask
  if (d_verbose){
    fprintf (stdout, "_write_fpga_reg_masked(%3d, 0x%04x,0x%04x)\n", regno, value, mask);
    fflush (stdout);
  }

  if (regno >= 0 && regno < MAX_REGS)
    d_fpga_shadows[regno] = value;

  return usrp_write_fpga_reg (d_udh, regno, (value & 0xffff) | ((mask & 0xffff)<<16));
}


bool
usrp_basic::_read_fpga_reg (int regno, int *value)
{
  return usrp_read_fpga_reg (d_udh, regno, value);
}

int
usrp_basic::_read_fpga_reg (int regno)
{
  int value;
  if (!_read_fpga_reg (regno, &value))
    return READ_FAILED;
  return value;
}

bool
usrp_basic::_write_9862 (int which_codec, int regno, unsigned char value)
{
  if (0 && d_verbose){
    // FIXME really want to enable logging in usrp_prims:usrp_9862_write
    fprintf(stdout, "_write_9862(codec = %d, regno = %2d, val = 0x%02x)\n", which_codec, regno, value);
    fflush(stdout);
  }

  return usrp_9862_write (d_udh, which_codec, regno, value);
}


bool
usrp_basic::_read_9862 (int which_codec, int regno, unsigned char *value) const
{
  return usrp_9862_read (d_udh, which_codec, regno, value);
}

int
usrp_basic::_read_9862 (int which_codec, int regno) const
{
  unsigned char value;
  if (!_read_9862 (which_codec, regno, &value))
    return READ_FAILED;
  return value;
}

bool
usrp_basic::_write_spi (int optional_header, int enables, int format, std::string buf)
{
  return usrp_spi_write (d_udh, optional_header, enables, format,
			 buf.data(), buf.size());
}

std::string
usrp_basic::_read_spi (int optional_header, int enables, int format, int len)
{
  if (len <= 0)
    return "";
  
  char buf[len];

  if (!usrp_spi_read (d_udh, optional_header, enables, format, buf, len))
    return "";

  return std::string (buf, len);
}


bool
usrp_basic::_set_led (int which, bool on)
{
  return usrp_set_led (d_udh, which, on);
}

////////////////////////////////////////////////////////////////
//
//			   usrp_basic_rx
//
////////////////////////////////////////////////////////////////

static unsigned char rx_init_regs[] = {
  REG_RX_PWR_DN,	0,
  REG_RX_A,		0,	// minimum gain = 0x00 (max gain = 0x14)
  REG_RX_B,		0,	// minimum gain = 0x00 (max gain = 0x14)
  REG_RX_MISC,		(RX_MISC_HS_DUTY_CYCLE | RX_MISC_CLK_DUTY),
  REG_RX_IF,		(RX_IF_USE_CLKOUT1
			 | RX_IF_2S_COMP),
  REG_RX_DIGITAL,	(RX_DIGITAL_2_CHAN)
};


usrp_basic_rx::usrp_basic_rx (int which_board, int fusb_block_size, int fusb_nblocks,
			      const std::string fpga_filename,
			      const std::string firmware_filename
			      )
  : usrp_basic (which_board, open_rx_interface, fpga_filename, firmware_filename),
    d_devhandle (0), d_ephandle (0),
    d_bytes_seen (0), d_first_read (true),
    d_rx_enable (false)
{
  // initialize rx specific registers

  if (!usrp_9862_write_many_all (d_udh, rx_init_regs, sizeof (rx_init_regs))){
    fprintf (stderr, "usrp_basic_rx: failed to init AD9862 RX regs\n");
    throw std::runtime_error ("usrp_basic_rx/init_9862");
  }

  if (0){
    // FIXME power down 2nd codec rx path
    usrp_9862_write (d_udh, 1, REG_RX_PWR_DN, 0x1);	// power down everything
  }

  // Reset the rx path and leave it disabled.
  set_rx_enable (false);
  usrp_set_fpga_rx_reset (d_udh, true);
  usrp_set_fpga_rx_reset (d_udh, false);

  set_fpga_rx_sample_rate_divisor (2);	// usually correct

  set_dc_offset_cl_enable(0xf, 0xf);	// enable DC offset removal control loops

  probe_rx_slots (false);

  // check fusb buffering parameters

  if (fusb_block_size < 0 || fusb_block_size > FUSB_BLOCK_SIZE)
    throw std::out_of_range ("usrp_basic_rx: invalid fusb_block_size");

  if (fusb_nblocks < 0)
    throw std::out_of_range ("usrp_basic_rx: invalid fusb_nblocks");
  
  if (fusb_block_size == 0)
    fusb_block_size = fusb_sysconfig::default_block_size();

  if (fusb_nblocks == 0)
    fusb_nblocks = std::max (1, FUSB_BUFFER_SIZE / fusb_block_size);

  d_devhandle = fusb_sysconfig::make_devhandle (d_udh);
  d_ephandle = d_devhandle->make_ephandle (USRP_RX_ENDPOINT, true,
					   fusb_block_size, fusb_nblocks);

  _write_fpga_reg(FR_ATR_MASK_1, 0);	// zero Rx side Auto Transmit/Receive regs
  _write_fpga_reg(FR_ATR_TXVAL_1, 0);
  _write_fpga_reg(FR_ATR_RXVAL_1, 0);
  _write_fpga_reg(FR_ATR_MASK_3, 0);
  _write_fpga_reg(FR_ATR_TXVAL_3, 0);
  _write_fpga_reg(FR_ATR_RXVAL_3, 0);
}

static unsigned char rx_fini_regs[] = {
  REG_RX_PWR_DN,	0x1				// power down everything
};

usrp_basic_rx::~usrp_basic_rx ()
{
  if (!set_rx_enable (false)){
    fprintf (stderr, "usrp_basic_rx: set_fpga_rx_enable failed\n");
    usb_strerror ();
  }

  d_ephandle->stop ();
  delete d_ephandle;
  delete d_devhandle;

  if (!usrp_9862_write_many_all (d_udh, rx_fini_regs, sizeof (rx_fini_regs))){
    fprintf (stderr, "usrp_basic_rx: failed to fini AD9862 RX regs\n");
  }
}


bool
usrp_basic_rx::start ()
{
  if (!usrp_basic::start ())	// invoke parent's method
    return false;

  // fire off reads before asserting rx_enable

  if (!d_ephandle->start ()){
    fprintf (stderr, "usrp_basic_rx: failed to start end point streaming");
    usb_strerror ();
    return false;
  }

  if (!set_rx_enable (true)){
    fprintf (stderr, "usrp_basic_rx: set_rx_enable failed\n");
    usb_strerror ();
    return false;
  }
  
  return true;
}

bool
usrp_basic_rx::stop ()
{
  bool ok = usrp_basic::stop();

  if (!set_rx_enable(false)){
    fprintf (stderr, "usrp_basic_rx: set_rx_enable(false) failed\n");
    usb_strerror ();
    ok = false;
  }

  if (!d_ephandle->stop()){
    fprintf (stderr, "usrp_basic_rx: failed to stop end point streaming");
    usb_strerror ();
    ok = false;
  }

  return ok;
}

usrp_basic_rx *
usrp_basic_rx::make (int which_board, int fusb_block_size, int fusb_nblocks,
		     const std::string fpga_filename,
		     const std::string firmware_filename)
{
  usrp_basic_rx *u = 0;
  
  try {
    u = new usrp_basic_rx (which_board, fusb_block_size, fusb_nblocks,
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
usrp_basic_rx::set_fpga_rx_sample_rate_divisor (unsigned int div)
{
  return _write_fpga_reg (FR_RX_SAMPLE_RATE_DIV, div - 1);
}


/*
 * \brief read data from the D/A's via the FPGA.
 * \p len must be a multiple of 512 bytes.
 *
 * \returns the number of bytes read, or -1 on error.
 *
 * If overrun is non-NULL it will be set true iff an RX overrun is detected.
 */
int
usrp_basic_rx::read (void *buf, int len, bool *overrun)
{
  int	r;
  
  if (overrun)
    *overrun = false;
  
  if (len < 0 || (len % 512) != 0){
    fprintf (stderr, "usrp_basic_rx::read: invalid length = %d\n", len);
    return -1;
  }

  r = d_ephandle->read (buf, len);
  if (r > 0)
    d_bytes_seen += r;

  /*
   * In many cases, the FPGA reports an rx overrun right after we
   * enable the Rx path.  If this is our first read, check for the
   * overrun to clear the condition, then ignore the result.
   */
  if (0 && d_first_read){	// FIXME
    d_first_read = false;
    bool bogus_overrun;
    usrp_check_rx_overrun (d_udh, &bogus_overrun);
  }

  if (overrun != 0 && d_bytes_seen >= d_bytes_per_poll){
    d_bytes_seen = 0;
    if (!usrp_check_rx_overrun (d_udh, overrun)){
      fprintf (stderr, "usrp_basic_rx: usrp_check_rx_overrun failed\n");
      usb_strerror ();
    }
  }
    
  return r;
}

bool
usrp_basic_rx::set_rx_enable (bool on)
{
  d_rx_enable = on;
  return usrp_set_fpga_rx_enable (d_udh, on);
}

// conditional disable, return prev state
bool
usrp_basic_rx::disable_rx ()
{
  bool enabled = rx_enable ();
  if (enabled)
    set_rx_enable (false);
  return enabled;
}

// conditional set
void
usrp_basic_rx::restore_rx (bool on)
{
  if (on != rx_enable ())
    set_rx_enable (on);
}

bool
usrp_basic_rx::set_pga (int which, double gain)
{
  if (which < 0 || which > 3)
    return false;

  gain = std::max (pga_min (), gain);
  gain = std::min (pga_max (), gain);

  int codec = which >> 1;
  int reg = (which & 1) == 0 ? REG_RX_A : REG_RX_B;

  // read current value to get input buffer bypass flag.
  unsigned char cur_rx;
  if (!_read_9862 (codec, reg, &cur_rx))
    return false;

  int int_gain = (int) rint ((gain - pga_min ()) / pga_db_per_step());

  cur_rx = (cur_rx & RX_X_BYPASS_INPUT_BUFFER) | (int_gain & 0x7f);
  return _write_9862 (codec, reg, cur_rx);
}

double
usrp_basic_rx::pga (int which) const
{
  if (which < 0 || which > 3)
    return READ_FAILED;

  int codec = which >> 1;
  int reg = (which & 1) == 0 ? REG_RX_A : REG_RX_B;
  unsigned char v;
  bool ok = _read_9862 (codec, reg, &v);
  if (!ok)
    return READ_FAILED;

  return (pga_db_per_step() * (v & 0x1f)) + pga_min();
}

static int
slot_id_to_oe_reg (int slot_id)
{
  static int reg[4]  = { FR_OE_0, FR_OE_1, FR_OE_2, FR_OE_3 };
  assert (0 <= slot_id && slot_id < 4);
  return reg[slot_id];
}

static int
slot_id_to_io_reg (int slot_id)
{
  static int reg[4]  = { FR_IO_0, FR_IO_1, FR_IO_2, FR_IO_3 };
  assert (0 <= slot_id && slot_id < 4);
  return reg[slot_id];
}

void
usrp_basic_rx::probe_rx_slots (bool verbose)
{
  struct usrp_dboard_eeprom	eeprom;
  static int slot_id_map[2] = { SLOT_RX_A, SLOT_RX_B };
  static const char *slot_name[2] = { "RX d'board A", "RX d'board B" };

  for (int i = 0; i < 2; i++){
    int slot_id = slot_id_map [i];
    const char *msg = 0;
    usrp_dbeeprom_status_t s = usrp_read_dboard_eeprom (d_udh, slot_id, &eeprom);

    switch (s){
    case UDBE_OK:
      d_dbid[i] = eeprom.id;
      msg = usrp_dbid_to_string (eeprom.id).c_str ();
      set_adc_offset (2*i+0, eeprom.offset[0]);
      set_adc_offset (2*i+1, eeprom.offset[1]);
      _write_fpga_reg (slot_id_to_oe_reg(slot_id), (0xffff << 16) | eeprom.oe);
      _write_fpga_reg (slot_id_to_io_reg(slot_id), (0xffff << 16) | 0x0000);
      break;
      
    case UDBE_NO_EEPROM:
      d_dbid[i] = -1;
      msg = "<none>";
      _write_fpga_reg (slot_id_to_oe_reg(slot_id), (0xffff << 16) | 0x0000);
      _write_fpga_reg (slot_id_to_io_reg(slot_id), (0xffff << 16) | 0x0000);
      break;
      
    case UDBE_INVALID_EEPROM:
      d_dbid[i] = -2;
      msg = "Invalid EEPROM contents";
      _write_fpga_reg (slot_id_to_oe_reg(slot_id), (0xffff << 16) | 0x0000);
      _write_fpga_reg (slot_id_to_io_reg(slot_id), (0xffff << 16) | 0x0000);
      break;
      
    case UDBE_BAD_SLOT:
    default:
      assert (0);
    }

    if (verbose){
      fflush (stdout);
      fprintf (stderr, "%s: %s\n", slot_name[i], msg);
    }
  }
}

bool
usrp_basic_rx::_write_oe (int which_dboard, int value, int mask)
{
  if (! (0 <= which_dboard && which_dboard <= 1))
    return false;

  return _write_fpga_reg (slot_id_to_oe_reg (dboard_to_slot (which_dboard)),
			  (mask << 16) | (value & 0xffff));
}

bool
usrp_basic_rx::write_io (int which_dboard, int value, int mask)
{
  if (! (0 <= which_dboard && which_dboard <= 1))
    return false;

  return _write_fpga_reg (slot_id_to_io_reg (dboard_to_slot (which_dboard)),
			  (mask << 16) | (value & 0xffff));
}

bool
usrp_basic_rx::read_io (int which_dboard, int *value)
{
  if (! (0 <= which_dboard && which_dboard <= 1))
    return false;

  int t;
  int reg = which_dboard + 1;	// FIXME, *very* magic number (fix in serial_io.v)
  bool ok = _read_fpga_reg (reg, &t);
  if (!ok)
    return false;

  *value = (t >> 16) & 0xffff;	// FIXME, more magic
  return true;
}

int
usrp_basic_rx::read_io (int which_dboard)
{
  int	value;
  if (!read_io (which_dboard, &value))
    return READ_FAILED;
  return value;
}

bool
usrp_basic_rx::write_aux_dac (int which_dboard, int which_dac, int value)
{
  return usrp_basic::write_aux_dac (dboard_to_slot (which_dboard),
				    which_dac, value);
}

bool
usrp_basic_rx::read_aux_adc (int which_dboard, int which_adc, int *value)
{
  return usrp_basic::read_aux_adc (dboard_to_slot (which_dboard),
				   which_adc, value);
}

int
usrp_basic_rx::read_aux_adc (int which_dboard, int which_adc)
{
  return usrp_basic::read_aux_adc (dboard_to_slot (which_dboard), which_adc);
}

int
usrp_basic_rx::block_size () const { return d_ephandle->block_size(); }

bool
usrp_basic_rx::set_dc_offset_cl_enable(int bits, int mask)
{
  return _write_fpga_reg(FR_DC_OFFSET_CL_EN, 
			 (d_fpga_shadows[FR_DC_OFFSET_CL_EN] & ~mask) | (bits & mask));
}

////////////////////////////////////////////////////////////////
//
//			   usrp_basic_tx
//
////////////////////////////////////////////////////////////////


//
// DAC input rate 64 MHz interleaved for a total input rate of 128 MHz
// DAC input is latched on rising edge of CLKOUT2
// NCO is disabled
// interpolate 2x
// coarse modulator disabled
//

static unsigned char tx_init_regs[] = {
  REG_TX_PWR_DN,	0,
  REG_TX_A_OFFSET_LO,	0,
  REG_TX_A_OFFSET_HI,	0,
  REG_TX_B_OFFSET_LO,	0,
  REG_TX_B_OFFSET_HI,	0,
  REG_TX_A_GAIN,	(TX_X_GAIN_COARSE_FULL | 0),
  REG_TX_B_GAIN,	(TX_X_GAIN_COARSE_FULL | 0),
  REG_TX_PGA,		0xff,			// maximum gain (0 dB)
  REG_TX_MISC,		0,
  REG_TX_IF,		(TX_IF_USE_CLKOUT1
			 | TX_IF_I_FIRST
			 | TX_IF_INV_TX_SYNC
			 | TX_IF_2S_COMP
			 | TX_IF_INTERLEAVED),
  REG_TX_DIGITAL,	(TX_DIGITAL_2_DATA_PATHS
			 | TX_DIGITAL_INTERPOLATE_4X),
  REG_TX_MODULATOR,	(TX_MODULATOR_DISABLE_NCO
			 | TX_MODULATOR_COARSE_MODULATION_NONE),
  REG_TX_NCO_FTW_7_0,	0,
  REG_TX_NCO_FTW_15_8,	0,
  REG_TX_NCO_FTW_23_16,	0
};

usrp_basic_tx::usrp_basic_tx (int which_board, int fusb_block_size, int fusb_nblocks,
			      const std::string fpga_filename,
			      const std::string firmware_filename)
  : usrp_basic (which_board, open_tx_interface, fpga_filename, firmware_filename),
    d_devhandle (0), d_ephandle (0),
    d_bytes_seen (0), d_first_write (true),
    d_tx_enable (false)
{
  if (!usrp_9862_write_many_all (d_udh, tx_init_regs, sizeof (tx_init_regs))){
    fprintf (stderr, "usrp_basic_tx: failed to init AD9862 TX regs\n");
    throw std::runtime_error ("usrp_basic_tx/init_9862");
  }

  if (0){
    // FIXME power down 2nd codec tx path
    usrp_9862_write (d_udh, 1, REG_TX_PWR_DN,
		     (TX_PWR_DN_TX_DIGITAL
		      | TX_PWR_DN_TX_ANALOG_BOTH));
  }

  // Reset the tx path and leave it disabled.
  set_tx_enable (false);
  usrp_set_fpga_tx_reset (d_udh, true);
  usrp_set_fpga_tx_reset (d_udh, false);

  set_fpga_tx_sample_rate_divisor (4);	// we're using interp x4

  probe_tx_slots (false);

  // check fusb buffering parameters

  if (fusb_block_size < 0 || fusb_block_size > FUSB_BLOCK_SIZE)
    throw std::out_of_range ("usrp_basic_rx: invalid fusb_block_size");

  if (fusb_nblocks < 0)
    throw std::out_of_range ("usrp_basic_rx: invalid fusb_nblocks");
  
  if (fusb_block_size == 0)
    fusb_block_size = FUSB_BLOCK_SIZE;

  if (fusb_nblocks == 0)
    fusb_nblocks = std::max (1, FUSB_BUFFER_SIZE / fusb_block_size);

  d_devhandle = fusb_sysconfig::make_devhandle (d_udh);
  d_ephandle = d_devhandle->make_ephandle (USRP_TX_ENDPOINT, false,
					   fusb_block_size, fusb_nblocks);

  _write_fpga_reg(FR_ATR_MASK_0, 0); // zero Tx side Auto Transmit/Receive regs
  _write_fpga_reg(FR_ATR_TXVAL_0, 0);
  _write_fpga_reg(FR_ATR_RXVAL_0, 0);
  _write_fpga_reg(FR_ATR_MASK_2, 0);
  _write_fpga_reg(FR_ATR_TXVAL_2, 0);
  _write_fpga_reg(FR_ATR_RXVAL_2, 0);
}


static unsigned char tx_fini_regs[] = {
  REG_TX_PWR_DN,	(TX_PWR_DN_TX_DIGITAL
			 | TX_PWR_DN_TX_ANALOG_BOTH),
  REG_TX_MODULATOR,	(TX_MODULATOR_DISABLE_NCO
			 | TX_MODULATOR_COARSE_MODULATION_NONE)
};

usrp_basic_tx::~usrp_basic_tx ()
{
  d_ephandle->stop ();
  delete d_ephandle;
  delete d_devhandle;

  if (!usrp_9862_write_many_all (d_udh, tx_fini_regs, sizeof (tx_fini_regs))){
    fprintf (stderr, "usrp_basic_tx: failed to fini AD9862 TX regs\n");
  }
}

bool
usrp_basic_tx::start ()
{
  if (!usrp_basic::start ())
    return false;

  if (!set_tx_enable (true)){
    fprintf (stderr, "usrp_basic_tx: set_tx_enable failed\n");
    usb_strerror ();
    return false;
  }
  
  if (!d_ephandle->start ()){
    fprintf (stderr, "usrp_basic_tx: failed to start end point streaming");
    usb_strerror ();
    return false;
  }

  return true;
}

bool
usrp_basic_tx::stop ()
{
  bool ok = usrp_basic::stop ();

  if (!d_ephandle->stop ()){
    fprintf (stderr, "usrp_basic_tx: failed to stop end point streaming");
    usb_strerror ();
    ok = false;
  }

  if (!set_tx_enable (false)){
    fprintf (stderr, "usrp_basic_tx: set_tx_enable(false) failed\n");
    usb_strerror ();
    ok = false;
  }

  return ok;
}

usrp_basic_tx *
usrp_basic_tx::make (int which_board, int fusb_block_size, int fusb_nblocks,
		     const std::string fpga_filename,
		     const std::string firmware_filename)
{
  usrp_basic_tx *u = 0;
  
  try {
    u = new usrp_basic_tx (which_board, fusb_block_size, fusb_nblocks,
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
usrp_basic_tx::set_fpga_tx_sample_rate_divisor (unsigned int div)
{
  return _write_fpga_reg (FR_TX_SAMPLE_RATE_DIV, div - 1);
}

/*!
 * \brief Write data to the A/D's via the FPGA.
 *
 * \p len must be a multiple of 512 bytes.
 * \returns number of bytes written or -1 on error.
 *
 * if \p underrun is non-NULL, it will be set to true iff
 * a transmit underrun condition is detected.
 */
int
usrp_basic_tx::write (const void *buf, int len, bool *underrun)
{
  int	r;
  
  if (underrun)
    *underrun = false;
  
  if (len < 0 || (len % 512) != 0){
    fprintf (stderr, "usrp_basic_tx::write: invalid length = %d\n", len);
    return -1;
  }

  r = d_ephandle->write (buf, len);
  if (r > 0)
    d_bytes_seen += r;
    
  /*
   * In many cases, the FPGA reports an tx underrun right after we
   * enable the Tx path.  If this is our first write, check for the
   * underrun to clear the condition, then ignore the result.
   */
  if (d_first_write && d_bytes_seen >= 4 * FUSB_BLOCK_SIZE){
    d_first_write = false;
    bool bogus_underrun;
    usrp_check_tx_underrun (d_udh, &bogus_underrun);
  }

  if (underrun != 0 && d_bytes_seen >= d_bytes_per_poll){
    d_bytes_seen = 0;
    if (!usrp_check_tx_underrun (d_udh, underrun)){
      fprintf (stderr, "usrp_basic_tx: usrp_check_tx_underrun failed\n");
      usb_strerror ();
    }
  }

  return r;
}

void
usrp_basic_tx::wait_for_completion ()
{
  d_ephandle->wait_for_completion ();
}

bool
usrp_basic_tx::set_tx_enable (bool on)
{
  d_tx_enable = on;
  // fprintf (stderr, "set_tx_enable %d\n", on);
  return usrp_set_fpga_tx_enable (d_udh, on);
}

// conditional disable, return prev state
bool
usrp_basic_tx::disable_tx ()
{
  bool enabled = tx_enable ();
  if (enabled)
    set_tx_enable (false);
  return enabled;
}

// conditional set
void
usrp_basic_tx::restore_tx (bool on)
{
  if (on != tx_enable ())
    set_tx_enable (on);
}

bool
usrp_basic_tx::set_pga (int which, double gain)
{
  if (which < 0 || which > 3)
    return false;

  gain = std::max (pga_min (), gain);
  gain = std::min (pga_max (), gain);

  int codec = which >> 1;	// 0 and 1 are same, as are 2 and 3

  int int_gain = (int) rint ((gain - pga_min ()) / pga_db_per_step());

  return _write_9862 (codec, REG_TX_PGA, int_gain);
}

double
usrp_basic_tx::pga (int which) const
{
  if (which < 0 || which > 3)
    return READ_FAILED;

  int codec = which >> 1;
  unsigned char v;
  bool ok = _read_9862 (codec, REG_TX_PGA, &v);
  if (!ok)
    return READ_FAILED;

  return (pga_db_per_step() * v) + pga_min();
}

void
usrp_basic_tx::probe_tx_slots (bool verbose)
{
  struct usrp_dboard_eeprom	eeprom;
  static int slot_id_map[2] = { SLOT_TX_A, SLOT_TX_B };
  static const char *slot_name[2] = { "TX d'board A", "TX d'board B" };

  for (int i = 0; i < 2; i++){
    int slot_id = slot_id_map [i];
    const char *msg = 0;
    usrp_dbeeprom_status_t s = usrp_read_dboard_eeprom (d_udh, slot_id, &eeprom);

    switch (s){
    case UDBE_OK:
      d_dbid[i] = eeprom.id;
      msg = usrp_dbid_to_string (eeprom.id).c_str ();
      // FIXME, figure out interpretation of dc offset for TX d'boards
      // offset = (eeprom.offset[1] << 16) | (eeprom.offset[0] & 0xffff);
      _write_fpga_reg (slot_id_to_oe_reg(slot_id), (0xffff << 16) | eeprom.oe);
      _write_fpga_reg (slot_id_to_io_reg(slot_id), (0xffff << 16) | 0x0000);
      break;
      
    case UDBE_NO_EEPROM:
      d_dbid[i] = -1;
      msg = "<none>";
      _write_fpga_reg (slot_id_to_oe_reg(slot_id), (0xffff << 16) | 0x0000);
      _write_fpga_reg (slot_id_to_io_reg(slot_id), (0xffff << 16) | 0x0000);
      break;
      
    case UDBE_INVALID_EEPROM:
      d_dbid[i] = -2;
      msg = "Invalid EEPROM contents";
      _write_fpga_reg (slot_id_to_oe_reg(slot_id), (0xffff << 16) | 0x0000);
      _write_fpga_reg (slot_id_to_io_reg(slot_id), (0xffff << 16) | 0x0000);
      break;
      
    case UDBE_BAD_SLOT:
    default:
      assert (0);
    }

    if (verbose){
      fflush (stdout);
      fprintf (stderr, "%s: %s\n", slot_name[i], msg);
    }
  }
}

bool
usrp_basic_tx::_write_oe (int which_dboard, int value, int mask)
{
  if (! (0 <= which_dboard && which_dboard <= 1))
    return false;

  return _write_fpga_reg (slot_id_to_oe_reg (dboard_to_slot (which_dboard)),
			  (mask << 16) | (value & 0xffff));
}

bool
usrp_basic_tx::write_io (int which_dboard, int value, int mask)
{
  if (! (0 <= which_dboard && which_dboard <= 1))
    return false;

  return _write_fpga_reg (slot_id_to_io_reg (dboard_to_slot (which_dboard)),
			  (mask << 16) | (value & 0xffff));
}

bool
usrp_basic_tx::read_io (int which_dboard, int *value)
{
  if (! (0 <= which_dboard && which_dboard <= 1))
    return false;

  int t;
  int reg = which_dboard + 1;	// FIXME, *very* magic number (fix in serial_io.v)
  bool ok = _read_fpga_reg (reg, &t);
  if (!ok)
    return false;

  *value = t & 0xffff;		// FIXME, more magic
  return true;
}

int
usrp_basic_tx::read_io (int which_dboard)
{
  int	value;
  if (!read_io (which_dboard, &value))
    return READ_FAILED;
  return value;
}

bool
usrp_basic_tx::write_aux_dac (int which_dboard, int which_dac, int value)
{
  return usrp_basic::write_aux_dac (dboard_to_slot (which_dboard),
				    which_dac, value);
}

bool
usrp_basic_tx::read_aux_adc (int which_dboard, int which_adc, int *value)
{
  return usrp_basic::read_aux_adc (dboard_to_slot (which_dboard),
				   which_adc, value);
}

int
usrp_basic_tx::read_aux_adc (int which_dboard, int which_adc)
{
  return usrp_basic::read_aux_adc (dboard_to_slot (which_dboard), which_adc);
}

int
usrp_basic_tx::block_size () const { return d_ephandle->block_size(); }


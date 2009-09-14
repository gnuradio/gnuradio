/* -*- c++ -*- */
/*
 * Copyright 2003,2004,2008,2009 Free Software Foundation, Inc.
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

#include <usrp/usrp_basic.h>
#include "usrp/usrp_prims.h"
#include "usrp_interfaces.h"
#include "fpga_regs_common.h"
#include "fpga_regs_standard.h"
#include "fusb.h"
#include "db_boards.h"
#include <usb.h>
#include <stdexcept>
#include <assert.h>
#include <math.h>
#include <ad9862.h>
#include <string.h>
#include <cstdio>

using namespace ad9862;

#define NELEM(x) (sizeof (x) / sizeof (x[0]))


static const double POLLING_INTERVAL = 0.1;	// seconds


//////////////////////////////////////////////////////////////////
//
//			usrp_basic
//
////////////////////////////////////////////////////////////////


// Given:
//   CLKIN = 64 MHz
//   CLKSEL pin = high 
//
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
    d_verbose (false), d_fpga_master_clock_freq(64000000), d_db(2)
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
  // shutdown_daughterboards();		// call from ~usrp_basic_{tx,rx}

  d_db.resize(0); // forget db shared ptrs

  if (d_udh)
    usb_close (d_udh);
}


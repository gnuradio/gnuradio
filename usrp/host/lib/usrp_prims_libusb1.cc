/* -*- c++ -*- */
/*
 * Copyright 2003,2004,2006,2009 Free Software Foundation, Inc.
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

#include "usrp/usrp_prims.h"
#include "usrp_commands.h"
#include "usrp_ids.h"
#include "usrp_i2c_addr.h"
#include "fpga_regs_common.h"
#include "fpga_regs_standard.h"
#include <libusb-1.0/libusb.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>		// FIXME should check with autoconf (nanosleep)
#include <algorithm>
#include <ad9862.h>
#include <assert.h>

extern "C" {
#include "md5.h"
};

#define VERBOSE 0

using namespace ad9862;

static const int FIRMWARE_HASH_SLOT	= 0;
static const int FPGA_HASH_SLOT 	= 1;

static const int hash_slot_addr[2] = {
  USRP_HASH_SLOT_0_ADDR,
  USRP_HASH_SLOT_1_ADDR
};

static const char *default_firmware_filename = "std.ihx";
static const char *default_fpga_filename     = "std_2rxhb_2tx.rbf";

#include "std_paths.h"
#include <stdio.h>

void
usrp_one_time_init (libusb_context **ctx)
{
  int ret;

  if ((ret = libusb_init (ctx)) < 0)
    fprintf (stderr, "usrp: libusb_init failed %i\n", ret);
}

void
usrp_rescan ()
{
  // nop
}

// ----------------------------------------------------------------

/*
 * q must be a real USRP, not an FX2.  Return its hardware rev number.
 */
int
usrp_hw_rev (struct libusb_device *q)
{
  struct libusb_device_descriptor desc;
  if (libusb_get_device_descriptor(q, &desc) < 0)
    fprintf (stderr, "usrp: libusb_get_device_descriptor failed\n");

  return desc.bcdDevice & 0x00FF;
}

/*
 * q must be a real USRP, not an FX2.  Return true if it's configured.
 */
bool
_usrp_configured_p (struct libusb_device *q)
{
  struct libusb_device_descriptor desc;
  if (libusb_get_device_descriptor(q, &desc) < 0)
    fprintf (stderr, "usrp: libusb_get_device_descriptor failed\n");

  return (desc.bcdDevice & 0xFF00) != 0;
}

bool
usrp_usrp_p (struct libusb_device *q)
{
  struct libusb_device_descriptor desc;
  if (libusb_get_device_descriptor(q, &desc) < 0)
    fprintf (stderr, "usrp: libusb_get_device_descriptor failed\n");

  return (desc.idVendor == USB_VID_FSF
	  && desc.idProduct == USB_PID_FSF_USRP);
}

bool
usrp_fx2_p (struct libusb_device *q)
{
  struct libusb_device_descriptor desc;
  if (libusb_get_device_descriptor(q, &desc) < 0)
    fprintf (stderr, "usrp: libusb_get_device_descriptor failed\n");

  return (desc.idVendor == USB_VID_CYPRESS
	  && desc.idProduct == USB_PID_CYPRESS_FX2);
}


// ----------------------------------------------------------------

struct libusb_device *
usrp_find_device (int nth, bool fx2_ok_p, libusb_context *ctx)
{
  libusb_device **list;

  struct libusb_device *q;
  int	 n_found = 0;

  // Make sure not operating on default context. There are cases where operating
  // with a single global (NULL) context may be preferable, so this check can be
  // skipped if you know what you're doing.
  assert (ctx != NULL);

  size_t cnt = libusb_get_device_list(ctx, &list);
  size_t i = 0;

  if (cnt < 0)
    fprintf(stderr, "usrp: libusb_get_device_list failed %d\n", cnt);

  for (i = 0; i < cnt; i++) {
    q = list[i];
    if (usrp_usrp_p (q) || (fx2_ok_p && usrp_fx2_p (q))) {
	if (n_found == nth)	// return this one
	  return q;
	n_found++;		// keep looking
    }
  }

  // The list needs to be freed. Right now just release it if nothing is found.
  libusb_free_device_list(list, 1);

  return 0;	// not found
}

struct libusb_device_handle *
usrp_open_interface (libusb_device *dev, int interface, int altinterface)
{
  libusb_device_handle *udh;
  int ret;

  if (libusb_open (dev, &udh) < 0)
    return 0;

  if (dev != libusb_get_device (udh)){
    fprintf (stderr, "%s:%d: internal error!\n", __FILE__, __LINE__);
    abort ();
  }

  if ((ret = libusb_claim_interface (udh, interface)) < 0) {
    fprintf (stderr, "%s:usb_claim_interface: failed interface %d\n", __FUNCTION__,interface);
    fprintf (stderr, "%d\n", ret);
    libusb_close (udh);
    return 0;
  }

  if ((ret = libusb_set_interface_alt_setting (udh, interface,
                                                   altinterface)) < 0) {
    fprintf (stderr, "%s:usb_set_alt_interface: failed\n", __FUNCTION__);
    fprintf (stderr, "%d\n", ret);
    libusb_release_interface (udh, interface);
    libusb_close (udh);
    return 0;
  }

  return udh;
}

bool
usrp_close_interface (libusb_device_handle *udh)
{
  // returns void
  libusb_close(udh);
  return 0;
}

// ----------------------------------------------------------------
// write internal ram using Cypress vendor extension

bool
write_internal_ram (struct libusb_device_handle *udh, unsigned char *buf,
                    int start_addr, size_t len)
{
  int addr;
  int n;
  int a;
  int quanta = MAX_EP0_PKTSIZE;

  for (addr = start_addr; addr < start_addr + (int) len; addr += quanta){
    n = len + start_addr - addr;
    if (n > quanta)
      n = quanta;

    a = libusb_control_transfer (udh, 0x40, 0xA0,
                         addr, 0, (unsigned char *)(buf + (addr - start_addr)), n, 1000);

    if (a < 0){
      fprintf(stderr,"write_internal_ram failed: %u\n", a);
      return false;
    }
  }
  return true;
}

// ----------------------------------------------------------------
// write vendor extension command to USRP

int
write_cmd (struct libusb_device_handle *udh,
           int request, int value, int index,
           unsigned char *bytes, int len)
{
  int   requesttype = (request & 0x80) ? VRT_VENDOR_IN : VRT_VENDOR_OUT;

  int r = libusb_control_transfer(udh, requesttype, request, value, index,
                                  (unsigned char *) bytes, len, 1000);

  if (r < 0){
    // we get EPIPE if the firmware stalls the endpoint.
    if (r != LIBUSB_ERROR_PIPE) {
      fprintf (stderr, "libusb_control_transfer failed: %i\n", r);
    }
  }

  return r;
}

bool
usrp_set_hash (struct libusb_device_handle *udh, int which,
               const unsigned char hash[USRP_HASH_SIZE])
{
  which &= 1;

  // we use the Cypress firmware down load command to jam it in.
  int r = libusb_control_transfer (udh, 0x40, 0xa0, hash_slot_addr[which], 0,
                           (unsigned char *) hash, USRP_HASH_SIZE, 1000);
  return r == USRP_HASH_SIZE;
}
  
bool
usrp_get_hash (struct libusb_device_handle *udh, int which,
               unsigned char hash[USRP_HASH_SIZE])
{
  which &= 1;

  // we use the Cypress firmware upload command to fetch it.
  int r = libusb_control_transfer (udh, 0xc0, 0xa0, hash_slot_addr[which], 0,
                           (unsigned char *) hash, USRP_HASH_SIZE, 1000);
  return r == USRP_HASH_SIZE;
}

bool
usrp_write_fpga_reg (struct libusb_device_handle *udh, int reg, int value)
{
  switch (usrp_hw_rev (libusb_get_device (udh))){
  case 0:                       // not supported ;)
    abort();

  default:
    return usrp1_fpga_write (udh, reg, value);
  }
}

bool
usrp_read_fpga_reg (struct libusb_device_handle *udh, int reg, int *value)
{
  switch (usrp_hw_rev (libusb_get_device (udh))){
  case 0:               // not supported ;)
    abort();

  default:
    return usrp1_fpga_read (udh, reg, value);
  }
}



void
power_down_9862s (struct libusb_device_handle *udh)
{
  static const unsigned char regs[] = {
    REG_RX_PWR_DN,	0x01,			// everything
    REG_TX_PWR_DN,	0x0f,			// pwr dn digital and analog_both
    REG_TX_MODULATOR,	0x00			// coarse & fine modulators disabled
  };

  switch (usrp_hw_rev (libusb_get_device (udh))){
  case 0:
    break;

  default:
    usrp_9862_write_many_all (udh, regs, sizeof (regs));
    break;
  }
}


// ----------------------------------------------------------------

std::string
usrp_serial_number(struct libusb_device_handle *udh)
{
  struct libusb_device_descriptor desc;
  if (libusb_get_device_descriptor(libusb_get_device(udh), &desc) < 0)
    fprintf (stderr, "usrp: libusb_get_device_descriptor failed\n");

  unsigned char iserial = desc.iSerialNumber;
  if (iserial == 0)
    return "";

  unsigned char buf[1024];
  if (libusb_get_string_descriptor_ascii(udh, iserial, buf, sizeof(buf)) < 0)
    return "";

  return (char*) buf;
}

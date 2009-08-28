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

bool our_nanosleep (const struct timespec *delay);
const char *get_proto_filename (const std::string user_filename,
                                const char *env_var,
                                const char *def);
char *find_file (const char *filename, int hw_rev);
void load_status_msg (usrp_load_status_t s, const char *type,
                      const char *filename);
bool usrp1_fpga_write (libusb_device_handle *udh, int regno, int value);
bool usrp1_fpga_read (libusb_device_handle *udh, int regno, int *value);
bool usrp_set_switch (libusb_device_handle *udh, int cmd_byte, bool on);

bool usrp_set_fpga_reset (struct usb_dev_handle *udh, bool on);
bool usrp_set_fpga_tx_enable (struct usb_dev_handle *udh, bool on);
bool usrp_set_fpga_rx_enable (struct usb_dev_handle *udh, bool on);
bool usrp_set_fpga_tx_reset (struct usb_dev_handle *udh, bool on);
bool usrp_set_fpga_rx_reset (struct usb_dev_handle *udh, bool on);


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

libusb_context *
usrp_one_time_init (bool new_context)
{

  static bool first = true;
  libusb_context *ctx = NULL;
  int ret;

  // On first call create default context in addition to any new requested
  // context. The default context is probably useless in this form, but keep
  // it for now due to compatibility reasons.

  if (first) {
    first = false;
    if ((ret = libusb_init (NULL)) < 0)
      fprintf (stderr, "usrp: libusb_init failed %i\n", ret);
  }

  if (new_context) {
    if ((ret = libusb_init (&ctx)) < 0)
      fprintf (stderr, "usrp: libusb_init failed %i\n", ret);
  }

  return ctx;
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
static bool
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

//usrp_one_time_init (false);
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

/*
 * The list needs to be freed. Right now just release it if nothing is found.
 */

  libusb_free_device_list(list, 1);

  return 0;	// not found
}

struct libusb_device_handle *
usrp_open_interface (libusb_device *dev, int interface, int altinterface)
{
  struct libusb_device_handle *udh;
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


static libusb_device_handle *
open_nth_cmd_interface (int nth, libusb_context *ctx)
{

  struct libusb_device *udev = usrp_find_device (nth, false, ctx);
  if (udev == 0){
    fprintf (stderr, "usrp: failed to find usrp[%d]\n", nth);
    return 0;
  }

  struct libusb_device_handle *udh;

  udh = usrp_open_cmd_interface (udev);
  if (udh == 0){
    // FIXME this could be because somebody else has it open.
    // We should delay and retry...
    fprintf (stderr, "open_nth_cmd_interface: open_cmd_interface failed\n");
    return 0;
  }

  return udh;
}

usrp_load_status_t
usrp_load_firmware_nth (int nth, const char *filename, bool force, libusb_context *ctx)
{
  struct libusb_device_handle *udh = open_nth_cmd_interface (nth, ctx);
  if (udh == 0)
    return ULS_ERROR;

  usrp_load_status_t s = usrp_load_firmware (udh, filename, force);
  usrp_close_interface (udh);

  switch (s){

  case ULS_ALREADY_LOADED:              // nothing changed...
    return ULS_ALREADY_LOADED;
    break;

  case ULS_OK:
    // we loaded firmware successfully.
 
    // It's highly likely that the board will renumerate (simulate a
    // disconnect/reconnect sequence), invalidating our current
    // handle.

    // FIXME.  Turn this into a loop that rescans until we refind ourselves
   
    struct timespec     t;      // delay for 1 second
    t.tv_sec = 2;
    t.tv_nsec = 0;
    our_nanosleep (&t);

    return ULS_OK;

  default:
  case ULS_ERROR:               // some kind of problem
    return ULS_ERROR;
  }
}

bool
usrp_load_standard_bits (int nth, bool force,
			 const std::string fpga_filename,
			 const std::string firmware_filename,
			 libusb_context *ctx)
{
  usrp_load_status_t 	s;
  const char		*filename;
  const char		*proto_filename;
  int hw_rev;

  assert (ctx != NULL);

  // first, figure out what hardware rev we're dealing with
  {
    struct libusb_device *udev = usrp_find_device (nth, false, ctx);
    if (udev == 0){
      fprintf (stderr, "usrp: failed to find usrp[%d]\n", nth);
      return false;
    }
    hw_rev = usrp_hw_rev (udev);
  }

  // start by loading the firmware

  proto_filename = get_proto_filename(firmware_filename, "USRP_FIRMWARE",
				      default_firmware_filename);
  filename = find_file(proto_filename, hw_rev);
  if (filename == 0){
    fprintf (stderr, "Can't find firmware: %s\n", proto_filename);
    return false;
  }
  s = usrp_load_firmware_nth (nth, filename, force, ctx);
  load_status_msg (s, "firmware", filename);

  if (s == ULS_ERROR)
    return false;

  // if we actually loaded firmware, we must reload fpga ...
  if (s == ULS_OK)
    force = true;

  // now move on to the fpga configuration bitstream

  proto_filename = get_proto_filename(fpga_filename, "USRP_FPGA",
				      default_fpga_filename);
  filename = find_file (proto_filename, hw_rev);
  if (filename == 0){
    fprintf (stderr, "Can't find fpga bitstream: %s\n", proto_filename);
    return false;
  }
  struct libusb_device_handle *udh = open_nth_cmd_interface (nth, ctx);
  if (udh == 0)
    return false;
  
  s = usrp_load_fpga (udh, filename, force);
  usrp_close_interface (udh);
  load_status_msg (s, "fpga bitstream", filename);

  if (s == ULS_ERROR)
    return false;

  return true;
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

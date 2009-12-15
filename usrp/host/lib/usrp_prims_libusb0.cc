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

#include "usrp_primsi.h"
#include "usrp_commands.h"
#include <usb.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ad9862.h>
#include <assert.h>

extern "C" {
#include "md5.h"
};

using namespace ad9862;

/*
 * libusb 0.12 / 1.0 compatibility
 */

struct usb_device *
_get_usb_device (struct usb_dev_handle *udh)
{
  return usb_device (udh);
}

struct usb_device_descriptor
_get_usb_device_descriptor (struct usb_device *q)
{
  return q->descriptor;
}

int
_get_usb_string_descriptor (struct usb_dev_handle *udh, int index,
                           unsigned char* data, int length)
{
  int ret;
  ret =  usb_get_string_simple (udh, index, (char*) data, length);

  if (ret < 0) {
    fprintf (stderr, "usrp: usb_get_string_descriptor failed: %s\n",
             usb_strerror());
  }

  return ret;
}

int
_usb_control_transfer (struct usb_dev_handle *udh, int request_type,
                      int request, int value, int index,
                      unsigned char *data, int length, unsigned int timeout)
{
  int ret;

  ret = usb_control_msg (udh, request_type,request, value, index,
                         (char*) data, length, (int) timeout);
  if (ret < 0) 
    fprintf (stderr, "usrp: usb_control_msg failed: %s\n", usb_strerror());

  return ret;
}


// ----------------------------------------------------------------


void
usrp_one_time_init (libusb_context **ctx)
{
  static bool first = true;

  if (first) {
    first = false;
    usb_init ();			// usb library init
    usb_find_busses ();
    usb_find_devices ();
  }
}

void
usrp_deinit (libusb_context *ctx)
{
  // nop
}

void
usrp_rescan ()
{
  usb_find_busses ();
  usb_find_devices ();
}


// ----------------------------------------------------------------


struct usb_device *
usrp_find_device (int nth, bool fx2_ok_p, libusb_context *ctx)
{
  struct usb_bus *p;
  struct usb_device *q;
  int	 n_found = 0;

  usrp_one_time_init ();

  p = usb_get_busses();
  while (p != NULL){
    q = p->devices;
    while (q != NULL){
      if (usrp_usrp_p (q) || (fx2_ok_p && usrp_fx2_p (q))){
	if (n_found == nth)	// return this one
	  return q;
	n_found++;		// keep looking
      }
      q = q->next;
    }
    p = p->next;
  }
  return 0;	// not found
}

struct usb_dev_handle *
usrp_open_interface (struct usb_device *dev, int interface, int altinterface)
{
  struct usb_dev_handle *udh = usb_open (dev);
  if (udh == 0)
    return 0;

  if (dev != usb_device (udh)){
    fprintf (stderr, "%s:%d: internal error!\n", __FILE__, __LINE__);
    abort ();
  }

#if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
  // There's no get get_configuration function, and with some of the newer kernels
  // setting the configuration, even if to the same value, hoses any other processes
  // that have it open.  Hence we opt to not set it at all (We've only
  // got a single configuration anyway).  This may hose the win32 stuff...

  // Appears to be required for libusb-win32 and Cygwin -- dew 09/20/06
  if (usb_set_configuration (udh, 1) < 0){
    /*
     * Ignore this error.
     *
     * Seems that something changed in drivers/usb/core/devio.c:proc_setconfig such that
     * it returns -EBUSY if _any_ of the interfaces of a device are open.
     * We've only got a single configuration, so setting it doesn't even seem
     * like it should be required.
     */
  }
#endif

  if (usb_claim_interface (udh, interface) < 0){
    fprintf (stderr, "%s:usb_claim_interface: failed interface %d\n", __FUNCTION__,interface);
    fprintf (stderr, "%s\n", usb_strerror());
    usb_close (udh);
    return 0;
  }

  if (usb_set_altinterface (udh, altinterface) < 0){
    fprintf (stderr, "%s:usb_set_alt_interface: failed\n", __FUNCTION__);
    fprintf (stderr, "%s\n", usb_strerror());
    usb_release_interface (udh, interface);
    usb_close (udh);
    return 0;
  }

  return udh;
}

bool
usrp_close_interface (struct usb_dev_handle *udh)
{
  // we're assuming that closing an interface automatically releases it.
  return usb_close (udh) == 0;
}


// ----------------------------------------------------------------
// write vendor extension command to USRP


int
write_cmd (struct usb_dev_handle *udh,
	   int request, int value, int index,
	   unsigned char *bytes, int len)
{
  int	requesttype = (request & 0x80) ? VRT_VENDOR_IN : VRT_VENDOR_OUT;

  int r = usb_control_msg (udh, requesttype, request, value, index,
			   (char *) bytes, len, 1000);
  if (r < 0){
    // we get EPIPE if the firmware stalls the endpoint.
    if (errno != EPIPE) {
      fprintf (stderr, "usb_control_msg failed: %s\n", usb_strerror ());
      fprintf (stderr, "write_cmd failed\n");
    }
  }

  return r;
}


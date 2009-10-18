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
#include <libusb-1.0/libusb.h>
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

static const int LIBUSB1_DEBUG = 0;

/*
 * libusb 0.12 / 1.0 compatibility
 */

static const char *
_get_usb_error_str (int usb_err)
{
  switch (usb_err) {
  case LIBUSB_SUCCESS:
    return "Success (no error)";
  case LIBUSB_ERROR_IO:
    return "Input/output error";
  case LIBUSB_ERROR_INVALID_PARAM:
    return "Invalid parameter";
  case LIBUSB_ERROR_ACCESS:
    return "Access denied (insufficient permissions)";
  case LIBUSB_ERROR_NO_DEVICE:
    return "No such device (it may have been disconnected)";
  case LIBUSB_ERROR_NOT_FOUND:
    return "Entity not found";
  case LIBUSB_ERROR_BUSY:
    return "Resource busy";
  case LIBUSB_ERROR_TIMEOUT:
    return "Operation timed out";
  case LIBUSB_ERROR_OVERFLOW:
    return "Overflow";
  case LIBUSB_ERROR_PIPE:
    return "Pipe error";
   case LIBUSB_ERROR_INTERRUPTED:
    return "System call interrupted (perhaps due to signal)";
  case LIBUSB_ERROR_NO_MEM:
    return "Insufficient memory";
  case LIBUSB_ERROR_NOT_SUPPORTED:
    return "Operation not supported or unimplemented on this platform";
  case LIBUSB_ERROR_OTHER:
    return "Unknown error";
  }

  return "Unknown error";
}

struct libusb_device *
_get_usb_device (struct libusb_device_handle *udh)
{
  return libusb_get_device (udh);
}

struct libusb_device_descriptor
_get_usb_device_descriptor(struct libusb_device *q)
{
  int ret;
  struct libusb_device_descriptor desc;

  ret = libusb_get_device_descriptor(q, &desc);

  if (ret < 0) {
    fprintf (stderr, "usrp: libusb_get_device_descriptor failed: %s\n",
             _get_usb_error_str(ret));
  }
  return desc;
}

int
_get_usb_string_descriptor (struct libusb_device_handle *udh, int index,
                            unsigned char* data, int length)
{
  int ret;
  ret = libusb_get_string_descriptor_ascii (udh, (uint8_t) index, data, length);

  if (ret < 0) {
    fprintf (stderr, "usrp: libusb_get_string_descriptor_ascii failed: %s\n",
             _get_usb_error_str(ret));
  }
  return ret;
}

int
_usb_control_transfer (struct libusb_device_handle *udh, int request_type,
                       int request, int value, int index,
                       unsigned char *data, int length, unsigned int timeout)
{
  int ret;
  ret = libusb_control_transfer (udh, request_type, request, value, index,
                                 data, length, timeout);
  if (ret < 0) {
    fprintf (stderr, "usrp: libusb_control_transfer failed: %s\n",
             _get_usb_error_str(ret));
  }
  return ret;
}


// ----------------------------------------------------------------


void
usrp_one_time_init (libusb_context **ctx)
{
  int ret;

  if ((ret = libusb_init (ctx)) < 0)
    fprintf (stderr, "usrp: libusb_init failed: %s\n", _get_usb_error_str(ret));

  // Enable debug verbosity if requested. This will only work if the debug
  // option is compiled into libusb and may produce a generous amount of output
  // on stdout. If debug output is not compiled into libusb, this call does
  // nothing. 
  if (LIBUSB1_DEBUG)
    libusb_set_debug(*ctx, 3);
}

void
usrp_deinit (struct libusb_context *ctx)
{
  // Each object _should_ be running in its own context. If running in default
  // context then leave the instance open as it may be shared.
  if (ctx != NULL)
    libusb_exit (ctx);
}

void
usrp_rescan ()
{
  // nop
}

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
    fprintf(stderr, "usrp: libusb_get_device_list failed: %s\n",
            _get_usb_error_str(cnt));

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
    fprintf (stderr, "%s:usb_claim_interface: failed interface %d\n",
             __FUNCTION__, interface);
    fprintf (stderr, "%s\n", _get_usb_error_str(ret));
    libusb_close (udh);
    return 0;
  }

  if ((ret = libusb_set_interface_alt_setting (udh, interface,
                                               altinterface)) < 0) {
    fprintf (stderr, "%s:usb_set_alt_interface: failed\n", __FUNCTION__);
    fprintf (stderr, "%s\n", _get_usb_error_str(ret));
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
// write vendor extension command to USRP


int
write_cmd (struct libusb_device_handle *udh,
           int request, int value, int index,
           unsigned char *bytes, int len)
{
  int requesttype = (request & 0x80) ? VRT_VENDOR_IN : VRT_VENDOR_OUT;

  int ret = libusb_control_transfer(udh, requesttype, request, value, index,
                                    bytes, len, 1000);

  if (ret < 0) {
    // we get EPIPE if the firmware stalls the endpoint.
    if (ret != LIBUSB_ERROR_PIPE) {
      fprintf (stderr, "usrp: libusb_control_transfer failed: %s\n",
               _get_usb_error_str(ret));
      fprintf (stderr, "usrp: write_cmd failed\n");
    }
  }

  return ret;
}


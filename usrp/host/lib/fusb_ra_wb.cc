/* -*- c++ -*- */
/*
 * Copyright 2003,2006 Free Software Foundation, Inc.
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

#include <fusb_ra_wb.h>
#include <usb.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/event.h>
#include <dev/usb/usb.h>

static const int USB_TIMEOUT = 1000;	// in milliseconds

// the following comment and function is from fusb_linux.cc
#if 0
// Totally evil and fragile extraction of file descriptor from
// guts of libusb.  They don't install usbi.h, which is what we'd need
// to do this nicely.
//
// FIXME if everything breaks someday in the future, look here...

static int
fd_from_usb_dev_handle (usb_dev_handle *udh)
{
  return *((int *) udh);
}
#endif

// the control endpoint doesn't actually do us any good so here is a
// new "fragile extraction"
static int
ep_fd_from_usb_dev_handle (usb_dev_handle *udh, int endpoint)
{
  struct usb_dev_handle_kludge2 { // see also usrp_prims.cc
    int			 fd;
    struct usb_bus	*bus;
    struct usb_device	*device;
    int			 config;
    int			 interface;
    int			 altsetting;
    void		*impl_info;
  };
  struct bsd_usb_dev_handle_info_kludge {
    int			 ep_fd[USB_MAX_ENDPOINTS];
  };
  struct bsd_usb_dev_handle_info_kludge *info
      = (struct bsd_usb_dev_handle_info_kludge *)
           ((struct usb_dev_handle_kludge2 *)udh)->impl_info;
  return info->ep_fd[UE_GET_ADDR(endpoint)];
}


fusb_devhandle_ra_wb::fusb_devhandle_ra_wb (usb_dev_handle *udh)
  : fusb_devhandle (udh)
{
  // that's it
}

fusb_devhandle_ra_wb::~fusb_devhandle_ra_wb ()
{
  // nop
}

fusb_ephandle *
fusb_devhandle_ra_wb::make_ephandle (int endpoint, bool input_p,
				     int block_size, int nblocks)
{
  return new fusb_ephandle_ra_wb (this, endpoint, input_p,
				  block_size, nblocks);
}

// ----------------------------------------------------------------

fusb_ephandle_ra_wb::fusb_ephandle_ra_wb (fusb_devhandle_ra_wb *dh,
					  int endpoint, bool input_p,
					  int block_size, int nblocks)
  : fusb_ephandle (endpoint, input_p, block_size, nblocks),
    d_devhandle (dh), d_ra_wb_on (false)
{
  // that's it 
}

fusb_ephandle_ra_wb::~fusb_ephandle_ra_wb ()
{
  // nop
}

bool
fusb_ephandle_ra_wb::start ()
{
  d_started = true;

  char buf = '\0';
  int fd;

  // this is to cause libusb to open the endpoint
  if (!d_input_p) {
    write(&buf, 0);
    fd = ep_fd_from_usb_dev_handle (d_devhandle->get_usb_dev_handle(),
				    d_endpoint);
  }
  else {
    read(&buf, 0);
    fd = ep_fd_from_usb_dev_handle (d_devhandle->get_usb_dev_handle(),
				    d_endpoint|USB_ENDPOINT_IN);
  }

  // enable read ahead/write behind
  int ret;
  struct usb_bulk_ra_wb_opt opts;
  int enable = 1;

  opts.ra_wb_buffer_size = d_block_size*d_nblocks;
  opts.ra_wb_request_size = d_block_size;
//  fprintf (stderr, "setting buffer size to %d, request size to %d\n",
//	   opts.ra_wb_buffer_size, opts.ra_wb_request_size);
  if (!d_input_p) {
    ret = ioctl (fd, USB_SET_BULK_WB_OPT, &opts);
    if (ret < 0)
      fprintf (stderr, "USB_SET_BULK_WB_OPT: %s\n", strerror(errno));
    else {
      ret = ioctl (fd, USB_SET_BULK_WB, &enable);
      if (ret < 0)
	fprintf (stderr, "USB_SET_BULK_WB: %s\n", strerror(errno));
      else
	d_ra_wb_on = true;
    }
  }
  else {
    ret = ioctl (fd, USB_SET_BULK_RA_OPT, &opts);
    if (ret < 0)
      fprintf (stderr, "USB_SET_BULK_RA_OPT: %s\n", strerror(errno));
    else {
      ret = ioctl (fd, USB_SET_BULK_RA, &enable);
      if (ret < 0)
	fprintf (stderr, "USB_SET_BULK_RA: %s\n", strerror(errno));
      else
	d_ra_wb_on = true;
    }
  }

  return true;
}

bool
fusb_ephandle_ra_wb::stop ()
{
  int fd;
  int ret;
  int enable = 0;
  if (d_ra_wb_on) {
    if (!d_input_p) {
      fd = ep_fd_from_usb_dev_handle (d_devhandle->get_usb_dev_handle(),
				      d_endpoint);
      ret = ioctl (fd, USB_SET_BULK_WB, &enable);
      if (ret < 0)
	fprintf (stderr, "USB_SET_BULK_WB: %s\n", strerror(errno));
      else
	d_ra_wb_on = false;
    }
    else {
      fd = ep_fd_from_usb_dev_handle (d_devhandle->get_usb_dev_handle(),
				      d_endpoint|USB_ENDPOINT_IN);
      ret = ioctl (fd, USB_SET_BULK_RA, &enable);
      if (ret < 0)
	fprintf (stderr, "USB_SET_BULK_RA: %s\n", strerror(errno));
      else
	d_ra_wb_on = false;
    }
  }

  d_started = false;
  return true;
}

int
fusb_ephandle_ra_wb::write (const void *buffer, int nbytes)
{
  if (!d_started)
    return -1;
  
  if (d_input_p)
    return -1;
  
  return usb_bulk_write (d_devhandle->get_usb_dev_handle (),
			 d_endpoint, (char *) buffer, nbytes, USB_TIMEOUT);
}

int
fusb_ephandle_ra_wb::read (void *buffer, int nbytes)
{
  if (!d_started)
    return -1;

  if (!d_input_p)
    return -1;

  return usb_bulk_read (d_devhandle->get_usb_dev_handle (),
			d_endpoint|USB_ENDPOINT_IN, (char *) buffer, nbytes,
			USB_TIMEOUT);
}

void
fusb_ephandle_ra_wb::wait_for_completion ()
{
  // as the driver is implemented this only makes sense for write 
  if (d_ra_wb_on && !d_input_p) {
    int fd = ep_fd_from_usb_dev_handle (d_devhandle->get_usb_dev_handle(),
					d_endpoint);
    int kq = kqueue();
    if (kq < 0)
      return;
    struct kevent evt;
    int nevents;
    EV_SET (&evt, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0/*NULL*/);
    nevents = kevent (kq, &evt, 1, &evt, 1, NULL);
    if (nevents < 1) {
      close(kq);
      return;
    }
    while (!(evt.flags & EV_ERROR) && evt.data < (d_block_size*d_nblocks)) {
      // it's a busy loop, but that's all I can do at the moment
      nevents = kevent (kq, NULL, 0, &evt, 1, NULL);
      // let's see if this improves the test_usrp_standard_tx throughput &
      // "CPU usage" by looping less frequently
      struct timeval timeout;
      timeout.tv_sec = 0;
      timeout.tv_usec = 1000; // 1 ms
      select (0, NULL, NULL, NULL, &timeout);
    }
    close (kq);
  }
}

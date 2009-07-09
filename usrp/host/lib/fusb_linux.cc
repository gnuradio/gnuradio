/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
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

#include <fusb_linux.h>
#include <usb.h>		// libusb header
#include <stdexcept>
#ifdef HAVE_LINUX_COMPILER_H
#include <linux/compiler.h>
#endif
#include <linux/usbdevice_fs.h>	// interface to kernel portion of user mode usb driver
#include <sys/ioctl.h>
#include <assert.h>
#include <string.h>
#include <algorithm>
#include <errno.h>
#include <string.h>
#include <cstdio>

#define MINIMIZE_TX_BUFFERING 1		// must be defined to 0 or 1


static const int MAX_BLOCK_SIZE = fusb_sysconfig::max_block_size();		// hard limit
static const int DEFAULT_BLOCK_SIZE = MAX_BLOCK_SIZE;
static const int DEFAULT_BUFFER_SIZE = 4 * (1L << 20);				// 4 MB / endpoint


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

inline static void
urb_set_ephandle (usbdevfs_urb *urb, fusb_ephandle_linux *handle)
{
  urb->usercontext = handle;
}

inline static fusb_ephandle_linux *
urb_get_ephandle (usbdevfs_urb *urb)
{
  return (fusb_ephandle_linux *) urb->usercontext;
}

// ------------------------------------------------------------------------
// 		   USB request block (urb) allocation
// ------------------------------------------------------------------------

static usbdevfs_urb *
alloc_urb (fusb_ephandle_linux *self, int buffer_length, int endpoint,
	   bool input_p, unsigned char *write_buffer)
{
  usbdevfs_urb	*urb = new usbdevfs_urb;
  memset (urb, 0, sizeof (*urb));

  urb->buffer_length = buffer_length;

  // We allocate dedicated memory only for input buffers.
  // For output buffers we reuse the same buffer (the kernel 
  // copies the data at submital time)

  if (input_p)
    urb->buffer = new unsigned char [buffer_length];
  else
    urb->buffer = write_buffer;

  // init common values

  urb->type = USBDEVFS_URB_TYPE_BULK;
  urb->endpoint = (endpoint & 0x7f) | (input_p ? 0x80 : 0);

  // USBDEVFS_URB_QUEUE_BULK goes away in linux 2.5, but is needed if
  // we are using a 2.4 usb-uhci host controller driver.  This is
  // unlikely since we're almost always going to be plugged into a
  // high speed host controller (ehci)
#if 0 && defined (USBDEVFS_URB_QUEUE_BULK)
  urb->flags = USBDEVFS_URB_QUEUE_BULK;
#endif

  urb->signr = 0;
  urb_set_ephandle (urb, self);

  return urb;
}

static void
free_urb (usbdevfs_urb *urb)
{
  // if this was an input urb, free the buffer
  if (urb->endpoint & 0x80)
    delete [] ((unsigned char *) urb->buffer);

  delete urb;
}

// ------------------------------------------------------------------------
// 				device handle
// ------------------------------------------------------------------------

fusb_devhandle_linux::fusb_devhandle_linux (usb_dev_handle *udh)
  : fusb_devhandle (udh)
{
  // that's all
}

fusb_devhandle_linux::~fusb_devhandle_linux ()
{
  // if there are any pending requests, cancel them and free the urbs.
  
  std::list<usbdevfs_urb*>::reverse_iterator it;

  for (it = d_pending_rqsts.rbegin (); it != d_pending_rqsts.rend (); it++){
    _cancel_urb (*it);
    free_urb (*it);
  }
}

fusb_ephandle *
fusb_devhandle_linux::make_ephandle (int endpoint, bool input_p,
				     int block_size, int nblocks)
{
  return new fusb_ephandle_linux (this, endpoint, input_p,
				  block_size, nblocks);
}


// Attempt to cancel all transactions associated with eph.

void
fusb_devhandle_linux::_cancel_pending_rqsts (fusb_ephandle_linux *eph)
{
  std::list<usbdevfs_urb*>::reverse_iterator it;

  for (it = d_pending_rqsts.rbegin (); it != d_pending_rqsts.rend (); it++){
    if (urb_get_ephandle (*it) == eph)
      _cancel_urb (*it);
  }
}

void 
fusb_devhandle_linux::pending_add (usbdevfs_urb *urb)
{
  d_pending_rqsts.push_back (urb);
}

usbdevfs_urb *
fusb_devhandle_linux::pending_get ()
{
  if (d_pending_rqsts.empty ())
    return 0;

  usbdevfs_urb *urb = d_pending_rqsts.front ();
  d_pending_rqsts.pop_front ();
  return urb;
}

bool
fusb_devhandle_linux::pending_remove (usbdevfs_urb *urb)
{
  std::list<usbdevfs_urb*>::iterator	result = find (d_pending_rqsts.begin (),
						       d_pending_rqsts.end (),
						       urb);
  if (result == d_pending_rqsts.end ()){
    fprintf (stderr, "fusb::pending_remove: failed to find urb in pending_rqsts: %p\n", urb);
    return false;
  }
  d_pending_rqsts.erase (result);
  return true;
}

/*
 * Submit the urb to the kernel.
 * iff successful, the urb will be placed on the devhandle's pending list.
 */
bool
fusb_devhandle_linux::_submit_urb (usbdevfs_urb *urb)
{
  int	ret;

  ret = ioctl (fd_from_usb_dev_handle (d_udh), USBDEVFS_SUBMITURB, urb);
  if (ret < 0){
    perror ("fusb::_submit_urb");
    return false;
  }
  
  pending_add (urb);
  return true;
}

/*
 * Attempt to cancel the in pending or in-progress urb transaction.
 * Return true iff transaction was sucessfully cancelled.
 *
 * Failure to cancel should not be considered a problem.  This frequently
 * occurs if the transaction has already completed in the kernel but hasn't
 * yet been reaped by the user mode code.
 *
 * urbs which were cancelled have their status field set to -ENOENT when
 * they are reaped.
 */
bool
fusb_devhandle_linux::_cancel_urb (usbdevfs_urb *urb)
{
  int ret = ioctl (fd_from_usb_dev_handle (d_udh), USBDEVFS_DISCARDURB, urb);
  if (ret < 0){
    // perror ("fusb::_cancel_urb");
    return false;
  }
  return true;
}

/*
 * Check with the kernel and see if any of our outstanding requests
 * have completed.  For each completed transaction, remove it from the
 * devhandle's pending list and append it to the completed list for
 * the corresponding endpoint.
 *
 * If any transactions are reaped return true.
 *
 * If ok_to_block_p is true, then this will block until at least one
 * transaction completes or an unrecoverable error occurs.
 */
bool
fusb_devhandle_linux::_reap (bool ok_to_block_p)
{
  int		ret;
  int		nreaped = 0;
  usbdevfs_urb	*urb = 0;

  int	fd = fd_from_usb_dev_handle (d_udh);
  
  // try to reap as many as possible without blocking...

  while ((ret = ioctl (fd, USBDEVFS_REAPURBNDELAY, &urb)) == 0){
    if (urb->status != 0 && urb->status != -ENOENT){
      fprintf (stderr, "_reap: usb->status = %d, actual_length = %5d\n",
	       urb->status, urb->actual_length);
    }
    pending_remove (urb);
    urb_get_ephandle (urb)->completed_list_add (urb);
    nreaped++;
  }

  if (nreaped > 0)		// if we got any, return w/o blocking
    return true;

  if (!ok_to_block_p)
    return false;
  
  ret = ioctl (fd, USBDEVFS_REAPURB, &urb);
  if (ret < 0){
    perror ("fusb::_reap");
    return false;
  }

  pending_remove (urb);
  urb_get_ephandle (urb)->completed_list_add (urb);
  return true;
}

void
fusb_devhandle_linux::_wait_for_completion ()
{
  while (!d_pending_rqsts.empty ())
    if (!_reap(true))
      break;
}
// ------------------------------------------------------------------------
// 			     end point handle
// ------------------------------------------------------------------------

fusb_ephandle_linux::fusb_ephandle_linux (fusb_devhandle_linux *devhandle,
					  int endpoint,
					  bool input_p,
					  int block_size, int nblocks)
  : fusb_ephandle (endpoint, input_p, block_size, nblocks),
    d_devhandle (devhandle), 
    d_write_work_in_progress (0), d_write_buffer (0),
    d_read_work_in_progress (0), d_read_buffer (0), d_read_buffer_end (0)
{

  if (d_block_size < 0 || d_block_size > MAX_BLOCK_SIZE)
    throw std::out_of_range ("fusb_ephandle_linux: block_size");

  if (d_nblocks < 0)
    throw std::out_of_range ("fusb_ephandle_linux: nblocks");

  if (d_block_size == 0)
    d_block_size = DEFAULT_BLOCK_SIZE;

  if (d_nblocks == 0)
    d_nblocks = std::max (1, DEFAULT_BUFFER_SIZE / d_block_size);

  if (!d_input_p)
    if (!MINIMIZE_TX_BUFFERING)
      d_write_buffer = new unsigned char [d_block_size];

  if (0)
    fprintf(stderr, "fusb_ephandle_linux::ctor: d_block_size = %d  d_nblocks = %d\n",
	    d_block_size, d_nblocks);

  // allocate urbs

  for (int i = 0; i < d_nblocks; i++)
    d_free_list.push_back (alloc_urb (this, d_block_size, d_endpoint,
				      d_input_p, d_write_buffer));
}

fusb_ephandle_linux::~fusb_ephandle_linux ()
{
  stop ();

  usbdevfs_urb *urb;

  while ((urb = free_list_get ()) != 0)
    free_urb (urb);

  while ((urb = completed_list_get ()) != 0)
    free_urb (urb);

  if (d_write_work_in_progress)
    free_urb (d_write_work_in_progress);

  delete [] d_write_buffer;

  if (d_read_work_in_progress)
    free_urb (d_read_work_in_progress);
}

// ----------------------------------------------------------------

bool
fusb_ephandle_linux::start ()
{
  if (d_started)
    return true;		// already running

  d_started = true;

  if (d_input_p){		// fire off all the reads
    usbdevfs_urb *urb;

    int nerrors = 0;
    while ((urb = free_list_get ()) != 0 && nerrors < d_nblocks){
      if (!submit_urb (urb))
	nerrors++;
    }
  }

  return true;
}

//
// kill all i/o in progress.
// kill any completed but unprocessed transactions.
//
bool
fusb_ephandle_linux::stop ()
{
  if (!d_started)
    return true;

  if (d_write_work_in_progress){
    free_list_add (d_write_work_in_progress);
    d_write_work_in_progress = 0;
  }

  if (d_read_work_in_progress){
    free_list_add (d_read_work_in_progress);
    d_read_work_in_progress = 0;
    d_read_buffer = 0;
    d_read_buffer_end = 0;
  }

  d_devhandle->_cancel_pending_rqsts (this);
  d_devhandle->_reap (false);

  while (1){
    usbdevfs_urb *urb;
    while ((urb = completed_list_get ()) != 0)
      free_list_add (urb);

    if (d_free_list.size () == (unsigned) d_nblocks)
      break;

    if (!d_devhandle->_reap(true))
      break;
  }

  d_started = false;
  return true;
}

// ----------------------------------------------------------------
//			routines for writing 
// ----------------------------------------------------------------

#if (MINIMIZE_TX_BUFFERING)

int 
fusb_ephandle_linux::write(const void *buffer, int nbytes)
{
  if (!d_started)
    return -1;
  
  if (d_input_p)
    return -1;

  assert(nbytes % 512 == 0);

  unsigned char *src = (unsigned char *) buffer;

  int n = 0;
  while (n < nbytes){

    usbdevfs_urb *urb = get_write_work_in_progress();
    if (!urb)
      return -1;
    assert(urb->actual_length == 0);
    int m = std::min(nbytes - n, MAX_BLOCK_SIZE);
    urb->buffer = src;
    urb->buffer_length = m;

    n += m;
    src += m;

    if (!submit_urb(urb))
      return -1;

    d_write_work_in_progress = 0;
  }

  return n;
}

#else

int 
fusb_ephandle_linux::write (const void *buffer, int nbytes)
{
  if (!d_started)
    return -1;
  
  if (d_input_p)
    return -1;

  unsigned char *src = (unsigned char *) buffer;

  int n = 0;
  while (n < nbytes){

    usbdevfs_urb *urb = get_write_work_in_progress ();
    if (!urb)
      return -1;
    unsigned char *dst = (unsigned char *) urb->buffer;
    int m = std::min (nbytes - n, urb->buffer_length - urb->actual_length);

    memcpy (&dst[urb->actual_length], &src[n], m);
    urb->actual_length += m;
    n += m;

    if (urb->actual_length == urb->buffer_length){
      if (!submit_urb (urb))
	return -1;
      d_write_work_in_progress = 0;
    }
  }

  return n;
}

#endif

usbdevfs_urb *
fusb_ephandle_linux::get_write_work_in_progress ()
{
  // if we've already got some work in progress, return it

  if (d_write_work_in_progress)
    return d_write_work_in_progress;

  while (1){

    reap_complete_writes ();

    usbdevfs_urb *urb = free_list_get ();

    if (urb != 0){
      assert (urb->actual_length == 0);
      d_write_work_in_progress = urb;
      return urb;
    }

    // The free list is empty.  Tell the device handle to reap.
    // Anything it reaps for us will end up on our completed list.

    if (!d_devhandle->_reap (true))
      return 0;
  }
}

void
fusb_ephandle_linux::reap_complete_writes ()
{
  // take a look at the completed_list and xfer to free list after
  // checking for errors.

  usbdevfs_urb *urb;
  
  while ((urb = completed_list_get ()) != 0){

    // Check for any errors or short writes that were reported in the urb.
    // The kernel sets status, actual_length and error_count.
    // error_count is only used for ISO xfers.
    // status is 0 if successful, else is an errno kind of thing

    if (urb->status != 0){
      fprintf (stderr, "fusb: (status %d) %s\n", urb->status, strerror (-urb->status));
    }
    else if (urb->actual_length != urb->buffer_length){
      fprintf (stderr, "fusb: short write xfer: %d != %d\n",
	       urb->actual_length, urb->buffer_length);
    }

    free_list_add (urb);
  }
}

void
fusb_ephandle_linux::wait_for_completion ()
{
  d_devhandle->_wait_for_completion ();
}

// ----------------------------------------------------------------
//		       routines for reading
// ----------------------------------------------------------------

int
fusb_ephandle_linux::read (void *buffer, int nbytes)
{
  if (!d_started)
    return -1;
  
  if (!d_input_p)
    return -1;

  unsigned char *dst = (unsigned char *) buffer;

  int n = 0;
  while (n < nbytes){

    if (d_read_buffer >= d_read_buffer_end)
      if (!reload_read_buffer ())
	return -1;

    int m = std::min (nbytes - n, (int) (d_read_buffer_end - d_read_buffer));

    memcpy (&dst[n], d_read_buffer, m);
    d_read_buffer += m;
    n += m;
  }

  return n;
}

bool
fusb_ephandle_linux::reload_read_buffer ()
{
  assert (d_read_buffer >= d_read_buffer_end);

  usbdevfs_urb *urb;

  if (d_read_work_in_progress){
    // We're done with this urb.  Fire off a read to refill it.
    urb = d_read_work_in_progress;
    d_read_work_in_progress = 0;
    d_read_buffer = 0;
    d_read_buffer_end = 0;
    urb->actual_length = 0;
    if (!submit_urb (urb))
      return false;
  }

  while (1){

    while ((urb = completed_list_get ()) == 0)
      if (!d_devhandle->_reap (true))
	return false;

    // check result of completed read

    if (urb->status != 0){
      // We've got a problem. Report it and fail.
      fprintf (stderr, "fusb: (rd status %d) %s\n", urb->status, strerror (-urb->status));
      urb->actual_length = 0;
      free_list_add (urb);
      return false;
    }

    // we've got a happy urb, full of data...

    d_read_work_in_progress = urb;
    d_read_buffer = (unsigned char *) urb->buffer;
    d_read_buffer_end = d_read_buffer + urb->actual_length;

    return true;
  }
}

// ----------------------------------------------------------------

void
fusb_ephandle_linux::free_list_add (usbdevfs_urb *urb)
{
  assert (urb_get_ephandle (urb) == this);
  urb->actual_length = 0;
  d_free_list.push_back (urb);
}

usbdevfs_urb *
fusb_ephandle_linux::free_list_get ()
{
  if (d_free_list.empty ())
    return 0;

  usbdevfs_urb *urb = d_free_list.front ();
  d_free_list.pop_front ();
  return urb;
}

void
fusb_ephandle_linux::completed_list_add (usbdevfs_urb *urb)
{
  assert (urb_get_ephandle (urb) == this);
  d_completed_list.push_back (urb);
}

usbdevfs_urb *
fusb_ephandle_linux::completed_list_get ()
{
  if (d_completed_list.empty ())
    return 0;

  usbdevfs_urb *urb = d_completed_list.front ();
  d_completed_list.pop_front ();
  return urb;
}

/*
 * Submit the urb.  If successful the urb ends up on the devhandle's
 * pending list, otherwise, it's back on our free list.
 */
bool
fusb_ephandle_linux::submit_urb (usbdevfs_urb *urb)
{
  if (!d_devhandle->_submit_urb (urb)){    // FIXME record the problem somewhere
    fprintf (stderr, "_submit_urb failed\n");
    free_list_add (urb);
    return false;
  }
  return true;
}

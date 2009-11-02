/* -*- c++ -*- */
/*
 * Copyright 2003,2009 Free Software Foundation, Inc.
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

#include <fusb_libusb1.h>
#include <fusb.h>
#include <libusb-1.0/libusb.h>
#include <stdexcept>
#include <cstdio>
#include <assert.h>
#include <string.h>
#include <algorithm>
#include <errno.h>
#include <string.h>

#define MINIMIZE_TX_BUFFERING true

static const int MAX_BLOCK_SIZE = fusb_sysconfig::max_block_size();
static const int DEFAULT_BLOCK_SIZE = MAX_BLOCK_SIZE;
static const int DEFAULT_BUFFER_SIZE = 4 * (1L << 20);  // 4 MB endpoint
static const int LIBUSB_TIMEOUT = 0; 			// no timeout

inline static fusb_ephandle_libusb1 *
lut_get_ephandle (libusb_transfer *lut)
{
  return (fusb_ephandle_libusb1 *) lut->user_data;
}

// ------------------------------------------------------------------------
// 	libusb_transfer allocation, deallocation, and callback
// ------------------------------------------------------------------------

static void
free_lut (libusb_transfer *lut)
{

  // if this was an input transfer, free the buffer
  if (lut->endpoint & 0x80)
    delete [] ((unsigned char *) lut->buffer);

  libusb_free_transfer(lut);

}

/*
 * The callback means the libusb_transfer is completed whether sent, cancelled,
 * or failed. Move the libusb_transfer from the pending list to the
 * completed list. If the cancel is from the destructor then free the
 * transfer instead; normally this won't happen since all endpoints should be
 * destroyed first leaving the pending list empty.
 */

static void
generic_callback(struct libusb_transfer *lut)
{

  // Fish out devhandle from endpoint
  fusb_devhandle_libusb1* dev_handle =
    lut_get_ephandle(lut)->get_fusb_devhandle_libusb1();

  dev_handle->pending_remove(lut);

  if (lut->status == LIBUSB_TRANSFER_CANCELLED && dev_handle->_teardown() == 1)
  {
    free_lut (lut);
    return;
  }

  lut_get_ephandle(lut)->completed_list_add(lut);

}

static libusb_transfer*
alloc_lut (fusb_ephandle_libusb1 *self, int buffer_length, int endpoint,
           bool input_p, unsigned char *write_buffer,
           fusb_devhandle_libusb1 *dh)
{

  struct libusb_transfer* lut = libusb_alloc_transfer(0);

  endpoint = (endpoint & 0x7f) | (input_p ? 0x80 : 0);

  if (input_p)
    write_buffer = new unsigned char [buffer_length];

  // We need the base class libusb_device_handle
  libusb_device_handle *dev_handle = dh->get_usb_dev_handle();

  // Load the libusb_transfer for bulk transfer
  libusb_fill_bulk_transfer (lut,		// transfer
                             dev_handle,        // dev_handle
                             endpoint,		// endpoint
                             write_buffer,	// buffer
                             buffer_length,	// length
                             generic_callback,	// callback
                             self,		// user_data
                             LIBUSB_TIMEOUT);	// timeout

  return lut;
}

// ------------------------------------------------------------------------
// 				device handle
// ------------------------------------------------------------------------

fusb_devhandle_libusb1::fusb_devhandle_libusb1 (libusb_device_handle *udh,
                                                libusb_context *ctx)
  : fusb_devhandle (udh), d_ctx (ctx), d_teardown (false)
{
  // that's it
}

fusb_devhandle_libusb1::~fusb_devhandle_libusb1 ()
{
  d_teardown = true;

  std::list<libusb_transfer*>::reverse_iterator it;

  // After cancellation the libusb_transfer is still active so delay freeing
  // transfer until callback occurs. In most cases the pending list should
  // already be empty by the time this destructor is called.

  for (it = d_pending_rqsts.rbegin (); it != d_pending_rqsts.rend (); it++) {
    _cancel_lut (*it);
  }

  // Wait for pending list to empty
  _wait_for_completion ();

}

fusb_ephandle*
fusb_devhandle_libusb1::make_ephandle (int endpoint, bool input_p,
				       int block_size, int nblocks)
{
  return new fusb_ephandle_libusb1 (this, endpoint, input_p,
				    block_size, nblocks);
}

/*
 * devhandle list manipulators
 */

void
fusb_devhandle_libusb1::pending_add (libusb_transfer *lut)
{
  d_pending_rqsts.push_back (lut);
}


/*
 * Attempt to cancel all transations associated with eph
 */

void
fusb_devhandle_libusb1::_cancel_pending_rqsts (fusb_ephandle_libusb1 *eph)
{
  std::list<libusb_transfer*>::reverse_iterator it;

  for (it = d_pending_rqsts.rbegin (); it != d_pending_rqsts.rend (); it++){
    if (lut_get_ephandle (*it) == eph)
      _cancel_lut (*it);
  }
}

/*
 * Pull from the pending list
 */

libusb_transfer *
fusb_devhandle_libusb1::pending_get ()
{
  if (d_pending_rqsts.empty ())
    return 0;

  libusb_transfer *lut = d_pending_rqsts.front ();
  d_pending_rqsts.pop_front ();
  return lut;
}

/*
 * Match libusb_tranfer with the pending list and erase
 * Return true if found, false otherwise
 */

bool
fusb_devhandle_libusb1::pending_remove (libusb_transfer *lut)
{
  std::list<libusb_transfer*>::iterator	result;
  result = find (d_pending_rqsts.begin (), d_pending_rqsts.end (), lut);

  if (result == d_pending_rqsts.end ()) {
    fprintf (stderr, "fusb::pending_remove: failed to find lut in pending_rqsts: %p\n", lut);

    return false;
  }
  d_pending_rqsts.erase (result);
  return true;
}

/*
 * Submit the libusb_transfer to libusb
 * iff successful, the transfer will be placed on the devhandle pending list.
 */

bool
fusb_devhandle_libusb1::_submit_lut (libusb_transfer *lut)
{

  int ret = libusb_submit_transfer (lut);
  if (ret < 0) {
    fprintf(stderr, "fusb::_submit_lut %d", ret);
    return false;
  }

  pending_add(lut);
  return true;

}

/*
 * Attempt to cancel any pending libusb_transfer transactions.
 * Return true in the absence of errors, which does not mean that the transfer
 * is cancelled. Cancellation can be checked after the callback is fired off
 * by libusb.
 */

bool
fusb_devhandle_libusb1::_cancel_lut (libusb_transfer *lut)
{

  int ret = libusb_cancel_transfer (lut);
  if (ret < 0) {
    fprintf (stderr, "fusb::_cancel_lut");
    return false;
  }
  return true;

}

/*
 * Reimplementing _reap for context use and compatibiliy with libusb-0.12.
 *
 * Returns false on timeout or error, true if an event was handled
 *
 * If ok_to_block_p is false then handle already pending events and return
 * immediately.
 *
 * If ok_to_block_p is true then call libusb_handle_events_timeout with default
 * timeout value of 2 seconds, which waits and returns on event arrival or
 * timeout.
 */

bool
fusb_devhandle_libusb1::_reap (bool ok_to_block_p)
{
  int ret;
  struct timeval tv;

  // Save pending size
  int pnd_size = d_pending_rqsts.size();

  if (ok_to_block_p) {
    tv.tv_sec = 2;
    tv.tv_usec =  0;
  }
  else {
    tv.tv_sec = 0;
    tv.tv_usec =  0;
  }

  if ((ret = libusb_handle_events_timeout(d_ctx, &tv)) < 0) {
    fprintf (stderr, "fusb::_reap libusb_handle_events() %i\n", ret);
    return false;
  }

  // Check that a pending transfer was removed
  if (pnd_size > d_pending_rqsts.size())
    return true;
  else {
    return false;
  }
}

void
fusb_devhandle_libusb1::_wait_for_completion ()
{

  while (!d_pending_rqsts.empty ())
    if (!_reap(true))
      break;

}

// ------------------------------------------------------------------------
// 				endpoint handle
// ------------------------------------------------------------------------

fusb_ephandle_libusb1::fusb_ephandle_libusb1 (fusb_devhandle_libusb1 *dh,
					      int endpoint, bool input_p,
					      int block_size, int nblocks)
  : fusb_ephandle (endpoint, input_p, block_size, nblocks),
    d_devhandle (dh),
    d_write_work_in_progress (0), d_write_buffer (0),
    d_read_work_in_progress (0), d_read_buffer (0), d_read_buffer_end (0)
{

  if (d_block_size < 0 || d_block_size > MAX_BLOCK_SIZE)
    throw std::out_of_range ("fusb_ephandle_libusb1: block_size");

  if (d_nblocks < 0)
    throw std::out_of_range ("fusb_ephandle_libusb1: nblocks");

  if (d_block_size == 0)
    d_block_size = DEFAULT_BLOCK_SIZE;

  if (d_nblocks == 0)
    d_nblocks = std::max (1, DEFAULT_BUFFER_SIZE / d_block_size);

  if (!d_input_p)
    if (!MINIMIZE_TX_BUFFERING)
      d_write_buffer = new unsigned char [d_block_size];

  if (0)
    fprintf(stderr, "fusb_ephandle_libusb1::ctor: d_block_size = %d  d_nblocks = %d\n",
      d_block_size, d_nblocks);

  // allocate libusb_transfers
  for (int i = 0; i < d_nblocks; i++)
    d_free_list.push_back (alloc_lut (this, d_block_size, d_endpoint,
                                      d_input_p, d_write_buffer, d_devhandle));
}

fusb_ephandle_libusb1::~fusb_ephandle_libusb1 ()
{

  stop ();

  libusb_transfer *lut;

  while ((lut = free_list_get ()) != 0)
    free_lut (lut);

  while ((lut = completed_list_get ()) != 0)
    free_lut (lut);

  if (d_write_work_in_progress)
    free_lut (d_write_work_in_progress);

  delete [] d_write_buffer;

  if (d_read_work_in_progress)
    free_lut (d_read_work_in_progress);

}

bool
fusb_ephandle_libusb1::start ()
{
  if (d_started)
    return true;

  d_started = true;

  if (d_input_p) {
     libusb_transfer *lut;

     int nerrors = 0;
     while ((lut = free_list_get ()) !=0 && nerrors < d_nblocks) {
       if (!submit_lut (lut))
         nerrors++;
     }
  }

  return true;

}

/*
 * Cancel all transfers in progress or pending and return to initial state
 */

bool
fusb_ephandle_libusb1::stop ()
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

  while (1) {
    libusb_transfer *lut;
    while ((lut = completed_list_get ()) != 0)
      free_list_add (lut);

    if (d_free_list.size () == (unsigned) d_nblocks)
      break;

    if (!d_devhandle->_reap(true))
      break;
  }

  d_started = false;

  return true;
}

// ------------------------------------------------------------------------
// 			routines for writing
// ------------------------------------------------------------------------

#if (MINIMIZE_TX_BUFFERING)

int
fusb_ephandle_libusb1::write (const void *buffer, int nbytes)
{
  if (!d_started)	// doesn't matter here, but keeps semantics constant
    return -1;

  if (d_input_p)
    return -1;

  assert(nbytes % 512 == 0);

  unsigned char *src = (unsigned char *) buffer;

  int n = 0;
  while (n < nbytes){

    struct libusb_transfer *lut = get_write_work_in_progress();
    if (!lut)
      return -1;
    assert(lut->actual_length == 0);
    int m = std::min(nbytes - n, MAX_BLOCK_SIZE);
    lut->buffer = src;
    lut->length = m;

    n += m;
    src += m;

    if (!submit_lut(lut))
      return -1;

    d_write_work_in_progress = 0;
  }

  return n;
}

#else

int
fusb_ephandle_libusb1::write (const void *buffer, int nbytes)
{
  if (!d_started)
    return -1;

  if (d_input_p)
    return -1;

  unsigned char *src = (unsigned char *) buffer;

  int n = 0;
  while (n < nbytes){

    libusb_transfer *lut = get_write_work_in_progress ();
    if (!lut)
      return -1;
    unsigned char *dst = (unsigned char *) lut->buffer;
    int m = std::min (nbytes - n, lut->length - lut->actual_length);

    memcpy (&dst[lut->actual_length], &src[n], m);
    lut->actual_length += m;
    n += m;

    if (lut->actual_length == lut->length){
      if (!submit_lut (lut))
        return -1;
      d_write_work_in_progress = 0;
    }
  }

  return n;
}

#endif

struct libusb_transfer *
fusb_ephandle_libusb1::get_write_work_in_progress ()
{
  if (d_write_work_in_progress)
    return d_write_work_in_progress;

  while (1) {

    reap_complete_writes ();

    struct libusb_transfer *lut = free_list_get ();

    if (lut != 0){
      assert (lut->actual_length == 0);
      d_write_work_in_progress = lut;
      return lut;
    }

    if (!d_devhandle->_reap (true))
      return 0;
  }
}

void
fusb_ephandle_libusb1::reap_complete_writes ()
{
  // take a look at the completed list and xfer to free list after
  // checking for errors.

  libusb_transfer *lut;

  while ((lut = completed_list_get ()) != 0) {

    // Check for any errors or short writes that were reporetd in the transfer.
    // libusb1 sets status, actual_length.

    if (lut->status != LIBUSB_TRANSFER_COMPLETED) {
      fprintf (stderr, "fusb: (status %d) \n", lut->status );
    }
    else if (lut->actual_length != lut->length){
      fprintf (stderr, "fusb: short write xfer: %d != %d\n",
               lut->actual_length, lut->length);
    }

    free_list_add (lut);
  }
}

void
fusb_ephandle_libusb1::wait_for_completion ()
{
  d_devhandle->_wait_for_completion ();
}

// ------------------------------------------------------------------------
// 			routines for reading
// ------------------------------------------------------------------------

int
fusb_ephandle_libusb1::read (void *buffer, int nbytes)
{
  if (!d_started)	// doesn't matter here, but keeps semantics constant
    return -1;

  if (!d_input_p)
    return -1;

  unsigned char *dst = (unsigned char *) buffer;

  int n = 0;
  while (n < nbytes) {

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
fusb_ephandle_libusb1::reload_read_buffer ()
{
  assert (d_read_buffer >= d_read_buffer_end);

  libusb_transfer *lut;

  if (d_read_work_in_progress) {
    lut = d_read_work_in_progress;
    d_read_work_in_progress = 0;
    d_read_buffer = 0;
    d_read_buffer_end = 0;
    lut->actual_length = 0;
    if (!submit_lut (lut))
      return false;
  }

  while (1) {

    while ((lut = completed_list_get ()) == 0 )
      if (!d_devhandle->_reap(true))
        return false;

    if (lut->status != LIBUSB_TRANSFER_COMPLETED) {
      fprintf (stderr, "fust: (rd status %d) %s\n", lut->status,
               strerror (-lut->status));
      lut->actual_length = 0;
      free_list_add (lut);
      return false;
    }

    d_read_work_in_progress = lut;
    d_read_buffer = (unsigned char *) lut->buffer;
    d_read_buffer_end = d_read_buffer + lut->actual_length;

    return true;
  }
}


/*
 * ephandle list manipulation
 */


void
fusb_ephandle_libusb1::free_list_add (libusb_transfer *lut)
{
  assert (lut_get_ephandle (lut) == this);
  lut->actual_length = 0;
  d_free_list.push_back (lut);
}

libusb_transfer *
fusb_ephandle_libusb1::free_list_get ()
{
  if (d_free_list.empty ())
    return 0;

  libusb_transfer *lut = d_free_list.front ();
  d_free_list.pop_front ();
  return lut;
}

void
fusb_ephandle_libusb1::completed_list_add (libusb_transfer *lut)
{
  assert (lut_get_ephandle (lut) == this);
  d_completed_list.push_back (lut);
}

libusb_transfer *
fusb_ephandle_libusb1::completed_list_get ()
{
  if (d_completed_list.empty ())
    return 0;

  libusb_transfer *lut = d_completed_list.front ();
  d_completed_list.pop_front ();
  return lut;
}

bool
fusb_ephandle_libusb1::submit_lut (libusb_transfer *lut)
{
  if (!d_devhandle->_submit_lut (lut)) {
    fprintf (stderr, "_submit_lut failed\n");
    free_list_add (lut);
    return false;
  }
  return true;
}

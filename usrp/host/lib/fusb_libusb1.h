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

#ifndef _FUSB_LIBUSB1_H_
#define _FUSB_LIBUSB1_H_

#include <fusb.h>
#include <list>

struct libusb_transfer;
struct libusb_context;

class fusb_ephandle_libusb1;

/*!
 * \brief libusb1 implementation of fusb_devhandle
 */
class fusb_devhandle_libusb1 : public fusb_devhandle
{
private:
  std::list<libusb_transfer*>	 d_pending_rqsts;
  libusb_context		*d_ctx;

  void pending_add (struct libusb_transfer *lut);
  struct libusb_transfer * pending_get ();

  bool d_teardown;

public:
  // CREATORS
  fusb_devhandle_libusb1 (libusb_device_handle *udh, libusb_context *ctx);
  virtual ~fusb_devhandle_libusb1 ();

  // MANIPULATORS
  virtual fusb_ephandle *make_ephandle (int endpoint, bool input_p,
					int block_size = 0, int nblocks = 0);
  // internal use only
  bool _submit_lut (libusb_transfer *);
  bool _cancel_lut (libusb_transfer *);
  void _cancel_pending_rqsts (fusb_ephandle_libusb1 *eph);
  bool _reap (bool ok_to_block_p);
  void _wait_for_completion ();

  // accessors to work from callback context
  bool pending_remove (struct libusb_transfer *lut);
  inline bool _teardown() { return d_teardown; }

};


/*!
 * \brief libusb1 implementation of fusb_ephandle
 */
class fusb_ephandle_libusb1 : public fusb_ephandle
{
private:
  fusb_devhandle_libusb1	 *d_devhandle;
  std::list<libusb_transfer*>     d_free_list;
  std::list<libusb_transfer*>     d_completed_list;
  libusb_transfer                *d_write_work_in_progress;
  unsigned char                  *d_write_buffer;
  libusb_transfer                *d_read_work_in_progress;
  unsigned char                  *d_read_buffer;
  unsigned char                  *d_read_buffer_end;

  libusb_transfer *get_write_work_in_progress ();
  void reap_complete_writes ();
  bool reload_read_buffer ();
  bool submit_lut (libusb_transfer *lut);

public:
  // CREATORS
  fusb_ephandle_libusb1 (fusb_devhandle_libusb1 *dh, int endpoint, bool input_p,
			 int block_size = 0, int nblocks = 0);
  virtual ~fusb_ephandle_libusb1 ();

  // MANIPULATORS

  virtual bool start ();  	//!< begin streaming i/o
  virtual bool stop ();		//!< stop streaming i/o

  /*!
   * \returns \p nbytes if write was successfully enqueued, else -1.
   * Will block if no free buffers available.
   */
  virtual int write (const void *buffer, int nbytes);

  /*!
   * \returns number of bytes read or -1 if error.
   * number of bytes read will be <= nbytes.
   * Will block if no input available.
   */
  virtual int read (void *buffer, int nbytes);

  /*
   * block until all outstanding writes have completed
   */
  virtual void wait_for_completion ();

  void free_list_add (struct libusb_transfer *lut);
  void completed_list_add (struct libusb_transfer *lut);
  struct libusb_transfer *free_list_get ();
  struct libusb_transfer *completed_list_get ();

  // accessor to work from callback context
  fusb_devhandle_libusb1* get_fusb_devhandle_libusb1 () const {
    return d_devhandle;
  }
};

#endif /* _FUSB_LINUX1_H_ */


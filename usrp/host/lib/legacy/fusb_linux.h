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

// Fast USB interface

#ifndef _FUSB_LINUX_H_
#define _FUSB_LINUX_H_

#include <fusb.h>
#include <list>

struct  usbdevfs_urb;
class   fusb_ephandle_linux;

/*!
 * \brief linux specific implementation of fusb_devhandle using usbdevice_fs
 */
class fusb_devhandle_linux : public fusb_devhandle {
private:
  std::list<usbdevfs_urb*>	 d_pending_rqsts;

  void pending_add (usbdevfs_urb *urb);
  bool pending_remove (usbdevfs_urb *urb);
  usbdevfs_urb * pending_get ();


public:
  // CREATORS
  fusb_devhandle_linux (usb_dev_handle *udh);
  virtual ~fusb_devhandle_linux ();

  // MANIPULATORS
  virtual fusb_ephandle *make_ephandle (int endpoint, bool input_p,
					int block_size = 0, int nblocks = 0);

  // internal use only
  bool _submit_urb (usbdevfs_urb *urb);
  bool _cancel_urb (usbdevfs_urb *urb);
  void _cancel_pending_rqsts (fusb_ephandle_linux *eph);
  bool _reap (bool ok_to_block_p);
  void _wait_for_completion ();
};

/*!
 * \brief linux specific implementation of fusb_ephandle using usbdevice_fs
 */

class fusb_ephandle_linux : public fusb_ephandle {
private:
  fusb_devhandle_linux	       *d_devhandle;
  std::list<usbdevfs_urb*>	d_free_list;
  std::list<usbdevfs_urb*>	d_completed_list;
  usbdevfs_urb		       *d_write_work_in_progress;
  unsigned char		       *d_write_buffer;
  usbdevfs_urb		       *d_read_work_in_progress;
  unsigned char		       *d_read_buffer;
  unsigned char		       *d_read_buffer_end;

  usbdevfs_urb *get_write_work_in_progress ();
  void reap_complete_writes ();
  bool reload_read_buffer ();
  bool submit_urb (usbdevfs_urb *urb);
  
public:
  fusb_ephandle_linux (fusb_devhandle_linux *dh, int endpoint, bool input_p,
		       int block_size = 0, int nblocks = 0);
  virtual ~fusb_ephandle_linux ();

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

  // internal use only
  void free_list_add (usbdevfs_urb *urb);
  void completed_list_add (usbdevfs_urb *urb);
  usbdevfs_urb *free_list_get (); 		// pop and return head of list or 0
  usbdevfs_urb *completed_list_get ();		// pop and return head of list or 0
};

#endif /* _FUSB_LINUX_H_ */

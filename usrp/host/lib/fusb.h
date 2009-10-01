/*  -*- c++ -*- */
/*
 * Copyright 2005,2009 Free Software Foundation, Inc.
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

#ifndef _FUSB_H_
#define _FUSB_H_

#include <usrp/libusb_types.h>

struct 	libusb_context;
class   fusb_ephandle;

/*!
 * \brief abstract usb device handle
 */
class fusb_devhandle {
private:
  // NOT IMPLEMENTED
  fusb_devhandle (const fusb_devhandle &rhs);		  // no copy constructor
  fusb_devhandle &operator= (const fusb_devhandle &rhs);  // no assignment operator

protected:
  libusb_device_handle		*d_udh;

public:
  // CREATORS
  fusb_devhandle (libusb_device_handle *udh);
  virtual ~fusb_devhandle ();

  // MANIPULATORS
  
  /*!
   * \brief return an ephandle of the correct subtype
   */
  virtual fusb_ephandle *make_ephandle (int endpoint, bool input_p,
					int block_size = 0, int nblocks = 0) = 0;
  
  // ACCESSORS
  libusb_device_handle *get_usb_dev_handle () const { return d_udh; }
};


/*!
 * \brief abstract usb end point handle
 */
class fusb_ephandle {
private:
  // NOT IMPLEMENTED
  fusb_ephandle (const fusb_ephandle &rhs);	        // no copy constructor
  fusb_ephandle &operator= (const fusb_ephandle &rhs);  // no assignment operator

protected:
  int				d_endpoint;
  bool				d_input_p;
  int				d_block_size;
  int				d_nblocks;
  bool				d_started;

public:
  fusb_ephandle (int endpoint, bool input_p,
		 int block_size = 0, int nblocks = 0);
  virtual ~fusb_ephandle ();

  virtual bool start () = 0;  	//!< begin streaming i/o
  virtual bool stop () = 0;	//!< stop streaming i/o

  /*!
   * \returns \p nbytes if write was successfully enqueued, else -1.
   * Will block if no free buffers available.
   */
  virtual int write (const void *buffer, int nbytes) = 0;

  /*!
   * \returns number of bytes read or -1 if error.
   * number of bytes read will be <= nbytes.
   * Will block if no input available.
   */
  virtual int read (void *buffer, int nbytes) = 0;

  /*
   * block until all outstanding writes have completed
   */
  virtual void wait_for_completion () = 0;

  /*!
   * \brief returns current block size.
   */
  int block_size () { return d_block_size; };
};


/*!
 * \brief factory for creating concrete instances of the appropriate subtype.
 */
class fusb_sysconfig {
public:
  /*!
   * \brief returns fusb_devhandle or throws if trouble
   */
  static fusb_devhandle *make_devhandle (libusb_device_handle *udh,
                                         libusb_context *ctx = 0);

  /*!
   * \brief Returns max block size in bytes (hard limit).
   */
  static int max_block_size ();

  /*!
   * \brief Returns default block size in bytes.
   */
  static int default_block_size ();

  /*!
   * \brief Returns the default buffer size in bytes.
   */
  static int default_buffer_size ();

};

#endif /* _FUSB_H_ */

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

#ifndef _FUSB_RA_WB_H_
#define _FUSB_RA_WB_H_

#include <fusb.h>

/*!
 * \brief generic implementation of fusb_devhandle using only libusb
 */
class fusb_devhandle_ra_wb : public fusb_devhandle
{
public:
  // CREATORS
  fusb_devhandle_ra_wb (usb_dev_handle *udh);
  virtual ~fusb_devhandle_ra_wb ();

  // MANIPULATORS
  virtual fusb_ephandle *make_ephandle (int endpoint, bool input_p,
					int block_size = 0, int nblocks = 0);
};


/*!
 * \brief generic implementation of fusb_ephandle using only libusb
 */
class fusb_ephandle_ra_wb : public fusb_ephandle
{
private:
  fusb_devhandle_ra_wb	*d_devhandle;
  bool d_ra_wb_on;
  
public:
  // CREATORS
  fusb_ephandle_ra_wb (fusb_devhandle_ra_wb *dh, int endpoint, bool input_p,
		       int block_size = 0, int nblocks = 0);
  virtual ~fusb_ephandle_ra_wb ();

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
};

#endif /* _FUSB_RA_WB_H_ */


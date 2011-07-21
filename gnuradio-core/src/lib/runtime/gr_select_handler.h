/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_SELECT_HANDLER_H
#define INCLUDED_GR_SELECT_HANDLER_H

#include <gr_core_api.h>
#include <boost/shared_ptr.hpp>

class gr_select_handler;
typedef boost::shared_ptr<gr_select_handler> gr_select_handler_sptr;


/*!
 * \brief Abstract handler for select based notification.
 * \ingroup base
 *
 * \sa gr_dispatcher
 */
class GR_CORE_API gr_select_handler
{
  int	d_fd;

protected:
  gr_select_handler(int file_descriptor);

public:
  virtual ~gr_select_handler();

  int fd() const { return d_fd; }
  int file_descriptor() const { return d_fd; }

  /*!
   * \brief Called when file_descriptor is readable.
   *
   * Called when the dispatcher detects that file_descriptor can
   * be read without blocking.
   */
  virtual void handle_read() = 0;

  /*!
   * \brief Called when file_descriptor is writable.
   *
   * Called when dispatcher detects that file descriptor can be 
   * written without blocking.
   */
  virtual void handle_write() = 0;

  /*!
   * Called each time around the dispatcher loop to determine whether
   * this handler's file descriptor should be added to the list on which
   * read events can occur.  The default method returns true, indicating
   * that by default, all handlers are interested in read events.
   */
  virtual bool readable() { return true; }

  /*!
   * Called each time around the dispatcher loop to determine whether
   * this handler's file descriptor should be added to the list on which
   * write events can occur.  The default method returns true, indicating
   * that by default, all handlers are interested in write events.
   */
  virtual bool writable() { return true; }
};

#endif /* INCLUDED_GR_SELECT_HANDLER_H */

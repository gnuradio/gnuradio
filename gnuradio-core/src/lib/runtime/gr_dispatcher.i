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

class gr_dispatcher;
typedef boost::shared_ptr<gr_dispatcher> gr_dispatcher_sptr;
%template(gr_dispatcher_sptr) boost::shared_ptr<gr_dispatcher>;

%rename(dispatcher) gr_make_dispatcher;
gr_dispatcher_sptr gr_make_dispatcher();

%rename(dispatcher_singleton) gr_dispatcher_singleton;
gr_dispatcher_sptr gr_dispatcher_singleton();

/*!
 * \brief invoke callbacks based on select.
 *
 * \sa gr_select_handler
 */
class gr_dispatcher
{
  gr_dispatcher();

public:
  ~gr_dispatcher();

  /*!
   * \brief Event dispatching loop.
   *
   * Enter a polling loop that only terminates after all gr_select_handlers
   * have been removed.  \p timeout sets the timeout parameter to the select()
   * call, measured in seconds.
   *
   * \param timeout maximum number of seconds to block in select.
   */
  void loop(double timeout=10);
};

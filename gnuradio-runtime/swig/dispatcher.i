/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
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

class gr::dispatcher;
typedef boost::shared_ptr<gr::dispatcher> gr::dispatcher_sptr;
%template(dispatcher_sptr) boost::shared_ptr<gr::dispatcher>;

%rename(dispatcher) gr::make_dispatcher;
gr::dispatcher_sptr gr::make_dispatcher();

%rename(dispatcher_singleton) gr::dispatcher_singleton;
gr::dispatcher_sptr gr::dispatcher_singleton();

/*!
 * \brief invoke callbacks based on select.
 *
 * \sa gr::select_handler
 */
class gr::dispatcher
{
  dispatcher();

public:
  ~dispatcher();

  /*!
   * \brief Event dispatching loop.
   *
   * Enter a polling loop that only terminates after all gr::select_handlers
   * have been removed. \p timeout sets the timeout parameter to the select()
   * call, measured in seconds.
   *
   * \param timeout maximum number of seconds to block in select.
   */
  void loop(double timeout=10);
};


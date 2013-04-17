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

#ifndef INCLUDED_GR_DISPATCHER_H
#define INCLUDED_GR_DISPATCHER_H

#include <gnuradio/api.h>
#include <gnuradio/select_handler.h>
#include <vector>

namespace gr {

  class dispatcher;
  typedef boost::shared_ptr<dispatcher> dispatcher_sptr;

  GR_RUNTIME_API dispatcher_sptr dispatcher_singleton();
  GR_RUNTIME_API dispatcher_sptr make_dispatcher();

  /*!
   * \brief invoke callbacks based on select.
   * \ingroup internal
   *
   * \sa gr_select_handler
   */
  class GR_RUNTIME_API dispatcher
  {
    dispatcher();
    friend GR_RUNTIME_API dispatcher_sptr make_dispatcher();

    std::vector<select_handler_sptr> d_handler;
    int d_max_index;

  public:
    ~dispatcher();

    bool add_handler(select_handler_sptr handler);
    bool del_handler(select_handler_sptr handler);
    bool del_handler(select_handler *handler);

    /*!
     * \brief Event dispatching loop.
     *
     * Enter a polling loop that only terminates after all
     * gr::select_handlers have been removed. \p timeout sets the
     * timeout parameter to the select() call, measured in seconds.
     *
     * \param timeout maximum number of seconds to block in select.
     */
    void loop(double timeout=10);
  };

} /* namespace gr */

#endif /* INCLUDED_GR_DISPATCHER_H */

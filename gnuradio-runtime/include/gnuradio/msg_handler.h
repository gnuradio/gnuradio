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

#ifndef INCLUDED_GR_MSG_HANDLER_H
#define INCLUDED_GR_MSG_HANDLER_H

#include <gnuradio/api.h>
#include <gnuradio/message.h>

namespace gr {

  class msg_handler;
  typedef boost::shared_ptr<msg_handler> msg_handler_sptr;

  /*!
   * \brief abstract class of message handlers
   * \ingroup base
   */
  class GR_RUNTIME_API msg_handler
  {
  public:
    virtual ~msg_handler();

    //! handle \p msg
    virtual void handle(message::sptr msg) = 0;
  };

} /* namespace gr */

#endif /* INCLUDED_GR_MSG_HANDLER_H */

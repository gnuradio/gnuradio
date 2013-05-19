/* -*- c++ -*- */
/*
 * Copyright 2009,2013 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INCLUDED_MSG_ACCEPTER_MSGQ_H
#define INCLUDED_MSG_ACCEPTER_MSGQ_H

#include <gnuradio/api.h>
#include <gnuradio/messages/msg_accepter.h>
#include <gnuradio/messages/msg_queue.h>

namespace gr {
  namespace messages {

    /*!
     * \brief Concrete class that accepts messages and inserts them
     * into a message queue.
     */
    class GR_RUNTIME_API msg_accepter_msgq : public msg_accepter
    {
    protected:
      msg_queue_sptr d_msg_queue;

    public:
      msg_accepter_msgq(msg_queue_sptr msgq);
      ~msg_accepter_msgq();

      virtual void post(pmt::pmt_t msg);

      msg_queue_sptr msg_queue() const { return d_msg_queue; }
    };

  } /* namespace messages */
} /* namespace gr */

#endif /* INCLUDED_MSG_ACCEPTER_MSGQ_H */

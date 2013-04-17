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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/messages/msg_accepter_msgq.h>

namespace gr {
  namespace messages {

    msg_accepter_msgq::msg_accepter_msgq(msg_queue_sptr msgq)
      : d_msg_queue(msgq)
    {
    }

    msg_accepter_msgq::~msg_accepter_msgq()
    {
      // NOP, required as virtual destructor
    }

    void
    msg_accepter_msgq::post(pmt::pmt_t msg)
    {
      d_msg_queue->insert_tail(msg);
    }

  } /* namespace messages */
} /* namespace gr */

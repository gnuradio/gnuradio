/* -*- c++ -*- */
/*
 * Copyright 2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/messages/msg_accepter_msgq.h>

namespace gr {
namespace messages {

msg_accepter_msgq::msg_accepter_msgq(msg_queue_sptr msgq) : d_msg_queue(msgq) {}

msg_accepter_msgq::~msg_accepter_msgq()
{
    // NOP, required as virtual destructor
}

void msg_accepter_msgq::post(pmt::pmt_t msg) { d_msg_queue->insert_tail(msg); }

} /* namespace messages */
} /* namespace gr */

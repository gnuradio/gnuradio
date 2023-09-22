/* -*- c++ -*- */
/*
 * Copyright 2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
class GR_RUNTIME_API msg_accepter_msgq
{
protected:
    msg_queue_sptr d_msg_queue;

public:
    msg_accepter_msgq(msg_queue_sptr msgq);
    virtual ~msg_accepter_msgq();

    virtual void post(pmt::pmt_t msg);

    msg_queue_sptr msg_queue() const { return d_msg_queue; }
};

} /* namespace messages */
} /* namespace gr */

#endif /* INCLUDED_MSG_ACCEPTER_MSGQ_H */

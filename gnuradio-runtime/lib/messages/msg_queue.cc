/* -*- c++ -*- */
/*
 * Copyright 2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/messages/msg_queue.h>
#include <stdexcept>

namespace gr {
namespace messages {

msg_queue_sptr make_msg_queue(unsigned int limit)
{
    return msg_queue_sptr(new msg_queue(limit));
}

msg_queue::msg_queue(unsigned int limit) : d_limit(limit) {}

msg_queue::~msg_queue() { flush(); }

void msg_queue::insert_tail(pmt::pmt_t msg)
{
    gr::thread::scoped_lock guard(d_mutex);

    while (full_p())
        d_not_full.wait(guard);

    d_msgs.push_back(msg);
    d_not_empty.notify_one();
}

pmt::pmt_t msg_queue::delete_head()
{
    gr::thread::scoped_lock guard(d_mutex);

    while (empty_p())
        d_not_empty.wait(guard);

    pmt::pmt_t m(d_msgs.front());
    d_msgs.pop_front();

    if (d_limit > 0) // Unlimited length queues never block on write
        d_not_full.notify_one();

    return m;
}

pmt::pmt_t msg_queue::delete_head_nowait()
{
    gr::thread::scoped_lock guard(d_mutex);

    if (empty_p())
        return pmt::pmt_t();

    pmt::pmt_t m(d_msgs.front());
    d_msgs.pop_front();

    if (d_limit > 0) // Unlimited length queues never block on write
        d_not_full.notify_one();

    return m;
}

void msg_queue::flush()
{
    while (delete_head_nowait() != pmt::pmt_t())
        ;
}

} /* namespace messages */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2005,2009 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_MSG_QUEUE_H
#define INCLUDED_GR_MSG_QUEUE_H

#include <gnuradio/api.h>
#include <gnuradio/msg_handler.h>
#include <gnuradio/thread/thread.h>

namespace gr {

/*!
 * \brief thread-safe message queue
 * \ingroup misc
 */
class GR_RUNTIME_API msg_queue : public msg_handler
{
    gr::thread::mutex d_mutex;
    gr::thread::condition_variable d_not_empty;
    gr::thread::condition_variable d_not_full;
    message::sptr d_head;
    message::sptr d_tail;
    unsigned int d_count; // # of messages in queue.
    unsigned int d_limit; // max # of messages in queue.  0 -> unbounded

public:
    typedef std::shared_ptr<msg_queue> sptr;

    static sptr make(unsigned int limit = 0);

    msg_queue(unsigned int limit);
    ~msg_queue() override;

    //! Generic msg_handler method: insert the message.
    void handle(message::sptr msg) override { insert_tail(msg); }

    /*!
     * \brief Insert message at tail of queue.
     * \param msg message
     *
     * Block if queue if full.
     */
    void insert_tail(message::sptr msg);

    /*!
     * \brief Delete message from head of queue and return it.
     * Block if no message is available.
     */
    message::sptr delete_head();

    /*!
     * \brief If there's a message in the q, delete it and return it.
     * If no message is available, return 0.
     */
    message::sptr delete_head_nowait();

    //! Delete all messages from the queue
    void flush();

    //! is the queue empty?
    bool empty_p() const { return d_count == 0; }

    //! is the queue full?
    bool full_p() const { return d_limit != 0 && d_count >= d_limit; }

    //! return number of messages in queue
    unsigned int count() const { return d_count; }

    //! return limit on number of message in queue.  0 -> unbounded
    unsigned int limit() const { return d_limit; }
};

} /* namespace gr */

#endif /* INCLUDED_GR_MSG_QUEUE_H */

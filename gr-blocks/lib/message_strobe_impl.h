/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_MESSAGE_STROBE_IMPL_H
#define INCLUDED_GR_MESSAGE_STROBE_IMPL_H

#include <gnuradio/blocks/message_strobe.h>

namespace gr {
namespace blocks {

class BLOCKS_API message_strobe_impl : public message_strobe
{
private:
    boost::shared_ptr<gr::thread::thread> d_thread;
    bool d_finished;
    long d_period_ms;
    pmt::pmt_t d_msg;
    const pmt::pmt_t d_port;

    void run();

public:
    message_strobe_impl(pmt::pmt_t msg, long period_ms);
    ~message_strobe_impl();

    void set_msg(pmt::pmt_t msg) { d_msg = msg; }
    pmt::pmt_t msg() const { return d_msg; }
    void set_period(long period_ms) { d_period_ms = period_ms; }
    long period() const { return d_period_ms; }

    // Overloading these to start and stop the internal thread that
    // periodically produces the message.
    bool start();
    bool stop();
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_STROBE_IMPL_H */

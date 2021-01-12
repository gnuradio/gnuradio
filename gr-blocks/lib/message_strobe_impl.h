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
#include <atomic>

namespace gr {
namespace blocks {

class BLOCKS_API message_strobe_impl : public message_strobe
{
private:
    gr::thread::thread d_thread;
    std::atomic<bool> d_finished;
    long d_period_ms;
    pmt::pmt_t d_msg;
    const pmt::pmt_t d_port;

    void run();

public:
    message_strobe_impl(pmt::pmt_t msg, long period_ms);
    ~message_strobe_impl() override;

    void set_msg(pmt::pmt_t msg) override { d_msg = msg; }
    pmt::pmt_t msg() const override { return d_msg; }
    void set_period(long period_ms) override { d_period_ms = period_ms; }
    long period() const override { return d_period_ms; }

    // Overloading these to start and stop the internal thread that
    // periodically produces the message.
    bool start() override;
    bool stop() override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_STROBE_IMPL_H */

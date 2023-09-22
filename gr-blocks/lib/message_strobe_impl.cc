/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "message_strobe_impl.h"
#include <gnuradio/io_signature.h>
#include <chrono>
#include <thread>

namespace gr {
namespace blocks {

message_strobe::sptr message_strobe::make(pmt::pmt_t msg, long period_ms)
{
    return gnuradio::make_block_sptr<message_strobe_impl>(msg, period_ms);
}

message_strobe_impl::message_strobe_impl(pmt::pmt_t msg, long period_ms)
    : block("message_strobe", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_finished(false),
      d_period_ms(period_ms),
      d_msg(msg),
      d_port(pmt::mp("strobe"))
{
    message_port_register_out(d_port);

    message_port_register_in(pmt::mp("set_msg"));
    set_msg_handler(pmt::mp("set_msg"), [this](pmt::pmt_t msg) { this->set_msg(msg); });
}

message_strobe_impl::~message_strobe_impl() {}

bool message_strobe_impl::start()
{
    d_finished = false;
    d_thread = gr::thread::thread([this] { run(); });

    return block::start();
}

bool message_strobe_impl::stop()
{
    // Shut down the thread
    d_finished = true;
    d_thread.interrupt();
    d_thread.join();

    return block::stop();
}

void message_strobe_impl::run()
{
    while (!d_finished) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<long>(d_period_ms)));
        if (d_finished) {
            return;
        }

        message_port_pub(d_port, d_msg);
    }
}

} /* namespace blocks */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2013,2014,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ZEROMQ_REP_MSG_SINK_IMPL_H
#define INCLUDED_ZEROMQ_REP_MSG_SINK_IMPL_H

#include "zmq_common_impl.h"
#include <gnuradio/zeromq/rep_msg_sink.h>
#include <thread>

namespace gr {
namespace zeromq {

class rep_msg_sink_impl : public rep_msg_sink
{
private:
    std::chrono::milliseconds d_timeout;
    zmq::context_t d_context;
    zmq::socket_t d_socket;
    std::unique_ptr<std::thread> d_thread;
    bool d_finished;

    const pmt::pmt_t d_port;

    void readloop();

public:
    rep_msg_sink_impl(char* address, int timeout, bool bind);
    ~rep_msg_sink_impl() override;

    bool start() override;
    bool stop() override;

    std::string last_endpoint() override
    {
        return d_socket.get(zmq::sockopt::last_endpoint);
    }
};

} // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_REP_MSG_SINK_IMPL_H */

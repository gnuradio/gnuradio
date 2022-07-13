/* -*- c++ -*- */
/*
 * Copyright 2013,2014,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ZEROMQ_SUB_MSG_SOURCE_IMPL_H
#define INCLUDED_ZEROMQ_SUB_MSG_SOURCE_IMPL_H

#include "zmq_common_impl.h"
#include <gnuradio/zeromq/sub_msg_source.h>
#include <thread>

namespace gr {
namespace zeromq {

class sub_msg_source_impl : public sub_msg_source
{
private:
    std::chrono::milliseconds d_timeout; // microseconds, -1 is blocking
    zmq::context_t d_context;
    zmq::socket_t d_socket;
    std::unique_ptr<std::thread> d_thread;
    const pmt::pmt_t d_port;

    void readloop();

public:
    bool d_finished;

    sub_msg_source_impl(char* address, int timeout, bool bind);
    ~sub_msg_source_impl() override;

    bool start() override;
    bool stop() override;

    std::string last_endpoint() override
    {
        return d_socket.get(zmq::sockopt::last_endpoint);
    }
};

} // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_SUB_MSG_SOURCE_IMPL_H */

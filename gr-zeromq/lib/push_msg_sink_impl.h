/* -*- c++ -*- */
/*
 * Copyright 2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ZEROMQ_PUSH_MSG_SINK_IMPL_H
#define INCLUDED_ZEROMQ_PUSH_MSG_SINK_IMPL_H

#include "zmq_common_impl.h"
#include <gnuradio/zeromq/push_msg_sink.h>

namespace gr {
namespace zeromq {

class push_msg_sink_impl : public push_msg_sink
{
private:
    float d_timeout;
    zmq::context_t d_context;
    zmq::socket_t d_socket;

public:
    push_msg_sink_impl(char* address, int timeout, bool bind);
    ~push_msg_sink_impl() override;

    void handler(pmt::pmt_t msg);
    std::string last_endpoint() override
    {
        return d_socket.get(zmq::sockopt::last_endpoint);
    }
};

} // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_ZMQ_PUSH_MSG_SINK_IMPL_H */

/* -*- c++ -*- */
/*
 * Copyright 2013,2014 Free Software Foundation, Inc.
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/zeromq/push_msg_sink.h>

#include <zmq.hpp>

namespace gr {
namespace zeromq {

class push_msg_sink_cpu : public virtual push_msg_sink
{
public:
    push_msg_sink_cpu(block_args args);

    std::string last_endpoint() const override
    {
        return d_socket.get(zmq::sockopt::last_endpoint);
    }

private:
    int d_timeout; // microseconds, -1 is blocking
    zmq::context_t d_context;
    zmq::socket_t d_socket;

    void handle_msg_in(pmtf::pmt msg) override;
};

} // namespace zeromq
} // namespace gr
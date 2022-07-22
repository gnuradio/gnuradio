/* -*- c++ -*- */
/*
 * Copyright 2013,2014,2019 Free Software Foundation, Inc.
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/zeromq/pull_msg_source.h>

#include <zmq.hpp>
#include <thread>

namespace gr {
namespace zeromq {

class pull_msg_source_cpu : public virtual pull_msg_source
{
public:
    pull_msg_source_cpu(block_args args);
    bool start() override;
    bool stop() override;
    std::string last_endpoint() const override
    {
        return d_socket.get(zmq::sockopt::last_endpoint);
    }

private:
    bool d_finished;
    int d_timeout; // microseconds, -1 is blocking
    zmq::context_t d_context;
    zmq::socket_t d_socket;
    std::thread d_thread;


    void readloop();
};

} // namespace zeromq
} // namespace gr
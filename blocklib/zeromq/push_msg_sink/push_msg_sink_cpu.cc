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

#include "push_msg_sink_cpu.h"
#include "push_msg_sink_cpu_gen.h"

namespace {
constexpr int LINGER_DEFAULT = 1000; // 1 second.
}

namespace gr {
namespace zeromq {

push_msg_sink_cpu::push_msg_sink_cpu(block_args args)
    : INHERITED_CONSTRUCTORS,
      d_timeout(args.timeout),
      d_context(1),
      d_socket(d_context, ZMQ_PUSH)
{

    d_socket.set(zmq::sockopt::linger, LINGER_DEFAULT);

    if (args.bind) {
        d_socket.bind(args.address);
    }
    else {
        d_socket.connect(args.address);
    }
}

void push_msg_sink_cpu::handle_msg_in(pmtf::pmt msg)
{
    std::stringbuf sb("");
    msg.serialize(sb);
    std::string s = sb.str();
    zmq::message_t zmsg(s.size());
    memcpy(zmsg.data(), s.c_str(), s.size());
    d_socket.send(zmsg, zmq::send_flags::none);
}

} // namespace zeromq
} // namespace gr
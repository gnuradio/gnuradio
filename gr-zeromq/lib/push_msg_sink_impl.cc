/* -*- c++ -*- */
/*
 * Copyright 2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "push_msg_sink_impl.h"
#include "tag_headers.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace zeromq {

push_msg_sink::sptr push_msg_sink::make(char* address, int timeout, bool bind)
{
    return gnuradio::get_initial_sptr(new push_msg_sink_impl(address, timeout, bind));
}

push_msg_sink_impl::push_msg_sink_impl(char* address, int timeout, bool bind)
    : gr::block("push_msg_sink",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(0, 0, 0)),
      d_timeout(timeout)
{
    int major, minor, patch;
    zmq::version(&major, &minor, &patch);

    if (major < 3) {
        d_timeout = timeout * 1000;
    }

    d_context = new zmq::context_t(1);
    d_socket = new zmq::socket_t(*d_context, ZMQ_PUSH);

    int time = 0;
    d_socket->setsockopt(ZMQ_LINGER, &time, sizeof(time));

    if (bind) {
        d_socket->bind(address);
    } else {
        d_socket->connect(address);
    }

    message_port_register_in(pmt::mp("in"));
    set_msg_handler(pmt::mp("in"), [this](pmt::pmt_t msg) { this->handler(msg); });
}

push_msg_sink_impl::~push_msg_sink_impl()
{
    d_socket->close();
    delete d_socket;
    delete d_context;
}

void push_msg_sink_impl::handler(pmt::pmt_t msg)
{
    std::stringbuf sb("");
    pmt::serialize(msg, sb);
    std::string s = sb.str();
    zmq::message_t zmsg(s.size());

    memcpy(zmsg.data(), s.c_str(), s.size());
#if USE_NEW_CPPZMQ_SEND_RECV
    d_socket->send(zmsg, zmq::send_flags::none);
#else
    d_socket->send(zmsg);
#endif
}

} /* namespace zeromq */
} /* namespace gr */

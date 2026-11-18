/* -*- c++ -*- */
/*
 * Copyright 2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ZEROMQ_ZMQ_COMMON_IMPL_H
#define INCLUDED_ZEROMQ_ZMQ_COMMON_IMPL_H

#include <zmq.hpp>

#if defined(CPPZMQ_VERSION) && defined(ZMQ_MAKE_VERSION) && \
    CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 3, 1)
#define USE_NEW_CPPZMQ_SEND_RECV 1
#else
#define USE_NEW_CPPZMQ_SEND_RECV 0
#endif

#if defined(CPPZMQ_VERSION) && defined(ZMQ_MAKE_VERSION) && \
    CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 8, 0)
#define USE_NEW_CPPZMQ_SET_GET 1
#else
#define USE_NEW_CPPZMQ_SET_GET 0
#endif

namespace gr {
namespace zeromq {

constexpr int IPV6_DEFAULT = 1;

inline void enable_ipv6(zmq::socket_t& socket)
{
#if USE_NEW_CPPZMQ_SET_GET
    socket.set(zmq::sockopt::ipv6, IPV6_DEFAULT);
#else
    socket.setsockopt(ZMQ_IPV6, &IPV6_DEFAULT, sizeof(IPV6_DEFAULT));
#endif
}

} /* namespace zeromq */
} /* namespace gr */

#endif /* INCLUDED_ZEROMQ_ZMQ_COMMON_IMPL_H */

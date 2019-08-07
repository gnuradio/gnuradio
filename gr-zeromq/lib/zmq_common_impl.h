/* -*- c++ -*- */
/*
 * Copyright 2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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

#endif /* INCLUDED_ZEROMQ_ZMQ_COMMON_IMPL_H */

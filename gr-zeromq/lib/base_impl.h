/* -*- c++ -*- */
/*
 * Copyright 2016,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ZEROMQ_BASE_IMPL_H
#define INCLUDED_ZEROMQ_BASE_IMPL_H

#include "zmq_common_impl.h"
#include <gnuradio/sync_block.h>

namespace gr {
namespace zeromq {

class base_impl : public virtual gr::sync_block
{
public:
    base_impl(int type, size_t itemsize, size_t vlen, int timeout, bool pass_tags);
    virtual ~base_impl();

protected:
    std::string last_endpoint();
    zmq::context_t* d_context;
    zmq::socket_t* d_socket;
    size_t d_vsize;
    int d_timeout;
    bool d_pass_tags;
};

class base_sink_impl : public base_impl
{
public:
    base_sink_impl(int type,
                   size_t itemsize,
                   size_t vlen,
                   char* address,
                   int timeout,
                   bool pass_tags,
                   int hwm);

protected:
    int send_message(const void* in_buf, const int in_nitems, const uint64_t in_offset);
};

class base_source_impl : public base_impl
{
public:
    base_source_impl(int type,
                     size_t itemsize,
                     size_t vlen,
                     char* address,
                     int timeout,
                     bool pass_tags,
                     int hwm);

protected:
    zmq::message_t d_msg;
    std::vector<gr::tag_t> d_tags;
    size_t d_consumed_bytes;
    int d_consumed_items;

    bool has_pending();
    int flush_pending(void* out_buf, const int out_nitems, const uint64_t out_offset);
    bool load_message(bool wait);
};

} // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_BASE_IMPL_H */

// vim: ts=2 sw=2 expandtab

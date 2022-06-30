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

#include "push_sink_impl.h"
#include "tag_headers.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace zeromq {

push_sink::sptr push_sink::make(
    size_t itemsize, size_t vlen, char* address, int timeout, bool pass_tags, int hwm)
{
    return gnuradio::make_block_sptr<push_sink_impl>(
        itemsize, vlen, address, timeout, pass_tags, hwm);
}

push_sink_impl::push_sink_impl(
    size_t itemsize, size_t vlen, char* address, int timeout, bool pass_tags, int hwm)
    : gr::sync_block("push_sink",
                     gr::io_signature::make(1, 1, itemsize * vlen),
                     gr::io_signature::make(0, 0, 0)),
      base_sink_impl(ZMQ_PUSH, itemsize, vlen, address, timeout, pass_tags, hwm)
{
    /* All is delegated */
}

int push_sink_impl::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    // Poll with a timeout (FIXME: scheduler can't wait for us)
    zmq::pollitem_t itemsout[] = { { static_cast<void*>(d_socket), 0, ZMQ_POLLOUT, 0 } };
    zmq::poll(&itemsout[0], 1, std::chrono::milliseconds{ d_timeout });

    // If we can send something, do it
    if (itemsout[0].revents & ZMQ_POLLOUT)
        return send_message(input_items[0], noutput_items, nitems_read(0));

    // If not, do nothing
    return 0;
}

} /* namespace zeromq */
} /* namespace gr */

// vim: ts=2 sw=2 expandtab

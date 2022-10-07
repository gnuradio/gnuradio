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

#include "pub_sink_impl.h"
#include "tag_headers.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace zeromq {

pub_sink::sptr pub_sink::make(size_t itemsize,
                              size_t vlen,
                              char* address,
                              int timeout,
                              bool pass_tags,
                              int hwm,
                              const std::string& key,
                              bool drop_on_hwm)
{
    return gnuradio::make_block_sptr<pub_sink_impl>(
        itemsize, vlen, address, timeout, pass_tags, hwm, key, drop_on_hwm);
}

pub_sink_impl::pub_sink_impl(size_t itemsize,
                             size_t vlen,
                             char* address,
                             int timeout,
                             bool pass_tags,
                             int hwm,
                             const std::string& key,
                             bool drop_on_hwm)
    : gr::sync_block("pub_sink",
                     gr::io_signature::make(1, 1, itemsize * vlen),
                     gr::io_signature::make(0, 0, 0)),
      base_sink_impl(ZMQ_PUB, itemsize, vlen, address, timeout, pass_tags, hwm, key)
{
    /* Socket option to prevent dropping of samples (backpressure) */
    int no_drop = (drop_on_hwm == true) ? 0 : 1;
#if USE_NEW_CPPZMQ_SET_GET
    d_socket.set(zmq::sockopt::xpub_nodrop, no_drop);
#else
    d_socket.setsockopt(ZMQ_XPUB_NODROP, &no_drop, sizeof(no_drop));
#endif
}

int pub_sink_impl::work(int noutput_items,
                        gr_vector_const_void_star& input_items,
                        gr_vector_void_star& output_items)
{
    return send_message(input_items[0], noutput_items, nitems_read(0));
}

} /* namespace zeromq */
} /* namespace gr */

// vim: ts=2 sw=2 expandtab

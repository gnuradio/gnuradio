/* -*- c++ -*- */
/*
 * Copyright 2016,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base.h"
#include "tag_headers.h"

namespace {
constexpr int LINGER_DEFAULT = 1000; // 1 second.
}

namespace gr {
namespace zeromq {

base::base(int type, size_t itemsize, int timeout, bool pass_tags, const std::string& key)
    : d_context(1),
      d_socket(d_context, type),
      d_vsize(itemsize),
      d_timeout(timeout),
      d_pass_tags(pass_tags),
      d_key(key)
{
    /* "Fix" timeout value (ms for new API, us for old API) */
    int major, minor, patch;
    zmq::version(&major, &minor, &patch);

    if (major < 3) {
        d_timeout *= 1000;
    }
}

std::string base::last_endpoint() const
{
    return d_socket.get(zmq::sockopt::last_endpoint);
}


base_sink::base_sink(int type,
                     size_t itemsize,
                     const std::string& address,
                     int timeout,
                     bool pass_tags,
                     int hwm,
                     const std::string& key)
    : base(type, itemsize, timeout, pass_tags, key)
{
    gr::configure_default_loggers(d_base_logger, d_base_debug_logger, "zmq_base_source");

    /* Set high watermark */
    if (hwm >= 0) {
        d_socket.set(zmq::sockopt::sndhwm, hwm);
    }

    /* Set ZMQ_LINGER so socket won't infinitely block during teardown */
    d_socket.set(zmq::sockopt::linger, LINGER_DEFAULT);

    /* Bind */
    d_socket.bind(address);
}

int base_sink::send_message(const void* in_buf,
                            const int in_nitems,
                            const uint64_t in_offset,
                            const std::vector<tag_t>& tags)
{
    /* Send key if it exists */
    if (!d_key.empty()) {
        zmq::message_t key_message(d_key.size());
        memcpy(key_message.data(), d_key.data(), d_key.size());
        d_socket.send(key_message, zmq::send_flags::sndmore);
    }
    /* Meta-data header */
    std::string header("");
    if (d_pass_tags) {
        // std::vector<gr::tag_t> tags;
        // get_tags_in_range(tags, 0, in_offset, in_offset + in_nitems);
        header = gen_tag_header(in_offset, tags);
    }

    /* Create message */
    size_t payload_len = in_nitems * d_vsize;
    size_t msg_len = d_pass_tags ? payload_len + header.length() : payload_len;
    zmq::message_t msg(msg_len);

    if (d_pass_tags) {
        memcpy(msg.data(), header.c_str(), header.length());
        memcpy((uint8_t*)msg.data() + header.length(), in_buf, payload_len);
    }
    else {
        memcpy(msg.data(), in_buf, payload_len);
    }

    /* Send */
    d_socket.send(msg, zmq::send_flags::none);

    /* Report back */
    return in_nitems;
}

base_source::base_source(int type,
                         size_t itemsize,
                         const std::string& address,
                         int timeout,
                         bool pass_tags,
                         int hwm,
                         const std::string& key)
    : base(type, itemsize, timeout, pass_tags, key),
      d_consumed_bytes(0),
      d_consumed_items(0)
{
    /* Set high watermark */
    if (hwm >= 0) {
        d_socket.set(zmq::sockopt::rcvhwm, hwm);
    }

    /* Set ZMQ_LINGER so socket won't infinitely block during teardown */
    d_socket.set(zmq::sockopt::linger, LINGER_DEFAULT);

    /* Connect */
    d_socket.connect(address);
}

bool base_source::has_pending() { return d_msg.size() > d_consumed_bytes; }

int base_source::flush_pending(block_work_output& work_output,
                               const int out_nitems,
                               const uint64_t out_offset)
{
    /* How much to copy in this call */
    int to_copy_items =
        std::min(out_nitems, (int)((d_msg.size() - d_consumed_bytes) / d_vsize));
    int to_copy_bytes = d_vsize * to_copy_items;
    auto nw = work_output.nitems_written();

    /* Copy actual data */
    memcpy(work_output.items<uint8_t>() + out_offset * d_vsize,
           (uint8_t*)d_msg.data() + d_consumed_bytes,
           to_copy_bytes);

    /* Add tags matching this segment of samples */
    for (unsigned int i = 0; i < d_tags.size(); i++) {
        if ((d_tags[i].offset() >= (uint64_t)d_consumed_items) &&
            (d_tags[i].offset() < (uint64_t)d_consumed_items + to_copy_items)) {
            gr::tag_t nt = d_tags[i];
            nt.set_offset(nt.offset() + nw + out_offset - d_consumed_items);
            work_output.add_tag(nt);
        }
    }

    /* Update pointer */
    d_consumed_items += to_copy_items;
    d_consumed_bytes += to_copy_bytes;

    return to_copy_items;
}

bool base_source::load_message(bool wait)
{
    /* Poll for input */
    zmq::pollitem_t items[] = { { static_cast<void*>(d_socket), 0, ZMQ_POLLIN, 0 } };
    zmq::poll(&items[0], 1, std::chrono::milliseconds{ wait ? d_timeout : 0 });

    if (!(items[0].revents & ZMQ_POLLIN))
        return false;

    /* Is this the start or continuation of a multi-part message? */
    auto more = d_socket.get(zmq::sockopt::rcvmore);

    /* Reset */
    d_msg.rebuild();
    d_tags.clear();
    d_consumed_items = 0;
    d_consumed_bytes = 0;

    /* Get the message */
    const bool ok = bool(d_socket.recv(d_msg));

    if (!ok) {
        // This shouldn't happen since we polled POLLIN, but ZMQ wants us to check
        // the return value.
        d_base_logger->warn("Failed to recv() message.");
        return false;
    }

    /* Throw away key and get the first message. Avoid blocking if a multi-part
     * message is not sent */
    if (!d_key.empty() && !more) {
        auto is_multipart = d_socket.get(zmq::sockopt::rcvmore);

        d_msg.rebuild();
        if (is_multipart) {
            const bool multi_ok = bool(d_socket.recv(d_msg));

            if (!multi_ok) {
                d_base_logger->error("Failure to receive multi-part message.");
            }
        }
        else {
            return false;
        }
    }
    /* Parse header from the first (or only) message of a multi-part message */
    if (d_pass_tags && !more) {
        uint64_t rcv_offset;

        /* Parse header */
        d_consumed_bytes = parse_tag_header(d_msg, rcv_offset, d_tags);

        /* Fixup the tags offset to be relative to the start of this message */
        for (unsigned int i = 0; i < d_tags.size(); i++) {
            d_tags[i].set_offset(d_tags[i].offset() - rcv_offset);
        }
    }

    /* Each message must contain an integer multiple of data vectors */
    if ((d_msg.size() - d_consumed_bytes) % d_vsize != 0) {
        throw std::runtime_error("Incompatible vector sizes: need a multiple of " +
                                 std::to_string(d_vsize) + " bytes per message");
    }

    /* We got one ! */
    return true;
}

} /* namespace zeromq */
} /* namespace gr */

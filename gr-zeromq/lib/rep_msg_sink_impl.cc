/* -*- c++ -*- */
/*
 * Copyright 2013,2014,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rep_msg_sink_impl.h"
#include "tag_headers.h"
#include <gnuradio/io_signature.h>
#include <memory>

namespace gr {
namespace zeromq {

rep_msg_sink::sptr rep_msg_sink::make(char* address, int timeout, bool bind)
{
    return gnuradio::make_block_sptr<rep_msg_sink_impl>(address, timeout, bind);
}

rep_msg_sink_impl::rep_msg_sink_impl(char* address, int timeout, bool bind)
    : gr::block("rep_msg_sink",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(0, 0, 0)),
      d_timeout(timeout),
      d_context(1),
      d_socket(d_context, ZMQ_REP),
      d_port(pmt::mp("in"))
{
    int major, minor, patch;
    zmq::version(&major, &minor, &patch);

    if (major < 3) {
        d_timeout = timeout * 1000;
    }

    int time = 0;
    d_socket.setsockopt(ZMQ_LINGER, &time, sizeof(time));

    if (bind) {
        d_socket.bind(address);
    } else {
        d_socket.connect(address);
    }

    message_port_register_in(d_port);
}

rep_msg_sink_impl::~rep_msg_sink_impl() {}

bool rep_msg_sink_impl::start()
{
    d_finished = false;
    d_thread = std::make_unique<boost::thread>([this] { readloop(); });
    return true;
}

bool rep_msg_sink_impl::stop()
{
    d_finished = true;
    d_thread->join();
    return true;
}

void rep_msg_sink_impl::readloop()
{
    while (!d_finished) {

        // while we have data, wait for query...
        while (!empty_p(d_port)) {

            // wait for query...
            zmq::pollitem_t items[] = {
                { static_cast<void*>(d_socket), 0, ZMQ_POLLIN, 0 }
            };
            zmq::poll(&items[0], 1, d_timeout);

            //  If we got a reply, process
            if (items[0].revents & ZMQ_POLLIN) {

                // receive data request
                zmq::message_t request;
#if USE_NEW_CPPZMQ_SEND_RECV
                const bool ok = bool(d_socket.recv(request));
#else
                const bool ok = d_socket.recv(&request);
#endif
                if (!ok) {
                    // Should not happen, we've checked POLLIN.
                    GR_LOG_ERROR(d_logger, "Failed to receive message.");
                    break; // Fall back to re-check d_finished
                }

                int req_output_items = *(static_cast<int*>(request.data()));
                if (req_output_items != 1)
                    throw std::runtime_error(
                        "Request was not 1 msg for rep/req request!!");

                // create message copy and send
                pmt::pmt_t msg = delete_head_nowait(d_port);
                std::stringbuf sb("");
                pmt::serialize(msg, sb);
                std::string s = sb.str();
                zmq::message_t zmsg(s.size());
                memcpy(zmsg.data(), s.c_str(), s.size());
#if USE_NEW_CPPZMQ_SEND_RECV
                d_socket.send(zmsg, zmq::send_flags::none);
#else
                d_socket.send(zmsg);
#endif
            } // if req
        }     // while !empty

    } // while !d_finished
}

} /* namespace zeromq */
} /* namespace gr */

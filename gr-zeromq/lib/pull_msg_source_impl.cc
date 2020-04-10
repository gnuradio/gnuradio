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

#include "pull_msg_source_impl.h"
#include "tag_headers.h"
#include <gnuradio/io_signature.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

namespace gr {
namespace zeromq {

pull_msg_source::sptr pull_msg_source::make(char* address, int timeout, bool bind)
{
    return gnuradio::get_initial_sptr(new pull_msg_source_impl(address, timeout, bind));
}

pull_msg_source_impl::pull_msg_source_impl(char* address, int timeout, bool bind)
    : gr::block("pull_msg_source",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(0, 0, 0)),
      d_timeout(timeout),
      d_port(pmt::mp("out"))
{
    int major, minor, patch;
    zmq::version(&major, &minor, &patch);

    if (major < 3) {
        d_timeout = timeout * 1000;
    }

    d_context = new zmq::context_t(1);
    d_socket = new zmq::socket_t(*d_context, ZMQ_PULL);

    int time = 0;
    d_socket->setsockopt(ZMQ_LINGER, &time, sizeof(time));

    if (bind) {
        d_socket->bind(address);
    } else {
        d_socket->connect(address);
    }

    message_port_register_out(d_port);
}

pull_msg_source_impl::~pull_msg_source_impl()
{
    d_socket->close();
    delete d_socket;
    delete d_context;
}

bool pull_msg_source_impl::start()
{
    d_finished = false;
    d_thread = new boost::thread(boost::bind(&pull_msg_source_impl::readloop, this));
    return true;
}

bool pull_msg_source_impl::stop()
{
    d_finished = true;
    d_thread->join();
    return true;
}

void pull_msg_source_impl::readloop()
{
    while (!d_finished) {

        zmq::pollitem_t items[] = { { static_cast<void*>(*d_socket), 0, ZMQ_POLLIN, 0 } };
        zmq::poll(&items[0], 1, d_timeout);

        //  If we got a reply, process
        if (items[0].revents & ZMQ_POLLIN) {

            // Receive data
            zmq::message_t msg;
#if USE_NEW_CPPZMQ_SEND_RECV
            d_socket->recv(msg);
#else
            d_socket->recv(&msg);
#endif

            std::string buf(static_cast<char*>(msg.data()), msg.size());
            std::stringbuf sb(buf);
            try {
                pmt::pmt_t m = pmt::deserialize(sb);
                message_port_pub(d_port, m);
            } catch (pmt::exception& e) {
                GR_LOG_ERROR(d_logger, std::string("Invalid PMT message: ") + e.what());
            }

        } else {
            boost::this_thread::sleep(boost::posix_time::microseconds(100));
        }
    }
}

} /* namespace zeromq */
} /* namespace gr */

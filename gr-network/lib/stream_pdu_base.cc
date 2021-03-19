/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_IO_H
#include <io.h>
#endif

#ifdef HAVE_WINDOWS_H
#include <winsock2.h>
#endif

#include "stream_pdu_base.h"
#include <gnuradio/basic_block.h>
#include <gnuradio/logger.h>
#include <gnuradio/pdu.h>
#include <boost/format.hpp>

static const long timeout_us = 100 * 1000; // 100ms

namespace gr {
namespace network {

stream_pdu_base::stream_pdu_base(int MTU) : d_fd(-1), d_started(false), d_finished(false)
{
    gr::configure_default_loggers(d_pdu_logger, d_pdu_debug_logger, "stream_pdu_base");
    // reserve space for rx buffer
    d_rxbuf.resize(MTU, 0);
}

stream_pdu_base::~stream_pdu_base() { stop_rxthread(); }

void stream_pdu_base::start_rxthread(basic_block* blk, pmt::pmt_t port)
{
    d_blk = blk;
    d_port = port;
    d_thread = gr::thread::thread([this] { run(); });
    d_started = true;
}

void stream_pdu_base::stop_rxthread()
{
    d_finished = true;

    if (d_started) {
        d_thread.interrupt();
        d_thread.join();
    }
}

void stream_pdu_base::run()
{
    while (!d_finished) {
        if (!wait_ready())
            continue;

        const int result = read(d_fd, &d_rxbuf[0], d_rxbuf.size());
        if (result <= 0)
            throw std::runtime_error("stream_pdu_base, bad socket read!");

        pmt::pmt_t vector = pmt::init_u8vector(result, &d_rxbuf[0]);
        pmt::pmt_t pdu = pmt::cons(pmt::PMT_NIL, vector);

        d_blk->message_port_pub(d_port, pdu);
    }
}

bool stream_pdu_base::wait_ready()
{
    // setup timeval for timeout
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout_us;

    // setup rset for timeout
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(d_fd, &rset);

    // call select with timeout on receive socket
    return ::select(d_fd + 1, &rset, NULL, NULL, &tv) > 0;
}

void stream_pdu_base::send(pmt::pmt_t msg)
{
    pmt::pmt_t vector = pmt::cdr(msg);
    size_t offset(0);
    size_t itemsize(pdu::itemsize(pdu::type_from_pmt(vector)));
    int len(pmt::length(vector) * itemsize);

    const int rv = write(d_fd, pmt::uniform_vector_elements(vector, offset), len);
    if (rv != len) {
        static auto msg = boost::format(
            "stream_pdu_base::send(pdu) write failed! (d_fd=%d, len=%d, rv=%d)");
        GR_LOG_WARN(d_pdu_logger, msg % d_fd % len % rv);
    }
}

} /* namespace network */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_STREAM_PDU_BASE_H
#define INCLUDED_STREAM_PDU_BASE_H

#include <gnuradio/basic_block.h>
#include <gnuradio/logger.h>
#include <gnuradio/thread/thread.h>
#include <pmt/pmt.h>

class basic_block;

namespace gr {
namespace network {

class stream_pdu_base
{
public:
    stream_pdu_base(int MTU = 10000);
    ~stream_pdu_base();

protected:
    int d_fd;
    bool d_started;
    bool d_finished;
    std::vector<uint8_t> d_rxbuf;
    gr::thread::thread d_thread;

    pmt::pmt_t d_port;
    basic_block* d_blk;

    void run();
    void send(pmt::pmt_t msg);
    bool wait_ready();
    void start_rxthread(basic_block* blk, pmt::pmt_t rxport);
    void stop_rxthread();

    gr::logger_ptr d_pdu_logger, d_pdu_debug_logger;
};

} /* namespace network */
} /* namespace gr */

#endif /* INCLUDED_STREAM_PDU_BASE_H */

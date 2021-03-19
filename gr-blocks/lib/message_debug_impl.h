/* -*- c++ -*- */
/*
 * Copyright 2005,2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_MESSAGE_DEBUG_IMPL_H
#define INCLUDED_GR_MESSAGE_DEBUG_IMPL_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/message_debug.h>
#include <gnuradio/thread/thread.h>
#include <pmt/pmt.h>

namespace gr {
namespace blocks {

class message_debug_impl : public message_debug
{
private:
    bool d_en_uvec;

    /*!
     * \brief Messages received in this port are printed to stdout.
     *
     * This port receives messages from the scheduler's message
     * handling mechanism and prints it to stdout. This message
     * handler function is only meant to be used by the scheduler to
     * handle messages posted to port 'print'. If the message is a
     * PDU, special formatting will be applied.
     *
     * \param msg A pmt message passed from the scheduler's message handling.
     */
    void print(const pmt::pmt_t& msg);

    /*!
     * \brief PDU formatted messages received in this port are printed to stdout.
     *
     * DEPRECATED as of 3.10 use print() for all printing!
     *
     * This port receives messages from the scheduler's message
     * handling mechanism and prints it to stdout. This message
     * handler function is only meant to be used by the scheduler to
     * handle messages posted to port 'print'.
     *
     * \param pdu A PDU message passed from the scheduler's message handling.
     */
    void print_pdu(const pmt::pmt_t& pdu);

    /*!
     * \brief Messages received in this port are stored in a vector.
     *
     * This port receives messages from the scheduler's message
     * handling mechanism and stores it in a vector. Messages can be
     * retrieved later using the 'get_message' function. This
     * message handler function is only meant to be used by the
     * scheduler to handle messages posted to port 'store'.
     *
     * \param msg A pmt message passed from the scheduler's message handling.
     */
    void store(const pmt::pmt_t& msg);

    gr::thread::mutex d_mutex;
    std::vector<pmt::pmt_t> d_messages;

public:
    message_debug_impl(bool en_uvec);
    ~message_debug_impl() override;

    size_t num_messages() override;
    pmt::pmt_t get_message(size_t i) override;
    void set_vector_print(bool en) override { d_en_uvec = en; };
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_DEBUG_IMPL_H */

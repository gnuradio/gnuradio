/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "message_debug_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <iostream>

namespace gr {
namespace blocks {

message_debug::sptr message_debug::make()
{
    return gnuradio::get_initial_sptr(new message_debug_impl());
}

void message_debug_impl::print(pmt::pmt_t msg)
{
    std::cout << "******* MESSAGE DEBUG PRINT ********\n";
    pmt::print(msg);
    std::cout << "************************************\n";
}

void message_debug_impl::store(pmt::pmt_t msg)
{
    gr::thread::scoped_lock guard(d_mutex);
    d_messages.push_back(msg);
}

void message_debug_impl::print_pdu(pmt::pmt_t pdu)
{
    pmt::pmt_t meta = pmt::car(pdu);
    pmt::pmt_t vector = pmt::cdr(pdu);
    std::cout << "* MESSAGE DEBUG PRINT PDU VERBOSE *\n";
    pmt::print(meta);
    size_t len = pmt::blob_length(vector);
    std::cout << "pdu_length = " << len << std::endl;
    std::cout << "contents = " << std::endl;
    size_t offset(0);
    const uint8_t* d = (const uint8_t*)pmt::uniform_vector_elements(vector, offset);
    for (size_t i = 0; i < len; i += 16) {
        printf("%04x: ", ((unsigned int)i));
        for (size_t j = i; j < std::min(i + 16, len); j++) {
            printf("%02x ", d[j]);
        }

        std::cout << std::endl;
    }

    std::cout << "***********************************\n";
}

int message_debug_impl::num_messages() { return (int)d_messages.size(); }

pmt::pmt_t message_debug_impl::get_message(int i)
{
    gr::thread::scoped_lock guard(d_mutex);

    if ((size_t)i >= d_messages.size()) {
        throw std::runtime_error("message_debug: index for message out of bounds.");
    }

    return d_messages[i];
}

message_debug_impl::message_debug_impl()
    : block("message_debug", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0))
{
    message_port_register_in(pmt::mp("print"));
    set_msg_handler(pmt::mp("print"), boost::bind(&message_debug_impl::print, this, _1));

    message_port_register_in(pmt::mp("store"));
    set_msg_handler(pmt::mp("store"), boost::bind(&message_debug_impl::store, this, _1));

    message_port_register_in(pmt::mp("print_pdu"));
    set_msg_handler(pmt::mp("print_pdu"),
                    boost::bind(&message_debug_impl::print_pdu, this, _1));
}

message_debug_impl::~message_debug_impl() {}

} /* namespace blocks */
} /* namespace gr */

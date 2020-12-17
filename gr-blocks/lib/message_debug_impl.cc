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

message_debug::sptr message_debug::make(bool en_uvec)
{
    return gnuradio::make_block_sptr<message_debug_impl>(en_uvec);
}

message_debug_impl::message_debug_impl(bool en_uvec)
    : block("message_debug", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_en_uvec(en_uvec)
{
    message_port_register_in(pmt::mp("print"));
    set_msg_handler(pmt::mp("print"), [this](pmt::pmt_t msg) { this->print(msg); });

    message_port_register_in(pmt::mp("store"));
    set_msg_handler(pmt::mp("store"), [this](pmt::pmt_t msg) { this->store(msg); });

    message_port_register_in(pmt::mp("print_pdu"));
    set_msg_handler(pmt::mp("print_pdu"),
                    [this](pmt::pmt_t msg) { this->print_pdu(msg); });
}

message_debug_impl::~message_debug_impl() {}

void message_debug_impl::print(pmt::pmt_t msg)
{
    std::stringstream sout;

    sout << "******* MESSAGE DEBUG PRINT ********" << std::endl
         << pmt::write_string(msg) << std::endl
         << "************************************" << std::endl;
    std::cout << sout.str();
}

void message_debug_impl::store(pmt::pmt_t msg)
{
    gr::thread::scoped_lock guard(d_mutex);
    d_messages.push_back(msg);
}

void message_debug_impl::print_pdu(pmt::pmt_t pdu)
{
    if (pmt::is_null(pdu) || !pmt::is_pair(pdu)) {
        return;
    }

    std::stringstream sout;

    pmt::pmt_t meta = pmt::car(pdu);
    pmt::pmt_t vector = pmt::cdr(pdu);

    sout << "***** VERBOSE PDU DEBUG PRINT ******" << std::endl
         << pmt::write_string(meta) << std::endl;

    if (pmt::is_uniform_vector(vector)) {
        size_t len = pmt::blob_length(vector);
        if (d_en_uvec) {
            sout << "pdu length = " << len << " bytes" << std::endl
                 << "pdu vector contents = " << std::endl;
            size_t offset(0);
            const uint8_t* d =
                (const uint8_t*)pmt::uniform_vector_elements(vector, offset);
            for (size_t i = 0; i < len; i += 16) {
                sout << std::hex << std::setw(4) << std::setfill('0')
                     << static_cast<unsigned int>(i) << ": ";
                for (size_t j = i; j < std::min(i + 16, len); j++) {
                    sout << std::hex << std::setw(2) << std::setfill('0')
                         << static_cast<unsigned int>(d[j]) << " ";
                }
                sout << std::endl;
            }
        } else {
            sout << "pdu length = " << len << " bytes (printing disabled)" << std::endl;
        }
    } else {
        sout << "Message CDR is not a uniform vector..." << std::endl;
    }

    sout << "************************************" << std::endl;
    std::cout << sout.str();
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

} /* namespace blocks */
} /* namespace gr */

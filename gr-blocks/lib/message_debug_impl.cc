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
    set_msg_handler(pmt::mp("print"),
                    [this](const pmt::pmt_t& msg) { this->print(msg); });

    message_port_register_in(pmt::mp("store"));
    set_msg_handler(pmt::mp("store"),
                    [this](const pmt::pmt_t& msg) { this->store(msg); });

    message_port_register_in(pmt::mp("print_pdu"));
    set_msg_handler(pmt::mp("print_pdu"),
                    [this](const pmt::pmt_t& msg) { this->print_pdu(msg); });
}

message_debug_impl::~message_debug_impl() {}

void message_debug_impl::print(const pmt::pmt_t& msg)
{
    std::stringstream sout;

    if (pmt::is_pdu(msg)) {
        const auto& meta = pmt::car(msg);
        const auto& vector = pmt::cdr(msg);

        sout << "***** VERBOSE PDU DEBUG PRINT ******" << std::endl
             << pmt::write_string(meta) << std::endl;

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
        sout << "******* MESSAGE DEBUG PRINT ********" << std::endl
             << pmt::write_string(msg) << std::endl;
    }

    sout << "************************************" << std::endl;
    std::cout << sout.str();
}

void message_debug_impl::store(const pmt::pmt_t& msg)
{
    gr::thread::scoped_lock guard(d_mutex);
    d_messages.push_back(msg);
}

// ! DEPRECATED as of 3.10 use print() for all printing!
void message_debug_impl::print_pdu(const pmt::pmt_t& pdu)
{
    if (pmt::is_pdu(pdu)) {
        GR_LOG_INFO(d_logger,
                    "The `print_pdu` port is deprecated and will be removed; forwarding "
                    "to `print`.");
        this->print(pdu);
    } else {
        GR_LOG_INFO(d_logger, "The `print_pdu` port is deprecated and will be removed.");
        GR_LOG_WARN(d_logger, "Non PDU type message received. Dropping.");
    }
}

size_t message_debug_impl::num_messages()
{
    gr::thread::scoped_lock guard(d_mutex);
    return d_messages.size();
}

pmt::pmt_t message_debug_impl::get_message(size_t i)
{
    gr::thread::scoped_lock guard(d_mutex);

    if (i >= d_messages.size()) {
        throw std::runtime_error("message_debug: index for message out of bounds.");
    }

    return d_messages[i];
}

} /* namespace blocks */
} /* namespace gr */

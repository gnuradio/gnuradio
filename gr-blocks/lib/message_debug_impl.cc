/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2012-2013 Free Software Foundation, Inc.
 * Copyright 2021 Marcus Müller
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
#include <spdlog/fmt/fmt.h>

namespace gr {
namespace blocks {

// Original code was inconsistent about line lengths, but 36 comes close to the average
// old *** HEADING *** line length.
constexpr unsigned int PRINT_LINE_LENGTH = 36;

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
    if (pmt::is_pdu(msg)) {
        const auto& meta = pmt::car(msg);
        const auto& vector = pmt::cdr(msg);

        fmt::print("{:*^{}}\n"
                   "{:s}\n",
                   " VERBOSE PDU DEBUG PRINT ",
                   PRINT_LINE_LENGTH,
                   pmt::write_string(meta));

        size_t len = pmt::blob_length(vector);
        if (d_en_uvec) {
            fmt::print("pdu length = {:10d} bytes\n"
                       "pdu vector contents = \n",
                       len);
            size_t offset(0);
            const uint8_t* d =
                (const uint8_t*)pmt::uniform_vector_elements(vector, offset);
            for (size_t i = 0; i < len; i += 16) {
                fmt::print("{:04x}: ", static_cast<unsigned int>(i));
                for (size_t j = i; j < std::min(i + 16, len); j++) {
                    fmt::print("{:02x} ", d[j]);
                }
                fmt::print("\n");
            }
        } else {
            fmt::print("pdu length = {:10d} bytes (printing disabled)\n", len);
        }

    } else {
        fmt::print("{:*^{}}\n"
                   "{:s}\n",
                   " MESSAGE DEBUG PRINT ",
                   PRINT_LINE_LENGTH,
                   pmt::write_string(msg));
    }

    fmt::print("{:*^{}}\n", "", PRINT_LINE_LENGTH);
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
        d_logger->info("The `print_pdu` port is deprecated and will be removed; "
                       "forwarding to `print`.");
        this->print(pdu);
    } else {
        d_logger->info("The `print_pdu` port is deprecated and will be removed.");
        d_logger->warn("Non PDU type message received. Dropping.");
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

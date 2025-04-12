/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2012-2013 Free Software Foundation, Inc.
 * Copyright 2021,2022 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "message_debug_impl.h"
#include <gnuradio/io_signature.h>

/* ensure that tweakme.h is included before the bundled spdlog/fmt header, see
 * https://github.com/gabime/spdlog/issues/2922 */
#include <spdlog/tweakme.h>

#include <pmt/pmt.h>
#include <spdlog/common.h>
#include <spdlog/fmt/fmt.h>
#if __has_include(<spdlog/fmt/ranges.h>)
#include <spdlog/fmt/ranges.h>
#elif __has_include(<spdlog/fmt/bundled/ranges.h>)
#include <spdlog/fmt/bundled/ranges.h>
#endif
#include <functional>
#include <utility>
#include <vector>

namespace gr {
namespace blocks {

// Original code was inconsistent about line lengths, but 36 comes close to the average
// old *** HEADING *** line length.
constexpr unsigned int PRINT_LINE_LENGTH = 36;

message_debug::sptr message_debug::make(bool en_uvec, spdlog::level::level_enum log_level)
{
    return gnuradio::make_block_sptr<message_debug_impl>(en_uvec, log_level);
}

message_debug_impl::message_debug_impl(bool en_uvec, spdlog::level::level_enum log_level)
    : block("message_debug", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_en_uvec(en_uvec),
      d_level(log_level)
{
    using msg_t = const pmt::pmt_t&;
    std::vector<std::pair<std::string, std::function<void(msg_t)>>> mapping{
        { "log", [&](msg_t m) { log(m); } },
        { "print", [&](msg_t m) { print(m); } },
        { "print_pdu", [&](msg_t m) { print_pdu(m); } },
        { "store", [&](msg_t m) { store(m); } }
    };
    for (const auto& [name, handler] : mapping) {
        const auto symbol = pmt::mp(name);
        message_port_register_in(symbol);
        set_msg_handler(symbol, handler);
    }
}

message_debug_impl::~message_debug_impl() {}

void message_debug_impl::log(const pmt::pmt_t& msg)
{
    if (pmt::is_pdu(msg)) {
        const auto& meta = pmt::car(msg);
        const auto& vector = pmt::cdr(msg);
        auto length = pmt::blob_length(vector);
        auto begin = reinterpret_cast<const uint8_t*>(pmt::blob_data(vector));
        auto output_length = length;
        if (!d_en_uvec)
            output_length = std::min<decltype(length)>(output_length, 16);
        auto datastring = fmt::format("{:X}{}",
                                      fmt::join(begin, begin + output_length, " "),
                                      output_length < length ? " …" : "");
        d_logger->log(d_level,
                      "PDU {:s}: {:10d} B [{}] ",
                      pmt::write_string(meta),
                      length,
                      datastring);
        return;
    }
    d_logger->log(d_level, "Message: {}", pmt::write_string(msg));
}
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

spdlog::level::level_enum message_debug_impl::level() const { return d_level; }
void message_debug_impl::set_level(spdlog::level::level_enum level) { d_level = level; }

} /* namespace blocks */
} /* namespace gr */

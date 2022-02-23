/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_UHD_COMMON_H
#define INCLUDED_GR_UHD_COMMON_H

#include <uhd/stream.hpp>
#include <uhd/version.hpp>
#include <stdexcept>

namespace gr {
namespace uhd {

static inline void check_abi(void)
{
#ifdef UHD_VERSION_ABI_STRING
    if (std::string(UHD_VERSION_ABI_STRING) == ::uhd::get_abi_string())
        return;

    throw std::runtime_error(
        "\nGR-UHD detected ABI compatibility mismatch with UHD library.\n"
        "GR-UHD was built against ABI: " +
        std::string(UHD_VERSION_ABI_STRING) +
        ",\nbut UHD library reports ABI: " + ::uhd::get_abi_string() +
        "\nSuggestion: install an ABI compatible version of UHD,\n"
        "or rebuild GR-UHD component against this ABI version.\n");
#endif
}

} /* namespace uhd */
} /* namespace gr */

/*!
 * The stream args ensure function sanitizes random user input.
 * We may extend this to handle more things in the future,
 * but ATM it ensures that the channels are initialized.
 */
static inline uhd::stream_args_t stream_args_ensure(const uhd::stream_args_t& args)
{
    uhd::stream_args_t sanitized = args;
    if (sanitized.channels.empty()) {
        sanitized.channels.push_back(0);
    }
    return sanitized;
}

#endif /* INCLUDED_GR_UHD_COMMON_H */

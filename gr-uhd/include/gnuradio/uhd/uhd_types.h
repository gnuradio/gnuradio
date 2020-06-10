/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* Wrappers for UHD types that exposed in the API of gr-uhd blocks */
#ifndef INCLUDED_GR_UHD_TYPES_H
#define INCLUDED_GR_UHD_TYPES_H

#include <uhd/stream.hpp>
#include <uhd/types/filters.hpp>
#include <uhd/types/ranges.hpp>
#include <uhd/types/sensors.hpp>
#include <uhd/types/stream_cmd.hpp>
#include <uhd/types/tune_request.hpp>
#include <uhd/types/tune_result.hpp>
#include <uhd/types/wb_iface.hpp>

namespace gr {
namespace uhd {

// typedef device_addr_t ::uhd::device_addr_t
class stream_args_t : public ::uhd::stream_args_t
{

public:
    stream_args_t(const std::string& cpu = "",
                  const std::string& otw = "",
                  const ::uhd::device_addr_t& args = ::uhd::device_addr_t(),
                  const std::vector<size_t> channels = std::vector<size_t>())
        : ::uhd::stream_args_t(cpu, otw)
    {
        this->args = args;
        this->channels = channels;
    }
};
} // namespace uhd
} // namespace gr


#endif
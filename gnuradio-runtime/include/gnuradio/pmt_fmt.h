/* -*- c++ -*- */
/*
 * Copyright 2023 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef INCLUDED_GNURADIO_PMT_FMT_H
#define INCLUDED_GNURADIO_PMT_FMT_H
/* ensure that tweakme.h is included before the bundled spdlog/fmt header, see
 * https://github.com/gabime/spdlog/issues/2922 */
#include <spdlog/tweakme.h>

#include <gnuradio/api.h>
#include <pmt/pmt.h>
#include <spdlog/fmt/fmt.h>
#include <string_view>

//!\brief enables PMTs to be formatted with fmt
template <>
struct GR_RUNTIME_API fmt::formatter<pmt::pmt_t> : formatter<std::string_view> {
    fmt::format_context::iterator format(const pmt::pmt_t& obj,
                                         format_context& ctx) const;
};
#endif

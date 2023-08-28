/* -*- c++ -*- */
/*
 * Copyright 2023 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include <gnuradio/pmt_fmt.h>
#include <pmt/pmt.h>
#include <spdlog/fmt/fmt.h>
#include <string_view>

fmt::format_context::iterator
fmt::formatter<pmt::pmt_t>::format(const pmt::pmt_t& obj, fmt::format_context& ctx) const
{
    return fmt::format_to(ctx.out(), "{}", pmt::write_string(obj));
}

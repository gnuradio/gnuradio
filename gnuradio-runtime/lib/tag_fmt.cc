/* -*- c++ -*- */
/*
 * Copyright 2023, 2025 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* ensure that tweakme.h is included before the bundled spdlog/fmt header, see
 * https://github.com/gabime/spdlog/issues/2922 */
#include <spdlog/tweakme.h>

#include <spdlog/fmt/fmt.h>

#include <gnuradio/pmt_fmt.h>
#include <gnuradio/tags.h>

fmt::format_context::iterator
fmt::formatter<gr::tag_t>::format(const gr::tag_t& tag, fmt::format_context& ctx) const
{
    return fmt::format_to(
        ctx.out(), "@{} {{'{}': '{}'}} ⌱{}", tag.offset, tag.key, tag.value, tag.srcid);
}

/*
 * Copyright 2012, 2018 Free Software Foundation, Inc.
 * Copyright 2025 Marcus Müller <mmueller@gnuradio.org>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pmt/pmt.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/logger.h>
#include <gnuradio/pmt_fmt.h>
#include <gnuradio/tags.h>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <string>

using gr::tag_t;
BOOST_AUTO_TEST_CASE(t0_io_signature_formatter)
{
    auto sig = gr::io_signature::make(0, 10, { 1, 2, 23, 42 });
    const std::string output = fmt::format("{}", *sig);
    const std::string reference = "Stream Range:                0 – ∞\n"
                                  "Item Sizes:        1, 2, 23, 42, …\n"
                                  "Buffer Types: printing not implemented";
    BOOST_CHECK(output == reference);
}

BOOST_AUTO_TEST_CASE(t1_pmt_formatter)
{
    auto input = pmt::mp("Foobar");
    std::string output = fmt::format("{}", input);
    BOOST_CHECK(output == "Foobar");
}
BOOST_AUTO_TEST_CASE(t2_tag_formtter)
{
    const std::string key_string{ "This is a test string. GNU Radio." };
    const std::string src_string{ "Test source." };
    const int value = 42;
    const uint64_t offset = 13371337;
    tag_t tag{
        .offset = offset,
        .key = pmt::mp(key_string),
        .value = pmt::from_long(value),
        .srcid = pmt::mp(src_string),
    };
    const std::string output = fmt::format("{}", tag);
    const std::string reference = "@13371337 {'" + key_string + "': '42'} ⌱" + src_string;
    BOOST_CHECK(output == reference);
}

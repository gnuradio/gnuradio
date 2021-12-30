/* -*- c++ -*- */
/*
 * Copyright 2021 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include <gnuradio/stream_iterators.h>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(iterators)
{
    size_t n_streams = 10;
    size_t n_items = 25;
    std::vector<int*> in_streams(n_streams);
    int val = 1;
    for (auto& in_stream : in_streams) {
        in_stream = new int[n_items];
        for (unsigned idx = 0; idx < n_items; ++idx) {
            in_stream[idx] = val++;
        }
        val += 100;
    }
    gr::stream_span<int*> my_str_span{ in_streams.data(), in_streams.size(), n_items };
    val = 1;
    for (const auto& this_stream_span : my_str_span) {
        for (const auto& item : this_stream_span) {
            BOOST_CHECK(item == val++);
        }
        val += 100;
    }
    val = 1;
    for (size_t stream_idx = 0; stream_idx < my_str_span.size(); ++stream_idx) {
        auto& input = my_str_span[stream_idx];
        for (size_t item_idx = 0; item_idx < my_str_span.nitems; ++item_idx) {
            BOOST_CHECK(input[item_idx] == val++);
        }
        val += 100;
    }
}

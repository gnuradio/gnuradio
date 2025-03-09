/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 * Copyright 2025 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "patterned_interleaver_impl.h"
#include <gnuradio/io_signature.h>
#include <algorithm>

namespace gr {
namespace blocks {

patterned_interleaver::sptr patterned_interleaver::make(size_t itemsize,
                                                        std::vector<int> pattern)
{
    return gnuradio::make_block_sptr<patterned_interleaver_impl>(itemsize, pattern);
}

namespace {
template <typename T>
constexpr typename T::value_type pattern_max(const T& pattern)
{
    if (pattern.empty()) {
        return -1;
    }
    return *std::max_element(pattern.cbegin(), pattern.cend());
}

template <typename T>
auto pattern_to_iosig(const T& pattern, size_t itemsize)
{
    if (pattern.empty()) {
        return gr::io_signature::make(0, 0, itemsize);
    }
    auto size = pattern_max(pattern) + 1;
    return gr::io_signature::make(size, gr::io_signature::IO_INFINITE, itemsize);
}

template <typename T>
constexpr std::vector<typename T::value_type> pattern_to_count(const T& pattern)
{
    if (pattern.empty()) {
        return {};
    }
    T counts(pattern_max(pattern) + 1);
    for (const auto& idx : pattern) {
        counts[idx]++;
    }
    return counts;
}
} // namespace

patterned_interleaver_impl::patterned_interleaver_impl(size_t itemsize,
                                                       std::vector<int> pattern)
    : block("patterned_interleaver",
            pattern_to_iosig(pattern, itemsize),
            io_signature::make(1, 1, itemsize)),
      d_pattern(pattern),
      d_counts(pattern_to_count(pattern)),
      d_itemsize(itemsize)
{
    set_output_multiple(d_pattern.size());
}

int patterned_interleaver_impl::general_work(int noutput_items,
                                             gr_vector_int& ninput_items,
                                             gr_vector_const_void_star& input_items,
                                             gr_vector_void_star& output_items)
{
    int items_left;
    unsigned int nblks;

    for (items_left = noutput_items, nblks = 0; items_left > 0;
         items_left -= d_pattern.size(), nblks++) {
        for (const auto input_idx : d_pattern) {
            memcpy(output_items[0], input_items[input_idx], d_itemsize);
            reinterpret_cast<const char*&>(input_items[input_idx]) += d_itemsize;
            reinterpret_cast<char*&>(output_items[0]) += d_itemsize;
        }
    }

    for (size_t i = 0; i < d_counts.size(); i++) {
        consume(i, d_counts[i] * nblks);
    }
    return nblks * d_pattern.size();
}

void patterned_interleaver_impl::forecast(int noutput_items,
                                          gr_vector_int& ninput_items_required)
{
    const int nblks = noutput_items / d_pattern.size();
    for (size_t i = 0; i < ninput_items_required.size(); i++) {
        ninput_items_required[i] = d_counts[i] * nblks;
    }
}

bool patterned_interleaver_impl::check_topology(int ninput, int noutput)
{
    if (noutput != 1) {
        return false;
    }
    /* it's not OK to have fewer inputs connected than our maximum input index, but more
     * might be OK */
    return static_cast<size_t>(ninput) >= d_counts.size();
}

} /* namespace blocks */
} /* namespace gr */

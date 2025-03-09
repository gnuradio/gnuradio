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

namespace gr {
namespace blocks {

patterned_interleaver::sptr patterned_interleaver::make(size_t itemsize,
                                                        std::vector<int> pattern)
{
    return gnuradio::make_block_sptr<patterned_interleaver_impl>(itemsize, pattern);
}

patterned_interleaver_impl::patterned_interleaver_impl(size_t itemsize,
                                                       std::vector<int> pattern)
    : block("patterned_interleaver",
            io_signature::make(
                pattern_max(pattern) + 1, pattern_max(pattern) + 1, itemsize),
            io_signature::make(1, 1, itemsize)),
      d_pattern(pattern),
      d_counts(pattern_max(pattern) + 1, 0),
      d_itemsize(itemsize)
{
    for (const auto& i : d_pattern) {
        d_counts[i]++;
    }
    set_output_multiple(d_pattern.size());
}

int patterned_interleaver_impl::general_work(int noutput_items,
                                             gr_vector_int& ninput_items,
                                             gr_vector_const_void_star& input_items,
                                             gr_vector_void_star& output_items)
{
    const size_t nblks = noutput_items / d_pattern.size();

    for (size_t i = 0; i < nblks; i++) {
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

} /* namespace blocks */
} /* namespace gr */

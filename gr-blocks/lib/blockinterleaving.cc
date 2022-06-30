/* -*- c++ -*- */
/*
 * Copyright 2022 Johannes Demel.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/blocks/blockinterleaving.h>
#include <spdlog/fmt/fmt.h>
#include <algorithm>
#include <numeric>

namespace gr {
namespace blocks {
namespace kernel {

block_interleaving::block_interleaving(std::vector<size_t> interleaver_indices)
{
    set_interleaver_indices(interleaver_indices);
}


void block_interleaving::set_interleaver_indices(
    const std::vector<size_t>& interleaver_indices)
{
    std::vector<size_t> test(interleaver_indices);
    std::sort(test.begin(), test.end());
    std::unique(test.begin(), test.end());
    const auto [min, max] = std::minmax_element(test.begin(), test.end());
    if (test.size() != interleaver_indices.size() || *min != 0 ||
        *max != interleaver_indices.size() - 1) {
        throw std::invalid_argument(
            fmt::format("Incorrect interleaver indices! Expected {} unique elements in "
                        "the range[0, {}) "
                        "in random order, but got {} elements in range [{}, {})!",
                        interleaver_indices.size(),
                        interleaver_indices.size(),
                        test.size(),
                        *min,
                        *max + 1));
    }

    _interleaver_indices = std::vector<size_t>(interleaver_indices);

    _deinterleaver_indices.resize(_interleaver_indices.size());
    std::iota(_deinterleaver_indices.begin(), _deinterleaver_indices.end(), 0);
    // sort indexes based on comparing values in v
    std::sort(_deinterleaver_indices.begin(),
              _deinterleaver_indices.end(),
              [&interleaver_indices](size_t i1, size_t i2) {
                  return interleaver_indices[i1] < interleaver_indices[i2];
              });
}

} /* namespace kernel */
} /* namespace blocks */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2022 Johannes Demel.
 * Copyright 2024 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/blocks/blockinterleaving.h>

/* ensure that tweakme.h is included before the bundled spdlog/fmt header, see
 * https://github.com/gabime/spdlog/issues/2922 ; fixed upstream in spdlog v1.13.0 */
#include <spdlog/tweakme.h>

#include <spdlog/fmt/fmt.h>

#include <stdexcept>
#include <vector>

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
    if (interleaver_indices.empty()) {
        throw std::invalid_argument("Incorrect interleaver indices! Can't be empty.");
    }

    std::vector<size_t> deinterleaver_indices(interleaver_indices.size(), 0);
    bool zero_seen = false;
    for (size_t counter = 0; counter < interleaver_indices.size(); ++counter) {
        const auto index = interleaver_indices[counter];

        // check bounds
        if (index >= interleaver_indices.size()) {
            throw std::invalid_argument(fmt::format(
                "Incorrect interleaver indices! Expected {0} unique elements in the "
                "range[0, {0}) in random order, but element {1} >= {0}!",
                interleaver_indices.size(),
                index));
        }

        // check for duplicate zero
        if (index == 0) {
            if (zero_seen) {
                throw std::invalid_argument(fmt::format(
                    "Incorrect interleaver indices! Expected {0} unique elements in the "
                    "range[0, {0}) in random order, but element {1} appears twice, at "
                    "positions {2} and {3}!",
                    interleaver_indices.size(),
                    index,
                    deinterleaver_indices[index],
                    counter));
            }
            zero_seen = true;
            deinterleaver_indices[0] = counter;
        }

        // put into deinterleaver, check duplicity while doing that
        else if (deinterleaver_indices[index] == 0) { // not yet used
            deinterleaver_indices[index] = counter;
        } else {
            throw std::invalid_argument(fmt::format(
                "Incorrect interleaver indices! Expected {0} unique elements in the "
                "range[0, {0}) in random order, but element {1} appears twice, at "
                "positions {2} and {3}!",
                interleaver_indices.size(),
                index,
                deinterleaver_indices[index],
                counter));
        }
    }

    /* At this point, we have inserted all N elements from interleaver_indices into our
     * deinterleaver_indices, no duplicates occurred during processing of N elements, and
     * none was >= N, which implies the range is unique and consecutive from 0 to N-1.
     */
    _interleaver_indices = interleaver_indices;
    _deinterleaver_indices = std::move(deinterleaver_indices);
}

} /* namespace kernel */
} /* namespace blocks */
} /* namespace gr */

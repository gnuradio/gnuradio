/* -*- c++ -*- */
/*
 * Copyright 2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/filter/filterbank.h>
#include <cstdio>
#include <stdexcept>

namespace gr {
namespace filter {
namespace kernel {

filterbank::filterbank(const std::vector<std::vector<float>>& taps)
    : d_taps(taps), d_nfilts(d_taps.size())
{
    if (d_nfilts == 0) {
        throw std::invalid_argument("The taps vector may not be empty.");
    }
    d_active.resize(d_nfilts);
    // Create an FIR filter for each channel and zero out the taps
    std::vector<float> vtaps(1, 0.0f);
    d_fir_filters.reserve(d_nfilts);
    for (unsigned int i = 0; i < d_nfilts; i++) {
        d_fir_filters.emplace_back(vtaps);
    }
    // Now, actually set the filters' taps
    set_taps(d_taps);
}

void filterbank::set_taps(const std::vector<std::vector<float>>& taps)
{
    // Check that the number of filters is correct.
    if (d_nfilts != taps.size()) {
        throw std::runtime_error("The number of filters is incorrect.");
    }
    d_taps = taps;
    // Check that taps contains vectors of taps, where each vector
    // is the same length.
    d_ntaps = d_taps[0].size();
    for (unsigned int i = 1; i < d_nfilts; i++) {
        if (d_taps[i].size() != d_ntaps) {
            throw std::runtime_error("All sets of taps must be of the same length.");
        }
    }
    for (unsigned int i = 0; i < d_nfilts; i++) {
        // If filter taps are all zeros don't bother to crunch the numbers.
        d_active[i] = false;
        for (unsigned int j = 0; j < d_ntaps; j++) {
            if (d_taps[i][j] != 0) {
                d_active[i] = true;
                break;
            }
        }

        d_fir_filters[i].set_taps(d_taps[i]);
    }
}

void filterbank::print_taps()
{
    unsigned int i, j;
    for (i = 0; i < d_nfilts; i++) {
        printf("filter[%d]: [", i);
        for (j = 0; j < d_taps_per_filter; j++) {
            printf(" %.4e", d_taps[i][j]);
        }
        printf("]\n\n");
    }
}

std::vector<std::vector<float>> filterbank::taps() const { return d_taps; }

} /* namespace kernel */
} /* namespace filter */
} /* namespace gr */

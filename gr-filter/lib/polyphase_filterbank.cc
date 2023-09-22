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

#include <gnuradio/filter/polyphase_filterbank.h>
#include <cstdio>

namespace gr {
namespace filter {
namespace kernel {
polyphase_filterbank::polyphase_filterbank(unsigned int nfilts,
                                           const std::vector<float>& taps)
    : d_nfilts(nfilts), d_fft(nfilts)
{
    d_fir_filters.reserve(d_nfilts);
    d_fft_filters.reserve(d_nfilts);

    // Create an FIR filter for each channel and zero out the taps
    const std::vector<float> vtaps(1, 0.0f);
    for (unsigned int i = 0; i < d_nfilts; i++) {
        d_fir_filters.emplace_back(vtaps);
        d_fft_filters.emplace_back(1, vtaps);
    }

    // Now, actually set the filters' taps
    set_taps(taps);
}

void polyphase_filterbank::set_taps(const std::vector<float>& taps)
{
    unsigned int i, j;
    unsigned int ntaps = taps.size();
    d_taps_per_filter = (unsigned int)ceil((double)ntaps / (double)d_nfilts);

    // Create d_numchan vectors to store each channel's taps
    d_taps.resize(d_nfilts);

    // Make a vector of the taps plus fill it out with 0's to fill
    // each polyphase filter with exactly d_taps_per_filter
    std::vector<float> tmp_taps = taps;
    while ((float)(tmp_taps.size()) < d_nfilts * d_taps_per_filter) {
        tmp_taps.push_back(0.0);
    }

    // Partition the filter
    for (i = 0; i < d_nfilts; i++) {
        // Each channel uses all d_taps_per_filter with 0's if not enough taps to fill out
        d_taps[i] = std::vector<float>(d_taps_per_filter, 0);
        for (j = 0; j < d_taps_per_filter; j++) {
            d_taps[i][j] = tmp_taps[i + j * d_nfilts];
        }

        // Set the filter taps for each channel
        d_fir_filters[i].set_taps(d_taps[i]);
        d_fft_filters[i].set_taps(d_taps[i]);
    }
}

void polyphase_filterbank::print_taps()
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

std::vector<std::vector<float>> polyphase_filterbank::taps() const { return d_taps; }

} /* namespace kernel */
} /* namespace filter */
} /* namespace gr */

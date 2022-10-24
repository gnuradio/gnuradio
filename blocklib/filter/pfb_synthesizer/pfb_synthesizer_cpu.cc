/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pfb_synthesizer_cpu.h"
#include "pfb_synthesizer_cpu_gen.h"

namespace gr {
namespace filter {

template <class T>
pfb_synthesizer_cpu<T>::pfb_synthesizer_cpu(
    const typename pfb_synthesizer<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T), d_twox(args.twox ? 2 : 1),
    d_numchans(args.numchans)
{
    this->set_relative_rate((double)d_numchans);
    this->set_output_multiple(d_numchans);

    // set up 2x multiplier; if twox==True, set to 2, otherwise to 1
    if (args.numchans % d_twox != 0) {
        throw std::invalid_argument("pfb_synthesizer_ccf_impl: number of channels must "
                                    "be even for 2x oversampling.");
    }

    d_filters.reserve(d_twox * d_numchans);
    d_channel_map.resize(d_twox * d_numchans);

    // Create an FIR filter for each channel and zero out the taps
    // and set the default channel map
    const std::vector<float> vtaps(0, d_twox * d_numchans);
    for (unsigned int i = 0; i < d_twox * d_numchans; i++) {
        d_filters.emplace_back(vtaps);
        d_channel_map[i] = i;
    }

    // Now, actually set the filters' taps
    _set_taps(args.taps);

    // Create the IFFT to handle the input channel rotations
    d_fft = std::make_unique<kernel::fft::fft_complex_rev>(d_twox * args.numchans);
    std::fill_n(d_fft->get_inbuf(), d_twox * args.numchans, 0);

    this->set_output_multiple(args.numchans);
}

template <class T>
void pfb_synthesizer_cpu<T>::_set_taps(const std::vector<float>& taps)
{
    // The different modes, 1x or 2x the sampling rate, have
    // different filtering partitions.
    if (d_twox == 1)
        set_taps1(taps);
    else
        set_taps2(taps);

    // Because we keep our own buffers inside the filters, we don't
    // need history.
    d_updated = true;
}

template <class T>
void pfb_synthesizer_cpu<T>::set_taps1(const std::vector<float>& taps)
{
    // In this partitioning, we do a normal polyphase partitioning by
    // deinterleaving the taps into each filter:
    //
    // Prototype filter: [t0, t1, t2, t3, t4, t5, t6]
    // filter 0: [t0, t3, t6]
    // filter 1: [t1, t4, 0 ]
    // filter 2: [t2, t5, 0 ]
    unsigned int i, j;

    unsigned int ntaps = taps.size();
    d_taps_per_filter = (unsigned int)ceil((double)ntaps / (double)d_numchans);

    // Create d_numchan vectors to store each channel's taps
    d_taps.resize(d_numchans);

    // Make a vector of the taps plus fill it out with 0's to fill
    // each polyphase filter with exactly d_taps_per_filter
    std::vector<float> tmp_taps = taps;
    while ((float)(tmp_taps.size()) < d_numchans * d_taps_per_filter) {
        tmp_taps.push_back(0.0);
    }

    // Partition the filter
    for (i = 0; i < d_numchans; i++) {
        // Each channel uses all d_taps_per_filter with 0's if not enough taps to fill out
        d_taps[i] = std::vector<float>(d_taps_per_filter, 0);
        for (j = 0; j < d_taps_per_filter; j++) {
            d_taps[i][j] =
                tmp_taps[i + j * d_numchans]; // add taps to channels in reverse order
        }

        // Set the filter taps for each channel
        d_filters[i].set_taps(d_taps[i]);
    }
}

template <class T>
void pfb_synthesizer_cpu<T>::set_taps2(const std::vector<float>& taps)
{
    // In this partitioning, create 2M filters each in Z^2; the
    // second half of the filters are also delayed by Z^1:
    //
    // Prototype filter: [t0, t1, t2, t3, t4, t5, t6]
    // filter 0: [t0,  0, t6]
    // filter 1: [t2,  0,  0]
    // filter 2: [t4,  0,  0]
    // filter 3: [ 0, t1,  0]
    // filter 4: [ 0, t3,  0 ]
    // filter 5: [ 0, t5,  0 ]

    unsigned int i, j;
    int state = 0;

    unsigned int ntaps = taps.size();
    d_taps_per_filter = (unsigned int)ceil((double)ntaps / (double)d_numchans);

    // Create d_numchan vectors to store each channel's taps
    d_taps.resize(d_twox * d_numchans);

    // Make a vector of the taps plus fill it out with 0's to fill
    // each polyphase filter with exactly d_taps_per_filter
    std::vector<float> tmp_taps = taps;
    while ((float)(tmp_taps.size()) < d_numchans * d_taps_per_filter) {
        tmp_taps.push_back(0.0);
        // tmp_taps.insert(tmp_taps.begin(), 0.0);
    }

    // Partition the filter
    for (i = 0; i < d_numchans; i++) {
        // Each channel uses all d_taps_per_filter with 0's if not enough taps to fill out
        d_taps[i] = std::vector<float>(d_taps_per_filter, 0);
        d_taps[d_numchans + i] = std::vector<float>(d_taps_per_filter, 0);
        state = 0;
        for (j = 0; j < d_taps_per_filter; j++) {
            if (state == 0) {
                d_taps[i][j] = 0;
                d_taps[d_numchans + i][j] = tmp_taps[i + j * d_numchans];
                state = 1;
            }
            else {
                d_taps[i][j] = tmp_taps[i + j * d_numchans];
                d_taps[d_numchans + i][j] = 0;
                state = 0;
            }
        }

        // Set the filter taps for each channel
        d_filters[i].set_taps(d_taps[i]);
        d_filters[d_numchans + i].set_taps(d_taps[d_numchans + i]);
    }
}


template <class T>
work_return_t pfb_synthesizer_cpu<T>::work(work_io& wio)
 {
    auto in =  wio.inputs()[0].items<T>();
    auto out = wio.outputs()[0].items<T>();
    auto noutput_items = wio.outputs()[0].n_items;

    unsigned int n, i;
    auto ninputs = wio.inputs().size();

    // Algorithm for critically sampled channels
    if (d_twox == 1) {
        for (n = 0; n < noutput_items / d_numchans; n++) {
            for (i = 0; i < ninputs; i++) {
                in =  wio.inputs()[i].items<T>();
                d_fft->get_inbuf()[d_channel_map[i]] = in[n];
            }

            // spin through IFFT
            d_fft->execute();

            for (i = 0; i < d_numchans; i++) {
                out[i] = d_filters[i].filter(d_fft->get_outbuf()[i]);
            }
            out += d_numchans;
        }
    }

    // Algorithm for oversampling by 2x
    else {
        for (n = 0; n < noutput_items / d_numchans; n++) {
            for (i = 0; i < ninputs; i++) {
                // in = (gr_complex*)input_items[ninputs-i-1];
                in =  wio.inputs()[i].items<T>();
                d_fft->get_inbuf()[d_channel_map[i]] = in[n];
            }

            // spin through IFFT
            d_fft->execute();

            // Output is sum of two filters, but the input buffer to the filters must be
            // circularly shifted by numchans every time through, done by using d_state to
            // determine which IFFT buffer position to pull from.
            for (i = 0; i < d_numchans; i++) {
                out[i] =
                    d_filters[i].filter(d_fft->get_outbuf()[d_state * d_numchans + i]);
                out[i] += d_filters[d_numchans + i].filter(
                    d_fft->get_outbuf()[(d_state ^ 1) * d_numchans + i]);
            }
            d_state ^= 1;

            out += d_numchans;
        }
    }

    wio.consume_each(noutput_items/d_numchans);
    wio.produce_each(noutput_items);
    return work_return_t::OK;
}

} /* namespace filter */
} /* namespace gr */

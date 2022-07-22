/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pfb_channelizer_cpu.h"
#include "pfb_channelizer_cpu_gen.h"
#include <volk/volk.h>

namespace gr {
namespace filter {

template <class T>
pfb_channelizer_cpu<T>::pfb_channelizer_cpu(
    const typename pfb_channelizer<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T),
      polyphase_filterbank(args.numchans, args.taps),
      d_oversample_rate(args.oversample_rate),
      d_nchans(args.numchans)
{

    // The over sampling rate must be rationally related to the number of channels
    // in that it must be N/i for i in [1,N], which gives an outputsample rate
    // of [fs/N, fs] where fs is the input sample rate.
    // This tests the specified input sample rate to see if it conforms to this
    // requirement within a few significant figures.
    const double srate = d_nfilts / d_oversample_rate;
    const double rsrate = round(srate);
    if (fabs(srate - rsrate) > 0.00001)
        throw std::invalid_argument(
            "pfb_channelizer: oversample rate must be N/i for i in [1, N]");

    this->set_relative_rate(d_oversample_rate / d_nchans);

    // Default channel map. The channel map specifies which input
    // goes to which output channel; so out[0] comes from
    // channel_map[0].
    d_channel_map.resize(d_nfilts);
    for (unsigned int i = 0; i < d_nfilts; i++) {
        d_channel_map[i] = i;
    }

    // We use a look up table to set the index of the FFT input
    // buffer, which equivalently performs the FFT shift operation
    // on every other turn when the rate_ratio>1.  Also, this
    // performs the index 'flip' where the first input goes into the
    // last filter. In the pfb_decimator_ccf, we directly index the
    // input_items buffers starting with this last; here we start
    // with the first and put it into the fft object properly for
    // the same effect.
    d_rate_ratio = (int)rintf(d_nfilts / d_oversample_rate);
    d_idxlut.resize(d_nfilts);
    for (unsigned int i = 0; i < d_nfilts; i++) {
        d_idxlut[i] = d_nfilts - ((i + d_rate_ratio) % d_nfilts) - 1;
    }

    // Calculate the number of filtering rounds to do to evenly
    // align the input vectors with the output channels
    d_output_multiple = 1;
    while ((d_output_multiple * d_rate_ratio) % d_nfilts != 0)
        d_output_multiple++;
    this->set_output_multiple(d_output_multiple);

    // Use set_taps to also set the history requirement
    set_taps(args.taps);

    // because we need a stream_to_streams block for the input,
    // only send tags from in[i] -> out[i].
    this->set_tag_propagation_policy(tag_propagation_policy_t::TPP_ONE_TO_ONE);

    d_deinterleaved.resize(d_nchans);
}

template <class T>
void pfb_channelizer_cpu<T>::set_taps(const std::vector<float>& taps)
{
    // std::scoped_lock guard(d_mutex);

    polyphase_filterbank::set_taps(taps);
    // set_history(d_taps_per_filter + 1);
    d_history = d_nchans * (d_taps_per_filter + 1);
    d_updated = true;
}


template <class T>
work_return_code_t pfb_channelizer_cpu<T>::work(work_io& wio)
{
    // std::scoped_lock guard(d_mutex);

    auto in = wio.inputs()[0].items<T>();
    auto out = wio.outputs()[0].items<T>();
    auto noutput_items = wio.outputs()[0].n_items;
    auto ninput_items = wio.inputs()[0].n_items;

    if ((size_t)ninput_items < d_history * d_nchans) { // if we can produce 1 output item
        return work_return_code_t::WORK_INSUFFICIENT_INPUT_ITEMS;
    }

    if (d_updated) {
        d_updated = false;
        return work_return_code_t::WORK_OK; // history requirements may have changed.
    }

    // includes history
    auto total_items =
        std::min(ninput_items / d_nchans, noutput_items + (d_history / d_nchans));

    for (size_t j = 0; j < d_nchans; j++) {
        if (d_deinterleaved[j].size() < total_items) {
            d_deinterleaved[j].resize(total_items);
        }
        for (size_t i = 0; i < total_items; i++) {
            d_deinterleaved[j][i] = in[i * d_nchans + j];
        }
    }

    size_t noutputs = wio.outputs().size();
    noutput_items = total_items - d_history + 1;

    // The following algorithm looks more complex in order to handle
    // the cases where we want more that 1 sps for each
    // channel. Otherwise, this would boil down into a single loop
    // that operates from input_items[0] to [d_nfilts].

    // When dealing with osps>1, we start not at the last filter,
    // but nfilts/osps and then wrap around to the next symbol into
    // the other set of filters.
    // For details of this operation, see:
    // fred harris, Multirate Signal Processing For Communication
    // Systems. Upper Saddle River, NJ: Prentice Hall, 2004.

    int n = 1, i = -1, j = 0, oo = 0, last;
    int toconsume = (int)rintf(noutput_items / d_oversample_rate);
    while (n <= toconsume) {
        j = 0;
        i = (i + d_rate_ratio) % d_nfilts;
        last = i;
        while (i >= 0) {
            // in = wio.inputs()[j].items<gr_complex>();
            in = d_deinterleaved[j].data();
            d_fft.get_inbuf()[d_idxlut[j]] = d_fir_filters[i].filter(&in[n]);
            j++;
            i--;
        }

        i = d_nfilts - 1;
        while (i > last) {
            // in = wio.inputs()[j].items<gr_complex>();
            in = d_deinterleaved[j].data();
            d_fft.get_inbuf()[d_idxlut[j]] = d_fir_filters[i].filter(&in[n - 1]);
            j++;
            i--;
        }

        n += (i + d_rate_ratio) >= (int)d_nfilts;

        // despin through FFT
        d_fft.execute();

        // Send to output channels
        for (unsigned int nn = 0; nn < noutputs; nn++) {
            out = wio.outputs()[nn].items<gr_complex>();
            out[oo] = d_fft.get_outbuf()[d_channel_map[nn]];
        }
        oo++;
    }
    wio.consume_each(toconsume * d_nchans);
    // this->produce_each(noutput_items - (d_history / d_nchans - 1), work_output);
    wio.produce_each(noutput_items);
    return work_return_code_t::WORK_OK;
}

} /* namespace filter */
} /* namespace gr */

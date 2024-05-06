/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pfb_channelizer_ccf_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>

#ifdef _MSC_VER
#define round(number) number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5)
#endif

namespace gr {
namespace filter {

pfb_channelizer_ccf::sptr pfb_channelizer_ccf::make(unsigned int nfilts,
                                                    const std::vector<float>& taps,
                                                    float oversample_rate)
{
    return gnuradio::make_block_sptr<pfb_channelizer_ccf_impl>(
        nfilts, taps, oversample_rate);
}

pfb_channelizer_ccf_impl::pfb_channelizer_ccf_impl(unsigned int nfilts,
                                                   const std::vector<float>& taps,
                                                   float oversample_rate)
    : block("pfb_channelizer_ccf",
            io_signature::make(nfilts, nfilts, sizeof(gr_complex)),
            io_signature::make(1, nfilts, sizeof(gr_complex))),
      polyphase_filterbank(nfilts, taps),
      d_updated(false),
      d_oversample_rate(oversample_rate)
{
    // The over sampling rate must be rationally related to the number of channels
    // in that it must be N/i for i in [1,N], which gives an outputsample rate
    // of [fs/N, fs] where fs is the input sample rate.
    // This tests the specified input sample rate to see if it conforms to this
    // requirement within a few significant figures.
    const double srate = nfilts / oversample_rate;
    const double rsrate = round(srate);
    if (fabs(srate - rsrate) > 0.00001)
        throw std::invalid_argument(
            "pfb_channelizer: oversample rate must be N/i for i in [1, N]");

    set_relative_rate(oversample_rate);

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
    set_output_multiple(d_output_multiple);

    // Use set_taps to also set the history requirement
    set_taps(taps);

    // because we need a stream_to_streams block for the input,
    // only send tags from in[i] -> out[i].
    set_tag_propagation_policy(TPP_ONE_TO_ONE);
}

void pfb_channelizer_ccf_impl::set_taps(const std::vector<float>& taps)
{
    gr::thread::scoped_lock guard(d_mutex);

    polyphase_filterbank::set_taps(taps);
    set_history(d_taps_per_filter + 1);
    d_updated = true;
}

void pfb_channelizer_ccf_impl::print_taps() { polyphase_filterbank::print_taps(); }

std::vector<std::vector<float>> pfb_channelizer_ccf_impl::taps() const
{
    return polyphase_filterbank::taps();
}

void pfb_channelizer_ccf_impl::set_channel_map(const std::vector<int>& map)
{
    gr::thread::scoped_lock guard(d_mutex);

    if (!map.empty()) {
        unsigned int max = (unsigned int)*std::max_element(map.begin(), map.end());
        if (max >= d_nfilts) {
            throw std::invalid_argument(
                "pfb_channelizer_ccf_impl::set_channel_map: map range out of bounds.");
        }
        d_channel_map = map;
    }
}

std::vector<int> pfb_channelizer_ccf_impl::channel_map() const { return d_channel_map; }

int pfb_channelizer_ccf_impl::general_work(int noutput_items,
                                           gr_vector_int& ninput_items,
                                           gr_vector_const_void_star& input_items,
                                           gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(d_mutex);

    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    if (d_updated) {
        d_updated = false;
        return 0; // history requirements may have changed.
    }

    size_t noutputs = output_items.size();

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
            in = (const gr_complex*)input_items[j];
            d_fft.get_inbuf()[d_idxlut[j]] = d_fir_filters[i].filter(&in[n]);
            j++;
            i--;
        }

        i = d_nfilts - 1;
        while (i > last) {
            in = (const gr_complex*)input_items[j];
            d_fft.get_inbuf()[d_idxlut[j]] = d_fir_filters[i].filter(&in[n - 1]);
            j++;
            i--;
        }

        n += (i + d_rate_ratio) >= (int)d_nfilts;

        // despin through FFT
        d_fft.execute();

        // Send to output channels
        for (unsigned int nn = 0; nn < noutputs; nn++) {
            out = (gr_complex*)output_items[nn];
            out[oo] = d_fft.get_outbuf()[d_channel_map[nn]];
        }
        oo++;
    }

    consume_each(toconsume);
    return noutput_items;
}

} /* namespace filter */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2013, 2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "ofdm_carrier_allocator_cvc_impl.h"
#include <gnuradio/gr_complex.h>
#include <gnuradio/io_signature.h>
#include <algorithm>

namespace gr {
namespace digital {

ofdm_carrier_allocator_cvc::sptr ofdm_carrier_allocator_cvc::make(
    int fft_len,
    const std::vector<std::vector<int>>& occupied_carriers,
    const std::vector<std::vector<int>>& pilot_carriers,
    const std::vector<std::vector<gr_complex>>& pilot_symbols,
    const std::vector<std::vector<gr_complex>>& sync_words,
    const std::string& len_tag_key,
    const bool output_is_shifted)
{
    return gnuradio::make_block_sptr<ofdm_carrier_allocator_cvc_impl>(fft_len,
                                                                      occupied_carriers,
                                                                      pilot_carriers,
                                                                      pilot_symbols,
                                                                      sync_words,
                                                                      len_tag_key,
                                                                      output_is_shifted);
}

ofdm_carrier_allocator_cvc_impl::ofdm_carrier_allocator_cvc_impl(
    int fft_len,
    const std::vector<std::vector<int>>& occupied_carriers,
    const std::vector<std::vector<int>>& pilot_carriers,
    const std::vector<std::vector<gr_complex>>& pilot_symbols,
    const std::vector<std::vector<gr_complex>>& sync_words,
    const std::string& len_tag_key,
    const bool output_is_shifted)
    : tagged_stream_block("ofdm_carrier_allocator_cvc",
                          io_signature::make(1, 1, sizeof(gr_complex)),
                          io_signature::make(1, 1, sizeof(gr_complex) * fft_len),
                          len_tag_key),
      d_fft_len(fft_len),
      d_occupied_carriers(occupied_carriers),
      d_pilot_carriers(pilot_carriers),
      d_pilot_symbols(pilot_symbols),
      d_sync_words(sync_words),
      d_symbols_per_set(0),
      d_output_is_shifted(output_is_shifted)
{
    // Sanity checks
    // If that is is null, the input is wrong -> force user to use ((),) in python
    if (d_occupied_carriers.empty()) {
        throw std::invalid_argument(
            "Occupied carriers must be of type vector of vector i.e. ((),).");
    }
    for (unsigned i = 0; i < d_occupied_carriers.size(); i++) {
        for (unsigned j = 0; j < d_occupied_carriers[i].size(); j++) {
            if (occupied_carriers[i][j] < 0) {
                d_occupied_carriers[i][j] += d_fft_len;
            }
            if (d_occupied_carriers[i][j] > d_fft_len || d_occupied_carriers[i][j] < 0) {
                throw std::invalid_argument("data carrier index out of bounds");
            }
            if (d_output_is_shifted) {
                d_occupied_carriers[i][j] =
                    (d_occupied_carriers[i][j] + fft_len / 2) % fft_len;
            }
        }
    }
    if (d_pilot_carriers.empty()) {
        throw std::invalid_argument(
            "Pilot carriers must be of type vector of vector i.e. ((),).");
    }
    for (auto& pilots_timeslot : d_pilot_carriers) {
        for (int& pilot_carrier : pilots_timeslot) {
            if (pilot_carrier < 0) {
                pilot_carrier += d_fft_len;
            }
            if (pilot_carrier > d_fft_len || pilot_carrier < 0) {
                throw std::invalid_argument("pilot carrier index out of bounds");
            }
            if (d_output_is_shifted) {
                pilot_carrier = (pilot_carrier + fft_len / 2) % fft_len;
            }
        }
    }
    if (d_pilot_symbols.empty()) {
        throw std::invalid_argument(
            "Pilot symbols must be of type vector of vector i.e. ((),).");
    }
    for (size_t i = 0; i < std::max(d_pilot_carriers.size(), d_pilot_symbols.size());
         i++) {
        if (d_pilot_carriers[i % d_pilot_carriers.size()].size() !=
            d_pilot_symbols[i % d_pilot_symbols.size()].size()) {
            throw std::invalid_argument("pilot_carriers do not match pilot_symbols");
        }
    }
    for (const auto& sync_word : d_sync_words) {
        if (sync_word.size() != (unsigned)d_fft_len) {
            throw std::invalid_argument("sync words must be fft length");
        }
    }

    for (auto& occupied_carrier : d_occupied_carriers) {
        d_symbols_per_set += occupied_carrier.size();
    }
    set_tag_propagation_policy(TPP_DONT);
    set_relative_rate((uint64_t)d_symbols_per_set, (uint64_t)d_occupied_carriers.size());
}

ofdm_carrier_allocator_cvc_impl::~ofdm_carrier_allocator_cvc_impl() {}

int ofdm_carrier_allocator_cvc_impl::calculate_output_stream_length(
    const gr_vector_int& ninput_items)
{
    auto nin = static_cast<size_t>(ninput_items[0]);
    auto nout = (nin / d_symbols_per_set) * d_occupied_carriers.size();
    for (size_t i = 0, k = 0; i < nin % d_symbols_per_set; k++) {
        nout++;
        i += d_occupied_carriers[k % d_occupied_carriers.size()].size();
    }
    return nout + d_sync_words.size();
}

int ofdm_carrier_allocator_cvc_impl::work(int noutput_items,
                                          gr_vector_int& ninput_items,
                                          gr_vector_const_void_star& input_items,
                                          gr_vector_void_star& output_items)
{
    auto in = reinterpret_cast<const gr_complex*>(input_items[0]);
    auto out = reinterpret_cast<gr_complex*>(output_items[0]);
    auto input_cnt = static_cast<std::size_t>(ninput_items[0]);
    auto end_pointer = out + d_fft_len * noutput_items;
    std::vector<tag_t> tags;

    // Copy Sync word
    for (const auto& sync_word : d_sync_words) {
        std::copy(sync_word.cbegin(), sync_word.end(), out);
        out += d_fft_len;
    }
    std::fill(out, end_pointer, gr_complex(0, 0));

    // Copy data symbols
    size_t n_ofdm_symbols = 0; // Number of output items
    size_t curr_set = 0;
    size_t symbols_to_allocate = d_occupied_carriers[0].size();
    size_t symbols_allocated = 0;
    for (size_t i = 0; i < input_cnt; i++) {
        if (symbols_allocated == 0) {
            // Copy all tags associated with these input symbols onto this OFDM symbol
            get_tags_in_range(tags,
                              0,
                              nitems_read(0) + i,
                              nitems_read(0) +
                                  std::min(i + symbols_to_allocate, input_cnt));
            for (const auto& tag : tags) {
                add_item_tag(0,
                             nitems_written(0) + n_ofdm_symbols +
                                 (n_ofdm_symbols == 0 ? 0 : d_sync_words.size()),
                             tag.key,
                             tag.value);
            }
            n_ofdm_symbols++;
        }
        out[(n_ofdm_symbols - 1) * d_fft_len +
            d_occupied_carriers[curr_set][symbols_allocated]] = in[i];
        symbols_allocated++;
        if (symbols_allocated == symbols_to_allocate) {
            curr_set = (curr_set + 1) % d_occupied_carriers.size();
            symbols_to_allocate = d_occupied_carriers[curr_set].size();
            symbols_allocated = 0;
        }
    }
    // Copy pilot symbols
    for (size_t i = 0; i < n_ofdm_symbols; i++) {
        auto& carrier = d_pilot_carriers[i % d_pilot_carriers.size()];
        for (size_t k = 0; k < carrier.size(); k++) {
            out[i * d_fft_len + carrier[k]] =
                d_pilot_symbols[i % d_pilot_symbols.size()][k];
        }
    }

    return n_ofdm_symbols + d_sync_words.size();
}

} // namespace digital
} // namespace gr

/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "linear_equalizer_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/misc.h>
#include <volk/volk.h>
#include <memory>
#include <vector>

#include "adaptive_algorithms.h"

using namespace pmt;
using namespace std;

namespace gr {
namespace digital {

linear_equalizer::sptr linear_equalizer::make(unsigned num_taps,
                                              unsigned sps,
                                              adaptive_algorithm_sptr alg,
                                              bool adapt_after_training,
                                              std::vector<gr_complex> training_sequence,
                                              const std::string& training_start_tag)
{
    return gnuradio::make_block_sptr<linear_equalizer_impl>(
        num_taps, sps, alg, adapt_after_training, training_sequence, training_start_tag);
}

/*
 * The private constructor
 */
linear_equalizer_impl::linear_equalizer_impl(unsigned num_taps,
                                             unsigned sps,
                                             adaptive_algorithm_sptr alg,
                                             bool adapt_after_training,
                                             std::vector<gr_complex> training_sequence,
                                             const std::string& training_start_tag)
    : gr::sync_decimator("linear_equalizer",
                         io_signature::make(1, 1, sizeof(gr_complex)),
                         io_signature::make3(1,
                                             3,
                                             sizeof(gr_complex),
                                             num_taps * sizeof(gr_complex),
                                             sizeof(unsigned short)),
                         sps),
      filter::kernel::fir_filter_ccc(vector<gr_complex>(num_taps, gr_complex(0, 0))),
      d_num_taps(num_taps),
      d_sps(sps),
      d_alg(alg),
      d_adapt_after_training(adapt_after_training),
      d_training_sequence(training_sequence),
      d_training_start_tag(pmt::intern(training_start_tag)),
      d_new_taps(num_taps),
      d_updated(false),
      d_training_sample(0)
{
    if (training_start_tag.empty() || training_sequence.empty()) {
        d_training_state = equalizer_state_t::DD;
    } else {
        d_training_state = equalizer_state_t::IDLE;
    }

    alg->initialize_taps(d_new_taps);
    // NOTE: the filter kernel reverses the taps internally
    filter::kernel::fir_filter_ccc::set_taps(d_new_taps);

    const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    set_alignment(max(1, alignment_multiple));
    set_history(num_taps);
}

void linear_equalizer_impl::set_taps(const vector<gr_complex>& taps)
{
    gr::thread::scoped_lock guard(d_mutex);
    d_new_taps = taps;
    d_updated = true;
}

vector<gr_complex> linear_equalizer_impl::taps() const
{
    gr::thread::scoped_lock guard(d_mutex);
    return d_taps;
}

int linear_equalizer_impl::equalize(const gr_complex* input_samples,
                                    gr_complex* output_symbols,
                                    unsigned int num_inputs,
                                    unsigned int max_num_outputs,
                                    std::vector<unsigned int> training_start_samples,
                                    bool history_included,
                                    gr_complex* taps,
                                    unsigned short* state)
{
    unsigned nout = num_inputs / d_sps;
    if (nout > max_num_outputs) {
        nout = max_num_outputs;
    }

    const gr_complex* samples;
    std::unique_ptr<std::vector<gr_complex>> in_prepended_history;
    if (history_included) {
        samples = input_samples;
    } else {
        in_prepended_history =
            std::make_unique<std::vector<gr_complex>>(num_inputs + d_num_taps - 1);
        std::copy(input_samples,
                  input_samples + num_inputs,
                  in_prepended_history->begin() + d_num_taps - 1);
        samples = in_prepended_history->data();
    }

    unsigned int tag_index = 0;
    unsigned int j = 0; // pre-decimated input buffer index
    for (unsigned i = 0; i < nout; i++) {
        output_symbols[i] = filter(&samples[j]);

        if (taps) {
            std::copy(d_taps.begin(), d_taps.end(), taps + d_num_taps * i);
        }

        if (state) {
            state[i] = (unsigned short)d_training_state;
        }

        if (!training_start_samples.empty() &&
            tag_index < training_start_samples.size()) {
            unsigned int tag_sample = training_start_samples[tag_index];
            if (tag_sample >= j && tag_sample < (j + decimation())) {
                d_training_state = equalizer_state_t::TRAINING;
                d_training_sample = 0;
                tag_index++;
            }
        }

        // Are we done with the training sequence
        if (d_training_sample >= d_training_sequence.size()) {
            if (d_adapt_after_training) {
                d_training_state = equalizer_state_t::DD;
            } else {
                d_training_state = equalizer_state_t::IDLE;
            }
            d_training_sample = -1;
        }

        // Adjust taps
        if (d_training_state == equalizer_state_t::TRAINING) {
            d_decision = d_training_sequence[d_training_sample];
            d_error = d_alg->error_tr(output_symbols[i],
                                      d_training_sequence[d_training_sample++]);
        } else if (d_training_state == equalizer_state_t::DD) {
            d_error = d_alg->error_dd(output_symbols[i], d_decision);
        }
        switch (d_training_state) {
        case equalizer_state_t::IDLE:
            d_error = gr_complex(0.0, 0.0);
            break;
        case equalizer_state_t::TRAINING:
        case equalizer_state_t::DD:
            d_alg->update_taps(
                &d_taps[0], &samples[j], d_error, d_decision, d_taps.size());
            for (unsigned k = 0; k < d_taps.size(); k++) {
                // Update aligned taps in filter object.
                filter::kernel::fir_filter_ccc::update_tap(d_taps[k], k);
            }
            break;
        }

        j += decimation();
    }

    return nout;
}

int linear_equalizer_impl::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    {
        gr::thread::scoped_lock guard(d_mutex);
        if (d_updated) {
            d_taps = d_new_taps;
            set_history(d_taps.size());
            d_updated = false;
            return 0; // history requirements may have changed.
        }
    }

    unsigned long int nread = nitems_read(0);
    vector<tag_t> tags;
    get_tags_in_window(tags, 0, 0, noutput_items * decimation(), d_training_start_tag);
    vector<unsigned int> training_start_samples(tags.size());
    unsigned int tag_index = 0;
    for (const auto& tag : tags) {
        training_start_samples[tag_index++] = tag.offset - nread;
    }

    auto in = static_cast<const gr_complex*>(input_items[0]);
    auto out = static_cast<gr_complex*>(output_items[0]);

    int outlen = output_items.size();
    unsigned short* state = nullptr;
    gr_complex* taps = nullptr;

    if (outlen > 1)
        taps = static_cast<gr_complex*>(output_items[1]);
    if (outlen > 2)
        state = static_cast<unsigned short*>(output_items[2]);

    return equalize(in,
                    out,
                    noutput_items * decimation(),
                    noutput_items,
                    training_start_samples,
                    true,
                    taps,
                    state);
}

} // namespace digital
} /* namespace gr */

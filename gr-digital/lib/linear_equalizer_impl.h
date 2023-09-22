/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_LINEAR_EQUALIZER_IMPL_H
#define INCLUDED_DIGITAL_LINEAR_EQUALIZER_IMPL_H

#include "equalizer.h"
#include <gnuradio/digital/linear_equalizer.h>
#include <gnuradio/filter/fir_filter.h>
#include <stdexcept>

namespace gr {
namespace digital {

class linear_equalizer_impl : public linear_equalizer, filter::kernel::fir_filter_ccc
{
private:
    const unsigned d_num_taps;
    const unsigned d_sps;
    adaptive_algorithm_sptr d_alg;
    const bool d_adapt_after_training;
    const std::vector<gr_complex> d_training_sequence;
    const pmt::pmt_t d_training_start_tag;

    std::vector<gr_complex> d_new_taps;
    bool d_updated;
    gr_complex d_error;
    gr_complex d_decision;

    equalizer_state_t d_training_state;
    unsigned int d_training_sample;

    mutable gr::thread::mutex d_mutex;

public:
    linear_equalizer_impl(unsigned num_taps,
                          unsigned sps,
                          adaptive_algorithm_sptr alg,
                          bool adapt_after_training,
                          std::vector<gr_complex> training_sequence,
                          const std::string& training_start_tag);
    ~linear_equalizer_impl() override{};

    void set_taps(const std::vector<gr_complex>& taps) override;
    std::vector<gr_complex> taps() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    // Same as the work function, but more specific and decoupled from the
    // scheduler-related actions
    int equalize(
        const gr_complex* input_samples,
        gr_complex* output_symbols,
        unsigned int num_inputs,
        unsigned int max_num_outputs,
        std::vector<unsigned int> training_start_samples = std::vector<unsigned int>(0),
        bool history_included = false,
        gr_complex* taps = nullptr,
        unsigned short* state = nullptr) override;
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_LINEAR_EQUALIZER_IMPL_H */

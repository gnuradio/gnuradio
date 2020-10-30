/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_DECISION_FEEDBACK_EQUALIZER_IMPL_H
#define INCLUDED_DIGITAL_DECISION_FEEDBACK_EQUALIZER_IMPL_H

#include "equalizer.h"
#include <gnuradio/digital/decision_feedback_equalizer.h>
#include <gnuradio/filter/fir_filter.h>
#include <volk/volk_alloc.hh>
#include <deque>

namespace gr {
namespace digital {

class decision_feedback_equalizer_impl : public decision_feedback_equalizer,
                                         filter::kernel::fir_filter_ccc
{
private:
    const unsigned d_num_taps_fwd;
    const unsigned d_num_taps_rev;
    const unsigned d_sps;
    adaptive_algorithm_sptr d_alg;
    const bool d_adapt_after_training;
    const std::vector<gr_complex> d_training_sequence;
    const pmt::pmt_t d_training_start_tag;
    const unsigned d_num_taps;
    std::deque<gr_complex> d_decision_history;
    std::vector<gr_complex> d_new_taps;
    bool d_updated;

    gr_complex d_error;
    equalizer_state_t d_training_state;
    unsigned int d_training_sample;
    volk::vector<gr_complex> d_filt_buf;

    mutable gr::thread::mutex d_mutex;

public:
    decision_feedback_equalizer_impl(unsigned num_taps_forward,
                                     unsigned num_taps_feedback,
                                     unsigned sps,
                                     adaptive_algorithm_sptr alg,
                                     bool adapt_after_training,
                                     std::vector<gr_complex> training_sequence,
                                     const std::string& training_start_tag);
    ~decision_feedback_equalizer_impl() override {}

    void update_decision_history(gr_complex decision);

    void set_taps(const std::vector<gr_complex>& taps) override;
    std::vector<gr_complex> taps() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    // Same as the work function, but more specific and decoupled from the
    // scheduler-related actions
    int equalize(const gr_complex* input_samples,
                 gr_complex* output_symbols,
                 unsigned int num_inputs,
                 unsigned int max_num_outputs,
                 std::vector<unsigned int> training_start_samples,
                 bool history_included,
                 gr_complex* taps,
                 unsigned short* state) override;
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_DECISION_FEEDBACK_EQUALIZER_IMPL_H */

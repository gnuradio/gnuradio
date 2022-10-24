/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/filter/freq_xlating_fir_filter.h>
#include <gnuradio/kernel/filter/fir_filter.h>
#include <gnuradio/kernel/math/rotator.h>

using namespace gr::kernel::math;

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
class freq_xlating_fir_filter_cpu : public freq_xlating_fir_filter<IN_T, OUT_T, TAP_T>
{
public:
    freq_xlating_fir_filter_cpu(
        const typename freq_xlating_fir_filter<IN_T, OUT_T, TAP_T>::block_args& args);

    work_return_t work(work_io& wio) override;

    void on_parameter_change(param_action_sptr action) override;

private:
    std::vector<TAP_T> d_proto_taps;
    gr::kernel::filter::fir_filter<IN_T, OUT_T, gr_complex> d_composite_fir;
    rotator d_r;
    double d_prev_center_freq;
    bool d_updated = true; // treat the first call to work() as an update

    size_t d_history = 1;
    int d_hist_change = 1;
    bool d_hist_updated = false;

    virtual void build_composite_fir();
};


} // namespace filter
} // namespace gr

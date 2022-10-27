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

#include <gnuradio/kernel/filter/fft_filter.h>
#include <gnuradio/filter/fft_filter.h>

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
class fft_filter_cpu : public fft_filter<IN_T, OUT_T, TAP_T>
{
public:
    fft_filter_cpu(const typename fft_filter<IN_T, OUT_T, TAP_T>::block_args& args);

    work_return_t work(work_io& wio) override;

    void on_parameter_change(param_action_sptr action) override;

private:
    gr::kernel::filter::fft_filter<IN_T, TAP_T> d_filter;

    bool d_updated = true; // treat the first call to work() as an update
    int d_nsamples;
    size_t d_history = 1;
    int d_hist_change = 1;
    bool d_hist_updated = false;
};


} // namespace filter
} // namespace gr

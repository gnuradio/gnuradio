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

#include <gnuradio/filter/fft_filter.h>
#include <gnuradio/kernel/filter/fft_filter.h>

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
    work_return_t enforce_constraints(work_io& wio) override;

    gr::kernel::filter::fft_filter<IN_T, TAP_T> d_filter;

    bool d_updated = true; // treat the first call to work() as an update
    int d_nsamples;
};


} // namespace filter
} // namespace gr

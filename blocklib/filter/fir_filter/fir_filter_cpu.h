/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/kernel/filter/fir_filter.h>

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
class fir_filter_cpu : public fir_filter<IN_T, OUT_T, TAP_T>
{
public:
    fir_filter_cpu(const typename fir_filter<IN_T, OUT_T, TAP_T>::block_args& args);

    work_return_t work(work_io&) override;

    void on_parameter_change(param_action_sptr action) override;


private:
    work_return_t enforce_constraints(work_io& wio) override;

    gr::kernel::filter::fir_filter<IN_T, OUT_T, TAP_T> d_fir;

    volk::vector<IN_T> d_startup_vec;
    size_t d_startup_nsamps = 0;
    bool d_startup_complete = false;

    size_t _deficit = 0;
};


} // namespace filter
} // namespace gr

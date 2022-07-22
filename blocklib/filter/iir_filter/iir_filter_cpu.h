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

#include <gnuradio/filter/iir_filter.h>

#include <gnuradio/kernel/filter/iir_filter.h>

namespace gr {
namespace filter {

template <class T_IN, class T_OUT, class TAP_T>
class iir_filter_cpu : public iir_filter<T_IN, T_OUT, TAP_T>
{
public:
    iir_filter_cpu(const typename iir_filter<T_IN, T_OUT, TAP_T>::block_args& args);

    work_return_code_t work(work_io&) override;

private:
    bool d_updated;
    kernel::filter::iir_filter<T_IN, T_OUT, TAP_T> d_iir;

    void on_parameter_change(param_action_sptr action) override
    {
        // This will set the underlying PMT
        block::on_parameter_change(action);
        auto fftaps = pmtf::get_as<std::vector<TAP_T>>(*this->param_fftaps);
        auto fbtaps = pmtf::get_as<std::vector<TAP_T>>(*this->param_fbtaps);

        // Do more updating for certain parameters
        if (action->id() == iir_filter<T_IN, T_OUT, TAP_T>::id_fftaps ||
            action->id() == iir_filter<T_IN, T_OUT, TAP_T>::id_fbtaps) {
            d_iir.set_taps(fftaps, fbtaps);
        }
    }
};


} // namespace filter
} // namespace gr

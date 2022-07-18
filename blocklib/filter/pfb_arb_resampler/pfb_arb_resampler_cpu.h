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

#include <gnuradio/filter/pfb_arb_resampler.h>
#include <gnuradio/kernel/filter/pfb_arb_resampler.h>

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
class pfb_arb_resampler_cpu : public pfb_arb_resampler<IN_T, OUT_T, TAP_T>
{
public:
    pfb_arb_resampler_cpu(
        const typename pfb_arb_resampler<IN_T, OUT_T, TAP_T>::block_args& args);

    work_return_code_t work(work_io&) override;
    virtual size_t group_delay() const override { return d_resamp.group_delay(); }
    virtual size_t phase_offset(float freq, float fs) const override
    {
        return d_resamp.phase_offset(freq, fs);
    };

private:
    kernel::filter::pfb_arb_resampler<IN_T, OUT_T, TAP_T> d_resamp;
    size_t d_history;

    void on_parameter_change(param_action_sptr action) override
    {
        // This will set the underlying PMT
        block::on_parameter_change(action);

        // Do more updating for certain parameters
        if (action->id() == pfb_arb_resampler<IN_T, OUT_T, TAP_T>::id_taps) {
            d_resamp.set_taps(pmtf::get_as<std::vector<TAP_T>>(*this->param_taps));
            d_history = d_resamp.taps_per_filter();
        }
        else if (action->id() == pfb_arb_resampler<IN_T, OUT_T, TAP_T>::id_rate) {
            d_resamp.set_rate(pmtf::get_as<float>(*this->param_rate));
            this->set_relative_rate(pmtf::get_as<float>(*this->param_rate));
        }
    }
};


} // namespace filter
} // namespace gr

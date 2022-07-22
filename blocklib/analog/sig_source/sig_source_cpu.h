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

#include <gnuradio/analog/sig_source.h>
#include <gnuradio/kernel/math/fxpt_nco.h>
#include <gnuradio/kernel/math/math.h>
#include <mutex>

namespace gr {
namespace analog {

template <class T>
class sig_source_cpu : public sig_source<T>
{
public:
    sig_source_cpu(const typename sig_source<T>::block_args& args);

    work_return_code_t work(work_io&) override;


    void on_parameter_change(param_action_sptr action) override
    {
        // This will set the underlying PMT
        block::on_parameter_change(action);

        // Do more updating for certain parameters
        if (action->id() == sig_source<T>::id_phase) {
            auto phase = pmtf::get_as<double>(*this->param_phase);
            d_nco.set_phase(phase);
        }
        else if (action->id() == sig_source<T>::id_frequency) {
            auto freq = pmtf::get_as<double>(*this->param_frequency);
            auto samp_freq = pmtf::get_as<double>(*this->param_sampling_freq);
            d_nco.set_freq(2 * GR_M_PI * freq / samp_freq);
        }
    }


private:
    // Declare private variables here
    gr::kernel::math::fxpt_nco d_nco;

    // sig_source has some non thread safe accessors (for now)
    std::mutex d_mutex;
};


} // namespace analog
} // namespace gr

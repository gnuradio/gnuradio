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
#include <gnuradio/kernel/fft/window.h>
#include <gnuradio/kernel/filter/firdes.h>
#include <gnuradio/kernel/filter/optfir.h>
#include <gnuradio/kernel/filter/pfb_arb_resampler.h>

using namespace gr::kernel::filter;

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
class pfb_arb_resampler_cpu : public pfb_arb_resampler<IN_T, OUT_T, TAP_T>
{
public:
    pfb_arb_resampler_cpu(
        const typename pfb_arb_resampler<IN_T, OUT_T, TAP_T>::block_args& args);

    work_return_t work(work_io&) override;
    virtual size_t group_delay() const override { return d_resamp->group_delay(); }
    virtual float phase_offset(float freq, float fs) const override
    {
        return d_resamp->phase_offset(freq, fs);
    };

private:
    std::unique_ptr<kernel::filter::pfb_arb_resampler<IN_T, OUT_T, TAP_T>> d_resamp;
    size_t d_history;

    bool d_init_padding = false;
    size_t d_padding_samps = 0;
    std::vector<IN_T> d_padding_vec;

    work_return_t enforce_constraints(work_io&) override;

    std::vector<TAP_T> create_taps(float rate, size_t flt_size=32, float atten= 100);

    std::vector<float> create_taps_float(float rate, size_t flt_size = 32, float atten = 100)
    {
        // # Create a filter that covers the full bandwidth of the output signal

        // # If rate >= 1, we need to prevent images in the output,
        // # so we have to filter it to less than half the channel
        // # width of 0.5.  If rate < 1, we need to filter to less
        // # than half the output signal's bw to avoid aliasing, so
        // # the half-band here is 0.5*rate.
        
        float percent = 0.80;
        if (rate < 1) {
            float halfband = 0.5 * rate;
            float bw = percent * halfband;
            float tb = (percent / 2.0) * halfband;
            // float ripple = 0.1;


            // # As we drop the bw factor, the optfir filter has a harder time converging;
            // # using the firdes method here for better results.
            return firdes::low_pass_2(flt_size,
                                      flt_size,
                                      bw,
                                      tb,
                                      atten,
                                      gr::kernel::fft::window::window_t::BLACKMAN_HARRIS);
        }
        else {
            float halfband = 0.5;
            float bw = percent * halfband;
            float tb = (percent / 2.0) * halfband;
            float ripple = 0.1;
            std::vector<double> dtaps;

            while (true) {
                try {
                    dtaps = gr::kernel::filter::optfir::low_pass(
                        flt_size, flt_size, bw, bw + tb, ripple, atten);
                    break;
                } catch (std::exception& e) {
                    ripple += 0.01;
                    this->d_logger->warn(
                        "Warning: set ripple to {:4f} dB. If this is a problem, adjust "
                        "the attenuation or create your own filter taps.",
                        ripple);

                    // # Build in an exit strategy; if we've come this far, it ain't
                    // working.
                    if (ripple >= 1.0) {
                        throw std::runtime_error(
                            "optfir could not generate an appropriate filter.");
                    }
                }
            }

            return std::vector<float>(dtaps.begin(), dtaps.end());
        }
    }


    void on_parameter_change(param_action_sptr action) override
    {
        // This will set the underlying PMT
        block::on_parameter_change(action);

        // Do more updating for certain parameters
        if (action->id() == pfb_arb_resampler<IN_T, OUT_T, TAP_T>::id_taps) {
            d_resamp->set_taps(pmtf::get_as<std::vector<TAP_T>>(*this->param_taps));
            d_history = d_resamp->taps_per_filter();
        }
        else if (action->id() == pfb_arb_resampler<IN_T, OUT_T, TAP_T>::id_rate) {
            d_resamp->set_rate(pmtf::get_as<float>(*this->param_rate));
            this->set_relative_rate(pmtf::get_as<float>(*this->param_rate));
        }
    }
};


} // namespace filter
} // namespace gr

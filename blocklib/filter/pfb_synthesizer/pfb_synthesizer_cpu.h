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

#include <gnuradio/filter/pfb_synthesizer.h>
#include <gnuradio/kernel/fft/fftw_fft.h>
#include <gnuradio/kernel/filter/fir_filter_with_buffer.h>

namespace gr {
namespace filter {

template <class T>
class pfb_synthesizer_cpu : public pfb_synthesizer<T>
{
public:
    pfb_synthesizer_cpu(const typename pfb_synthesizer<T>::block_args& args);

    work_return_t work(work_io& wio) override;

    void on_parameter_change(param_action_sptr action) override
    {
        // This will set the underlying PMT
        block::on_parameter_change(action);

        // Do more updating for certain parameters
        if (action->id() == pfb_synthesizer<T>::id_taps) {
            this->_set_taps(pmtf::get_as<std::vector<float>>(*this->param_taps));
        }
    }

private:
    bool d_updated = true;

    std::vector<std::vector<float>> d_taps;
    size_t d_taps_per_filter;
    std::unique_ptr<kernel::fft::fft_complex_rev> d_fft;
    std::vector<kernel::filter::fir_filter_with_buffer_ccf> d_filters;
    int d_state = 0;
    int d_twox;
    size_t d_numchans;
    std::vector<int> d_channel_map;

    void _set_taps(const std::vector<float>& taps);

    /*!
     * \brief Tap setting algorithm for critically sampled channels
     */
    void set_taps1(const std::vector<float>& taps);

    /*!
     * \brief Tap setting algorithm for 2x over-sampled channels
     */
    void set_taps2(const std::vector<float>& taps);
};


} // namespace filter
} // namespace gr

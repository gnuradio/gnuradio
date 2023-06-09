/* -*- c++ -*- */
/*
 * Copyright 2023 Ettus Research, A National Instruments Brand
 * Copyright 2023 Jeff Long
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_QTGUI_FOSPHOR_FORMATTER_IMPL_H
#define INCLUDED_QTGUI_FOSPHOR_FORMATTER_IMPL_H

#include <gnuradio/fft/fft.h>
#include <gnuradio/qtgui/fosphor_formatter.h>
#include <volk/volk_alloc.hh>

namespace gr {
namespace qtgui {

class fosphor_formatter_impl : public fosphor_formatter
{
public:
    fosphor_formatter_impl(int fft_size,
                           int num_bins,
                           int input_decim,
                           int waterfall_decim,
                           int histo_decim,
                           double scale,
                           double alpha,
                           double epsilon,
                           double trise,
                           double tdecay);

    ~fosphor_formatter_impl() override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) final;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) final;

private:
    const int d_fft_size;
    const int d_num_bins;
    const int d_input_decim;
    const int d_waterfall_decim;
    const int d_histo_decim;
    const float d_scale;
    const float d_alpha;
    const float d_epsilon;
    const float d_trise;
    const float d_tdecay;

    gr::fft::fft_complex_fwd* d_fft;
    std::vector<float> d_window;

    volk::vector<float> d_iir;
    volk::vector<float> d_maxhold_buf;
    volk::vector<float> d_histo_buf_f;
    volk::vector<uint8_t> d_histo_buf_b;
    volk::vector<int16_t> d_hit_count;
    volk::vector<float> d_logfft;
    volk::vector<uint8_t> d_logfft_b;
    volk::vector<uint8_t> d_logfft_avg_b;

    // Counters for decimation
    int d_input_count;
    int d_waterfall_count;
    int d_histo_count;
};

} // namespace qtgui
} /* namespace gr */

#endif /* INCLUDED_QTGUI_FOSPHOR_FORMATTER_IMPL_H */

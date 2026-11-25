/* -*- c++ -*- */
/*
 * Copyright 2023 Ettus Research, A National Instruments Brand
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rfnoc_f15_formatter_impl.h"
#include <gnuradio/block.h>
#include <gnuradio/fft/window.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/sync_block.h>
#include <cmath>


namespace {

// Helper function for converting float to unsigned char
void float_array_to_uchar(const float* in,
                          unsigned char* out,
                          int nsamples,
                          const float scaling = 1.0f)
{
    constexpr int MIN_UCHAR = 0;
    constexpr int MAX_UCHAR = 255;
    for (int i = 0; i < nsamples; i++) {
        long int r = (long int)rint(in[i] * scaling);
        if (r < MIN_UCHAR)
            r = MIN_UCHAR;
        else if (r > MAX_UCHAR)
            r = MAX_UCHAR;
        out[i] = r;
    }
}

class float_to_uchar_vector : public gr::sync_block
{
    const int d_fft_size;

public:
    using sptr = std::shared_ptr<float_to_uchar_vector>;

    explicit float_to_uchar_vector(int fft_size)
        : gr::sync_block("float2byte",
                         gr::io_signature::make(1, 1, sizeof(float) * fft_size),
                         gr::io_signature::make(1, 1, sizeof(uint8_t) * fft_size)),
          d_fft_size(fft_size)
    {
    }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override
    {
        const auto* in = static_cast<const float*>(input_items[0]);
        auto* out = static_cast<uint8_t*>(output_items[0]);
        for (int item = 0; item < noutput_items; ++item) {
            float_array_to_uchar(in, out, d_fft_size);
            in += d_fft_size;
            out += d_fft_size;
        }
        return noutput_items;
    }
};

class histogram_processor : public gr::block
{
    const int d_fft_size;
    const int d_histo_decim;
    const int d_num_bins = 64;
    int d_histo_count = 0;
    const float d_epsilon;
    const double d_trise;
    const double d_tdecay;
    volk::vector<float> d_maxhold_buf;
    volk::vector<float> d_histo_buf_f;
    volk::vector<int16_t> d_hit_count;

    void update_histo_val(float& hv, const int16_t hc)
    {
        if (hv < 0.01f && hc == 0) {
            return;
        }

        const float a = static_cast<float>(hc) / d_histo_decim;
        const float b = a / d_trise;
        const float c = b + 1.0f / d_tdecay;
        const float d = b / c;
        const float e = std::pow(1.0f - c, d_histo_decim);

        hv = (hv - d) * e + d;
        hv = std::max(std::min(1.0f, hv), 0.0f);
    }

public:
    using sptr = std::shared_ptr<histogram_processor>;

    histogram_processor(
        int fft_size, int histo_decim, double epsilon, double trise, double tdecay)
        : gr::block("histo_proc",
                    gr::io_signature::make2(
                        3, 3, sizeof(uint8_t) * fft_size, sizeof(float) * fft_size),
                    gr::io_signature::make(1, 1, sizeof(uint8_t) * fft_size)),
          d_fft_size(fft_size),
          d_histo_decim(histo_decim),
          d_epsilon(epsilon),
          d_trise(trise),
          d_tdecay(tdecay),
          d_maxhold_buf(fft_size, 0.0f),
          d_histo_buf_f(fft_size * d_num_bins, 0.0f),
          d_hit_count(fft_size * d_num_bins, 0)
    {
        set_output_multiple(d_num_bins + 2);
    }

    void forecast(int, gr_vector_int& ninput_items_required) override
    {
        const int required = d_histo_decim - d_histo_count;
        std::fill(ninput_items_required.begin(), ninput_items_required.end(), required);
    }

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override
    {
        const int items_to_process = std::min<int>({ ninput_items[0],
                                                     ninput_items[1],
                                                     ninput_items[2],
                                                     d_histo_decim - d_histo_count });
        if (items_to_process == 0 || noutput_items < d_num_bins + 2) {
            return 0;
        }

        const auto* in_logfft_b = static_cast<const uint8_t*>(input_items[0]);
        const auto* in_logfft_f = static_cast<const float*>(input_items[1]);
        const auto* in_logfft_avg = static_cast<const float*>(input_items[2]);
        auto* out = static_cast<uint8_t*>(output_items[0]);

        for (int item = 0; item < items_to_process; ++item) {
            volk_32f_s32f_multiply_32f(
                d_maxhold_buf.data(), d_maxhold_buf.data(), d_epsilon, d_fft_size);
            volk_32f_x2_max_32f(
                d_maxhold_buf.data(), d_maxhold_buf.data(), in_logfft_f, d_fft_size);
            for (int i = 0; i < d_fft_size; ++i) {
                const uint8_t bin_index = in_logfft_b[i] >> 2;
                d_hit_count[bin_index * d_fft_size + i]++;
            }
            ++d_histo_count;
            in_logfft_f += d_fft_size;
            in_logfft_b += d_fft_size;
        }
        consume_each(items_to_process);
        if (d_histo_count < d_histo_decim) {
            return 0;
        }
        d_histo_count = 0;

        for (size_t i = 0; i < d_histo_buf_f.size(); ++i) {
            update_histo_val(d_histo_buf_f[i], d_hit_count[i]);
        }
        float_array_to_uchar(d_histo_buf_f.data(), out, d_fft_size * d_num_bins, 256);
        const int avg_idx = d_num_bins * d_fft_size;
        const int maxhold_idx = avg_idx + d_fft_size;
        float_array_to_uchar(in_logfft_avg, out + avg_idx, d_fft_size);
        float_array_to_uchar(d_maxhold_buf.data(), out + maxhold_idx, d_fft_size);

        auto tag = gr::tag_t{};
        tag.key = pmt::string_to_symbol("rx_eob");
        tag.value = pmt::PMT_T;
        tag.offset = nitems_written(0) + d_num_bins + 1;
        add_item_tag(0, tag);

        std::fill(d_hit_count.begin(), d_hit_count.end(), 0);
        return d_num_bins + 2;
    }
};

// Helper function to transform the scale factor to something we can put into
// the nlog10_ff block
float calculate_log10_scale(const double scale)
{
    return static_cast<float>(scale) * 10.0f;
}


} // namespace

using namespace gr::qtgui;

rfnoc_f15_formatter::sptr rfnoc_f15_formatter::make(int fft_size,
                                                    int num_bins,
                                                    int input_decim,
                                                    int waterfall_decim,
                                                    int histo_decim,
                                                    double scale,
                                                    double alpha,
                                                    double epsilon,
                                                    double trise,
                                                    double tdecay)
{
    return gnuradio::make_block_sptr<rfnoc_f15_formatter_impl>(fft_size,
                                                               num_bins,
                                                               input_decim,
                                                               waterfall_decim,
                                                               histo_decim,
                                                               scale,
                                                               alpha,
                                                               epsilon,
                                                               trise,
                                                               tdecay);
}


rfnoc_f15_formatter_impl::rfnoc_f15_formatter_impl(int fft_size,
                                                   int num_bins,
                                                   int input_decim,
                                                   int waterfall_decim,
                                                   int histo_decim,
                                                   double scale,
                                                   double alpha,
                                                   double epsilon,
                                                   double trise,
                                                   double tdecay)
    : hier_block2("rfnoc_f15_formatter",
                  io_signature::make(1, 1, sizeof(gr_complex)),
                  io_signature::make(2, 2, sizeof(unsigned char) * fft_size)),
      // Init sub-blocks
      d_s2v(gr::blocks::stream_to_vector::make(sizeof(gr_complex), fft_size)),
      d_input_decim(
          gr::blocks::keep_one_in_n::make(sizeof(gr_complex) * fft_size, input_decim)),
      d_fft(gr::fft::fft_v<gr_complex, true /* forward */>::make(
          fft_size, gr::fft::window::blackman_harris(fft_size), true /* shift */)),
      d_c2m(gr::blocks::complex_to_mag_squared::make(fft_size)),
      d_log(gr::blocks::nlog10_ff::make(calculate_log10_scale(scale),
                                        fft_size,
                                        20 * std::log10(static_cast<float>(fft_size)))),
      d_f2byte(gnuradio::make_block_sptr<float_to_uchar_vector>(fft_size)),
      d_avg(gr::filter::single_pole_iir_filter_ff::make(alpha, fft_size)),
      d_histo_proc(gnuradio::make_block_sptr<histogram_processor>(
          fft_size, histo_decim, epsilon, trise, tdecay)),
      d_wf_decim(gr::blocks::keep_one_in_n::make(sizeof(unsigned char) * fft_size,
                                                 waterfall_decim))
{
    // Common path
    connect(self(), 0, d_s2v, 0);
    connect(d_s2v, 0, d_input_decim, 0);
    connect(d_input_decim, 0, d_fft, 0);
    connect(d_fft, 0, d_c2m, 0);
    connect(d_c2m, 0, d_log, 0);
    connect(d_log, 0, d_f2byte, 0);
    // Histogram path
    connect(d_log, 0, d_avg, 0);
    connect(d_f2byte, 0, d_histo_proc, 0);
    connect(d_log, 0, d_histo_proc, 1);
    connect(d_avg, 0, d_histo_proc, 2);
    connect(d_histo_proc, 0, self(), 0);
    // Waterfall path
    connect(d_f2byte, 0, d_wf_decim, 0);
    connect(d_wf_decim, 0, self(), 1);
}

rfnoc_f15_formatter_impl::~rfnoc_f15_formatter_impl() {}

/* -*- c++ -*- */
/*
 * Copyright 2023 Ettus Research, A National Instruments Brand
 * Copyright 2023 Jeff Long
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "fosphor_formatter_impl.h"
#include <gnuradio/fft/fft.h>
#include <gnuradio/fft/window.h>
#include <gnuradio/io_signature.h>
#include <volk/volk_32fc_32f_multiply_32fc.h>
#include <volk/volk_32fc_x2_multiply_32fc.h>
#include <cmath>
#include <cstring>

using gr::fft::window;

namespace gr {
namespace qtgui {

fosphor_formatter_impl::fosphor_formatter_impl(int fft_size,
                                               int num_bins,
                                               int input_decim,
                                               int waterfall_decim,
                                               int histo_decim,
                                               double scale,
                                               double alpha,
                                               double epsilon,
                                               double trise,
                                               double tdecay)
    : block("histo_proc",
            gr::io_signature::make(1, 1, sizeof(gr_complex)),
            gr::io_signature::make(2, 2, sizeof(unsigned char) * fft_size)),
      d_fft_size(fft_size),
      d_num_bins(num_bins),
      d_input_decim(input_decim),
      d_waterfall_decim(waterfall_decim),
      d_histo_decim(histo_decim),
      d_scale(static_cast<float>(scale)),
      d_alpha(static_cast<float>(alpha)),
      d_epsilon(static_cast<float>(epsilon)),
      d_trise(static_cast<float>(trise)),
      d_tdecay(static_cast<float>(tdecay)),
      d_iir(fft_size, 0.0f),
      d_maxhold_buf(fft_size, 0.0f),
      d_histo_buf_f(fft_size * d_num_bins, 0.0f),
      d_histo_buf_b(fft_size * d_num_bins, 0),
      d_hit_count(fft_size * d_num_bins, 0),
      d_input_count(0),
      d_waterfall_count(0),
      d_histo_count(0)
{
    // Output is d_num_bins histogram levels, plus average and max rows. Ensure
    // there is space, in case output is generated.
    set_min_output_buffer(2 * (d_num_bins + 2));

    // TODO: make sure decims are >0

    d_fft = new gr::fft::fft_complex_fwd(fft_size);

    // TODO: make normalization an option
    const bool normalize = true;
    d_window = window::build(
        window::WIN_BLACKMAN_HARRIS, fft_size, window::INVALID_WIN_PARAM, normalize);

    d_logfft.resize(d_fft_size);
    d_logfft_b.resize(d_fft_size);
    d_logfft_avg_b.resize(d_fft_size);
}

fosphor_formatter_impl::~fosphor_formatter_impl()
{
    if (d_fft) {
        delete d_fft;
        d_fft = nullptr;
    }
}

void fosphor_formatter_impl::forecast(int noutput_items,
                                      gr_vector_int& ninput_items_required)
{
    // Require a full fft vector, as if input was vectorized.
    ninput_items_required[0] = d_fft_size;
}

int fosphor_formatter_impl::general_work(int noutput_items,
                                         gr_vector_int& ninput_items,
                                         gr_vector_const_void_star& input_items,
                                         gr_vector_void_star& output_items)
{
    const gr_complex* in = static_cast<const gr_complex*>(input_items[0]);
    uint8_t* out_histo = static_cast<uint8_t*>(output_items[0]);
    uint8_t* out_waterfall = static_cast<uint8_t*>(output_items[1]);

    float convert_mul = 3.5f;
    float convert_add = 120.0f;

    // Number of fft vectors
    int n_in = ninput_items[0] / d_fft_size;

    int n = 0;
    int n_histo = 0;
    int n_wf = 0;
    while (n < n_in) {

        // Decimate input by ignoring
        ++d_input_count;
        ++n;
        if (d_input_count < d_input_decim) {
            in += d_fft_size;
            continue;
        }
        d_input_count = 0;

        gr_complex* fft_in = d_fft->get_inbuf();
        volk_32fc_32f_multiply_32fc(fft_in, in, d_window.data(), d_fft_size);
        d_fft->execute();
        gr_complex* fft_out = d_fft->get_outbuf();
        const float scale_factor =
            1.0f * 1.0f / static_cast<float>(d_fft_size * d_fft_size);
        // values are divided by normalizationFactor before power calc, so invert
        volk_32fc_s32f_power_spectrum_32f(d_logfft.data() + d_fft_size / 2,
                                          fft_out,
                                          1.0f / scale_factor,
                                          d_fft_size / 2);
        volk_32fc_s32f_power_spectrum_32f(d_logfft.data(),
                                          fft_out + d_fft_size / 2,
                                          1.0f / scale_factor,
                                          d_fft_size / 2);
        volk_32f_s32f_add_32f(d_logfft.data(), d_logfft.data(), 100.0f, d_fft_size);

        // Decay previous max hold value
        volk_32f_s32f_multiply_32f(
            d_maxhold_buf.data(), d_maxhold_buf.data(), d_epsilon, d_fft_size);

        // Update max
        volk_32f_x2_max_32f(
            d_maxhold_buf.data(), d_maxhold_buf.data(), d_logfft.data(), d_fft_size);

        ++d_histo_count;
        if (d_histo_count == d_histo_decim) {

            ++n_histo;
            d_histo_count = 0;

            volk_32f_s32f_x2_convert_8u(
                d_logfft_b.data(), d_logfft.data(), convert_mul, convert_add, d_fft_size);

            // Update hit counter
            for (size_t i = 0; i < static_cast<size_t>(d_fft_size); i++) {
                // TODO: allow num_bins other than 64 (implied here)
                const uint8_t bin_index = d_logfft_b[i] >> 2;
                d_hit_count[bin_index * d_fft_size + i]++;
            }

            // Now update the output histogram buffer
            for (size_t i = 0; i < d_histo_buf_f.size(); ++i) {

                const int16_t hc = d_hit_count[i];
                float hv = d_histo_buf_f[i];

                // Subtract from previous histogram values proportional to d_tdecay,
                // and add in current hit count proportional to d_trise. Take
                // decimation into account. (This code could be just a bit clearer).

                // TODO: should be able to factor pow() out of the loop to help
                // performance.
                if (hv >= 0.01f || hc != 0) {
                    const float a = static_cast<float>(hc) / d_histo_decim;
                    const float b = a / d_trise;
                    const float c = b + 1.0f / d_tdecay;
                    const float d = b / c;
                    const float e = std::pow(1.0f - c, d_histo_decim);
                    hv = (hv - d) * e + d;
                    hv = std::max(std::min(1.0f, hv), 0.0f);
                    d_histo_buf_f[i] = hv;
                }
            }

            // Copy the histogram buffer
            volk_32f_s32f_x2_convert_8u(
                out_histo, d_histo_buf_f.data(), 256.0f, 0.0f, d_fft_size * d_num_bins);
            out_histo += d_fft_size * d_num_bins;
            // Copy max hold to out buffer
            volk_32f_s32f_x2_convert_8u(
                out_histo, d_maxhold_buf.data(), convert_mul, convert_add, d_fft_size);
            out_histo += d_fft_size;
            // Copy average to out buffer
            volk_32f_s32f_x2_convert_8u(
                out_histo, d_iir.data(), convert_mul, convert_add, d_fft_size);
            out_histo += d_fft_size;

            int num_items_written = n_histo * (d_num_bins + 2);
            // Insert stream tag
            auto tag = gr::tag_t{};
            tag.key = pmt::string_to_symbol("rx_eob");
            tag.value = pmt::PMT_T;
            tag.offset = nitems_written(0) + num_items_written - 1;
            add_item_tag(0, tag);

            // Reset hit counter
            std::fill(d_hit_count.begin(), d_hit_count.end(), 0);
        }

        // WF Decim (output f2b to output 1)
        ++d_waterfall_count;
        if (d_waterfall_count == d_waterfall_decim) {
            volk_32f_s32f_x2_convert_8u(
                out_waterfall, d_logfft.data(), convert_mul, convert_add, d_fft_size);
            d_waterfall_count = 0;
            ++n_wf;
            out_waterfall += d_fft_size;
        }

        in += d_fft_size;
    }

    // printf("%d %d %d\n", n_histo, n_wf, n);
    produce(0, n_histo * (d_num_bins + 2));
    produce(1, n_wf);
    consume(0, n * d_fft_size);

    return WORK_CALLED_PRODUCE;
}

fosphor_formatter::sptr fosphor_formatter::make(int fft_size,
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
    return gnuradio::make_block_sptr<fosphor_formatter_impl>(fft_size,
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

} // namespace qtgui
} // namespace gr

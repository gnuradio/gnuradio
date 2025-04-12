/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rational_resampler_impl.h"

/* ensure that tweakme.h is included before the bundled spdlog/fmt header, see
 * https://github.com/gabime/spdlog/issues/2922 */
#include <spdlog/tweakme.h>

#include <gnuradio/fft/window.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/io_signature.h>
#include <spdlog/fmt/fmt.h>
#include <numeric>
#include <stdexcept>

namespace gr {
namespace filter {


/**
 * @brief Given the interpolation rate, decimation rate and a fractional bandwidth,
    design a set of taps.
 *
 * Uses default parameters to build a low pass filter using a Kaiser Window
 *
 * @param interpolation interpolation factor (integer > 0)
 * @param decimation decimation factor (integer > 0)
 * @param fractional_bw fractional bandwidth in (0, 0.5)  0.4 works well. (float)
 */
template <typename TAP_T>
std::vector<TAP_T> design_resampler_filter(const unsigned interpolation,
                                           const unsigned decimation,
                                           const float fractional_bw)
{

    if (fractional_bw >= 0.5 || fractional_bw <= 0) {
        throw std::range_error(fmt::format(
            "Invalid fractional_bandwidth {:.2f}, must be in (0, 0.5)", fractional_bw));
    }

    // These are default values used to generate the filter when no taps are known
    //  Pulled from rational_resampler.py
    float beta = 7.0;
    float halfband = 0.5;
    float rate = float(interpolation) / float(decimation);
    float trans_width, mid_transition_band;

    if (rate >= 1.0) {
        trans_width = halfband - fractional_bw;
        mid_transition_band = halfband - trans_width / 2.0;
    } else {
        trans_width = rate * (halfband - fractional_bw);
        mid_transition_band = rate * halfband - trans_width / 2.0;
    }

    return firdes::low_pass(interpolation,       /* gain */
                            interpolation,       /* Fs */
                            mid_transition_band, /* trans mid point */
                            trans_width,         /* transition width */
                            fft::window::WIN_KAISER,
                            beta); /* beta*/
}

template <>
std::vector<gr_complex> design_resampler_filter<gr_complex>(const unsigned interpolation,
                                                            const unsigned decimation,
                                                            const float fractional_bw)
{
    auto real_taps =
        design_resampler_filter<float>(interpolation, decimation, fractional_bw);

    std::vector<gr_complex> cplx_taps(real_taps.size());
    for (size_t i = 0; i < real_taps.size(); i++) {
        cplx_taps[i] = real_taps[i];
    }

    return cplx_taps;
}

template <class IN_T, class OUT_T, class TAP_T>
typename rational_resampler<IN_T, OUT_T, TAP_T>::sptr
rational_resampler<IN_T, OUT_T, TAP_T>::make(unsigned interpolation,
                                             unsigned decimation,
                                             const std::vector<TAP_T>& taps,
                                             float fractional_bw)
{
    return gnuradio::make_block_sptr<rational_resampler_impl<IN_T, OUT_T, TAP_T>>(
        interpolation, decimation, taps, fractional_bw);
}

template <class IN_T, class OUT_T, class TAP_T>
rational_resampler_impl<IN_T, OUT_T, TAP_T>::rational_resampler_impl(
    unsigned interpolation,
    unsigned decimation,
    const std::vector<TAP_T>& taps,
    float fractional_bw)
    : block("rational_resampler<IN_T,OUT_T,TAP_T>",
            io_signature::make(1, 1, sizeof(IN_T)),
            io_signature::make(1, 1, sizeof(OUT_T)))
{
    if (interpolation == 0) {
        throw std::out_of_range(
            "rational_resampler_impl<IN_T,OUT_T,TAP_T>: interpolation must be > 0");
    }
    if (decimation == 0) {
        throw std::out_of_range(
            "rational_resampler_impl<IN_T,OUT_T,TAP_T>: decimation must be > 0");
    }

    // If taps are not specified, we need to design them based on fractional_bw
    if (taps.empty() && fractional_bw <= 0) {
        fractional_bw = 0.4;
    }

    auto d = std::gcd(interpolation, decimation);

    if (!taps.empty() && (d > 1)) {
        this->d_logger->info(
            "Rational resampler has user-provided taps but interpolation ({:d}) and "
            "decimation ({:d}) have a GCD of {:d}, which increases the complexity of "
            "the filterbank. Consider reducing these values by the GCD.",
            interpolation,
            decimation,
            d);
    }

    std::vector<TAP_T> staps;
    if (taps.empty()) {
        if (fractional_bw <= 0) {
            fractional_bw = 0.4;
        }
        // If we don't have user-provided taps, reduce the interp and
        // decim values by the GCD (if there is one) and then define
        // the taps from these new values.
        interpolation /= d;
        decimation /= d;
        staps = design_resampler_filter<TAP_T>(interpolation, decimation, fractional_bw);
    } else {
        staps = taps;
    }

    d_decimation = decimation;

    this->set_relative_rate(uint64_t{ interpolation }, uint64_t{ decimation });
    this->set_output_multiple(1);

    d_firs.reserve(interpolation);
    for (unsigned i = 0; i < interpolation; i++) {
        d_firs.emplace_back(std::vector<TAP_T>());
    }

    set_taps(staps);
    install_taps(d_new_taps);
}

template <class IN_T, class OUT_T, class TAP_T>
void rational_resampler_impl<IN_T, OUT_T, TAP_T>::set_taps(const std::vector<TAP_T>& taps)
{
    d_new_taps = taps;
    d_updated = true;

    // round up length to a multiple of the interpolation factor
    int n = taps.size() % this->interpolation();
    if (n > 0) {
        n = this->interpolation() - n;
        while (n-- > 0) {
            d_new_taps.insert(d_new_taps.end(), 0);
        }
    }

    assert(d_new_taps.size() % this->interpolation() == 0);
}

template <class IN_T, class OUT_T, class TAP_T>
void rational_resampler_impl<IN_T, OUT_T, TAP_T>::install_taps(
    const std::vector<TAP_T>& taps)
{
    int nfilters = this->interpolation();
    int nt = taps.size() / nfilters;

    assert(nt * nfilters == (int)taps.size());

    std::vector<std::vector<TAP_T>> xtaps(nfilters);

    for (int n = 0; n < nfilters; n++)
        xtaps[n].resize(nt);

    for (int i = 0; i < (int)taps.size(); i++)
        xtaps[i % nfilters][i / nfilters] = taps[i];

    for (int n = 0; n < nfilters; n++)
        d_firs[n].set_taps(xtaps[n]);

    set_history(nt);
    d_updated = false;
}

template <class IN_T, class OUT_T, class TAP_T>
std::vector<TAP_T> rational_resampler_impl<IN_T, OUT_T, TAP_T>::taps() const
{
    return d_new_taps;
}

template <class IN_T, class OUT_T, class TAP_T>
void rational_resampler_impl<IN_T, OUT_T, TAP_T>::forecast(
    int noutput_items, gr_vector_int& ninput_items_required)
{
    int nreqd = std::max(
        1U,
        (int)((double)(noutput_items + 1) * this->decimation() / this->interpolation()) +
            history() - 1);
    unsigned ninputs = ninput_items_required.size();
    for (unsigned i = 0; i < ninputs; i++)
        ninput_items_required[i] = nreqd;
}

template <class IN_T, class OUT_T, class TAP_T>
int rational_resampler_impl<IN_T, OUT_T, TAP_T>::general_work(
    int noutput_items,
    gr_vector_int& ninput_items,
    gr_vector_const_void_star& input_items,
    gr_vector_void_star& output_items)
{
    auto in = reinterpret_cast<const IN_T*>(input_items[0]);
    auto out = reinterpret_cast<OUT_T*>(output_items[0]);

    if (d_updated) {
        install_taps(d_new_taps);
        return 0; // history requirement may have increased.
    }

    unsigned int ctr = d_ctr;
    int count = 0;

    int i = 0;
    while ((i < noutput_items) && (count < ninput_items[0])) {
        out[i++] = d_firs[ctr].filter(in);
        ctr += this->decimation();
        while (ctr >= this->interpolation()) {
            ctr -= this->interpolation();
            in++;
            count++;
        }
    }

    d_ctr = ctr;
    this->consume_each(count);
    return i;
}
template class rational_resampler<gr_complex, gr_complex, gr_complex>;
template class rational_resampler<gr_complex, gr_complex, float>;
template class rational_resampler<float, gr_complex, gr_complex>;
template class rational_resampler<float, float, float>;
template class rational_resampler<float, std::int16_t, float>;
template class rational_resampler<std::int16_t, gr_complex, gr_complex>;

} /* namespace filter */
} /* namespace gr */

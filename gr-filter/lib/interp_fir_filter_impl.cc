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

#include "interp_fir_filter_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <stdexcept>

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
typename interp_fir_filter<IN_T, OUT_T, TAP_T>::sptr
interp_fir_filter<IN_T, OUT_T, TAP_T>::make(unsigned interpolation,
                                            const std::vector<TAP_T>& taps)
{
    return gnuradio::make_block_sptr<interp_fir_filter_impl<IN_T, OUT_T, TAP_T>>(
        interpolation, taps);
}

template <class IN_T, class OUT_T, class TAP_T>
interp_fir_filter_impl<IN_T, OUT_T, TAP_T>::interp_fir_filter_impl(
    unsigned interpolation, const std::vector<TAP_T>& taps)
    : sync_interpolator("interp_fir_filter<IN_T,OUT_T,TAP_T>",
                        io_signature::make(1, 1, sizeof(IN_T)),
                        io_signature::make(1, 1, sizeof(OUT_T)),
                        interpolation),
      d_updated(false)
{
    if (interpolation == 0) {
        throw std::out_of_range("interp_fir_filter_impl: interpolation must be > 0");
    }

    if (taps.empty()) {
        throw std::runtime_error("interp_fir_filter_impl: no filter taps provided.");
    }

    std::vector<TAP_T> dummy_taps;

    d_firs.reserve(interpolation);
    for (unsigned i = 0; i < interpolation; i++) {
        d_firs.emplace_back(dummy_taps);
    }

    set_taps(taps);
    install_taps(d_new_taps);
}

template <class IN_T, class OUT_T, class TAP_T>
void interp_fir_filter_impl<IN_T, OUT_T, TAP_T>::set_taps(const std::vector<TAP_T>& taps)
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

    if (d_new_taps.size() % this->interpolation() != 0) {
        throw std::runtime_error(
            "interp_fir_filter_impl: error setting interpolator taps.");
    }
}

template <class IN_T, class OUT_T, class TAP_T>
void interp_fir_filter_impl<IN_T, OUT_T, TAP_T>::install_taps(
    const std::vector<TAP_T>& taps)
{
    unsigned nfilters = this->interpolation();
    int nt = taps.size() / nfilters;

    std::vector<std::vector<TAP_T>> xtaps(nfilters);

    for (unsigned n = 0; n < nfilters; n++) {
        xtaps[n].resize(nt);
    }

    for (size_t i = 0; i < taps.size(); i++) {
        xtaps[i % nfilters][i / nfilters] = taps[i];
    }

    for (unsigned n = 0; n < nfilters; n++) {
        d_firs[n].set_taps(xtaps[n]);
    }

    this->set_history(nt);
    d_updated = false;
}

template <class IN_T, class OUT_T, class TAP_T>
std::vector<TAP_T> interp_fir_filter_impl<IN_T, OUT_T, TAP_T>::taps() const
{
    return d_new_taps;
}

template <class IN_T, class OUT_T, class TAP_T>
int interp_fir_filter_impl<IN_T, OUT_T, TAP_T>::work(
    int noutput_items,
    gr_vector_const_void_star& input_items,
    gr_vector_void_star& output_items)
{
    const IN_T* in = (const IN_T*)input_items[0];
    OUT_T* out = (OUT_T*)output_items[0];

    if (d_updated) {
        install_taps(d_new_taps);
        return 0; // history requirements may have changed.
    }

    int nfilters = this->interpolation();
    int ni = noutput_items / this->interpolation();

    for (int i = 0; i < ni; i++) {
        for (int nf = 0; nf < nfilters; nf++) {
            out[nf] = d_firs[nf].filter(&in[i]);
        }
        out += nfilters;
    }

    return noutput_items;
}
template class interp_fir_filter<gr_complex, gr_complex, gr_complex>;
template class interp_fir_filter<gr_complex, gr_complex, float>;
template class interp_fir_filter<float, gr_complex, gr_complex>;
template class interp_fir_filter<float, float, float>;
template class interp_fir_filter<float, std::int16_t, float>;
template class interp_fir_filter<std::int16_t, gr_complex, gr_complex>;


} /* namespace filter */
} /* namespace gr */

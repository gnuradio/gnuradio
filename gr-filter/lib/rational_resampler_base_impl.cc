/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rational_resampler_base_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <stdexcept>

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
typename rational_resampler_base<IN_T, OUT_T, TAP_T>::sptr
rational_resampler_base<IN_T, OUT_T, TAP_T>::make(unsigned interpolation,
                                                  unsigned decimation,
                                                  const std::vector<TAP_T>& taps)
{
    return gnuradio::get_initial_sptr(
        new rational_resampler_base_impl<IN_T, OUT_T, TAP_T>(
            interpolation, decimation, taps));
}

template <class IN_T, class OUT_T, class TAP_T>
rational_resampler_base_impl<IN_T, OUT_T, TAP_T>::rational_resampler_base_impl(
    unsigned interpolation, unsigned decimation, const std::vector<TAP_T>& taps)
    : block("rational_resampler_base<IN_T,OUT_T,TAP_T>",
            io_signature::make(1, 1, sizeof(IN_T)),
            io_signature::make(1, 1, sizeof(OUT_T))),
      d_history(1),
      d_interpolation(interpolation),
      d_decimation(decimation),
      d_ctr(0),
      d_firs(interpolation),
      d_updated(false)
{
    if (interpolation == 0)
        throw std::out_of_range(
            "rational_resampler_base_impl<IN_T,OUT_T,TAP_T>: interpolation must be > 0");
    if (decimation == 0)
        throw std::out_of_range(
            "rational_resampler_base_impl<IN_T,OUT_T,TAP_T>: decimation must be > 0");

    this->set_relative_rate((uint64_t)interpolation, (uint64_t)decimation);
    this->set_output_multiple(1);

    std::vector<TAP_T> dummy_taps;

    for (unsigned i = 0; i < interpolation; i++) {
        d_firs[i] = new kernel::fir_filter<IN_T, OUT_T, TAP_T>(1, dummy_taps);
    }

    set_taps(taps);
    install_taps(d_new_taps);
}

template <class IN_T, class OUT_T, class TAP_T>
rational_resampler_base_impl<IN_T, OUT_T, TAP_T>::~rational_resampler_base_impl()
{
    int interp = this->interpolation();
    for (int i = 0; i < interp; i++) {
        delete d_firs[i];
    }
}

template <class IN_T, class OUT_T, class TAP_T>
void rational_resampler_base_impl<IN_T, OUT_T, TAP_T>::set_taps(
    const std::vector<TAP_T>& taps)
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
void rational_resampler_base_impl<IN_T, OUT_T, TAP_T>::install_taps(
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
        d_firs[n]->set_taps(xtaps[n]);

    set_history(nt);
    d_updated = false;
}

template <class IN_T, class OUT_T, class TAP_T>
std::vector<TAP_T> rational_resampler_base_impl<IN_T, OUT_T, TAP_T>::taps() const
{
    return d_new_taps;
}

template <class IN_T, class OUT_T, class TAP_T>
void rational_resampler_base_impl<IN_T, OUT_T, TAP_T>::forecast(
    int noutput_items, gr_vector_int& ninput_items_required)
{
    int nreqd = std::max(
        (unsigned)1,
        (int)((double)(noutput_items + 1) * this->decimation() / this->interpolation()) +
            history() - 1);
    unsigned ninputs = ninput_items_required.size();
    for (unsigned i = 0; i < ninputs; i++)
        ninput_items_required[i] = nreqd;
}

template <class IN_T, class OUT_T, class TAP_T>
int rational_resampler_base_impl<IN_T, OUT_T, TAP_T>::general_work(
    int noutput_items,
    gr_vector_int& ninput_items,
    gr_vector_const_void_star& input_items,
    gr_vector_void_star& output_items)
{
    const IN_T* in = (const IN_T*)input_items[0];
    OUT_T* out = (OUT_T*)output_items[0];

    if (d_updated) {
        install_taps(d_new_taps);
        return 0; // history requirement may have increased.
    }

    unsigned int ctr = d_ctr;
    int count = 0;

    int i = 0;
    while ((i < noutput_items) && (count < ninput_items[0])) {
        out[i++] = d_firs[ctr]->filter(in);
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
template class rational_resampler_base<gr_complex, gr_complex, gr_complex>;
template class rational_resampler_base<gr_complex, gr_complex, float>;
template class rational_resampler_base<float, gr_complex, gr_complex>;
template class rational_resampler_base<float, float, float>;
template class rational_resampler_base<float, std::int16_t, float>;
template class rational_resampler_base<std::int16_t, gr_complex, gr_complex>;

} /* namespace filter */
} /* namespace gr */

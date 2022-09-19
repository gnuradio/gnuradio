/* -*- c++ -*- */
/*
 * Copyright 2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#include <gnuradio/kernel/filter/pfb_arb_resampler.h>
#include <gnuradio/kernel/math/math.h>
#include <gnuradio/logger.h>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace gr {
namespace kernel {
namespace filter {

template <class T_IN, class T_OUT, class TAP_T>
void pfb_arb_resampler<T_IN, T_OUT, TAP_T>::set_taps(const std::vector<TAP_T>& taps)
{
    std::vector<TAP_T> dtaps;
    create_diff_taps(taps, dtaps);
    create_taps(taps, d_taps, d_filters);
    create_taps(dtaps, d_dtaps, d_diff_filters);
}


template <class T_IN, class T_OUT, class TAP_T>
void pfb_arb_resampler<T_IN, T_OUT, TAP_T>::set_rate(float rate)
{
    d_dec_rate = (size_t)floor(d_int_rate / rate);
    d_flt_rate = (d_int_rate / rate) - d_dec_rate;
}

template <class T_IN, class T_OUT, class TAP_T>
void pfb_arb_resampler<T_IN, T_OUT, TAP_T>::set_phase(float ph)
{
    if ((ph < 0) || (ph >= 2.0 * GR_M_PI)) {
        throw std::runtime_error(
            "pfb_arb_resampler_ccf: set_phase value out of bounds [0, 2pi).");
    }

    float ph_diff = 2.0 * GR_M_PI / (float)d_filters.size();
    d_last_filter = static_cast<int>(ph / ph_diff);
}

template <class T_IN, class T_OUT, class TAP_T>
float pfb_arb_resampler<T_IN, T_OUT, TAP_T>::phase() const
{
    float ph_diff = 2.0 * GR_M_PI / static_cast<float>(d_filters.size());
    return d_last_filter * ph_diff;
}

template <class T_IN, class T_OUT, class TAP_T>
float pfb_arb_resampler<T_IN, T_OUT, TAP_T>::phase_offset(float freq, float fs) const
{
    float adj = (2.0 * GR_M_PI) * (freq / fs) / static_cast<float>(d_int_rate);
    return -adj * d_est_phase_change;
}

template <class T_IN, class T_OUT, class TAP_T>
void pfb_arb_resampler<T_IN, T_OUT, TAP_T>::create_taps(
    const std::vector<TAP_T>& newtaps,
    std::vector<std::vector<TAP_T>>& ourtaps,
    std::vector<fir_filter<T_IN, T_OUT, TAP_T>>& ourfilter)
{
    size_t ntaps = newtaps.size();
    d_taps_per_filter = (size_t)ceil((double)ntaps / (double)d_int_rate);

    // Create d_numchan vectors to store each channel's taps
    ourtaps.resize(d_int_rate);

    // Make a vector of the taps plus fill it out with 0's to fill
    // each polyphase filter with exactly d_taps_per_filter
    std::vector<TAP_T> tmp_taps;
    tmp_taps = newtaps;
    while ((float)(tmp_taps.size()) < d_int_rate * d_taps_per_filter) {
        tmp_taps.push_back(0.0);
    }

    for (size_t i = 0; i < d_int_rate; i++) {
        // Each channel uses all d_taps_per_filter with 0's if not enough taps to fill out
        ourtaps[i] = std::vector<TAP_T>(d_taps_per_filter, 0);
        for (size_t j = 0; j < d_taps_per_filter; j++) {
            ourtaps[i][j] = tmp_taps[i + j * d_int_rate];
        }

        // Build a filter for each channel and add it's taps to it
        ourfilter[i].set_taps(ourtaps[i]);
    }
}
template <class T_IN, class T_OUT, class TAP_T>
void pfb_arb_resampler<T_IN, T_OUT, TAP_T>::create_diff_taps(
    const std::vector<TAP_T>& newtaps, std::vector<TAP_T>& difftaps)
{
    // Calculate the differential taps using a derivative filter
    std::vector<TAP_T> diff_filter(2);
    diff_filter[0] = -1;
    diff_filter[1] = 1;

    for (size_t i = 0; i < newtaps.size() - 1; i++) {
        TAP_T tap = 0;
        for (size_t j = 0; j < diff_filter.size(); j++) {
            tap += diff_filter[j] * newtaps[i + j];
        }
        difftaps.push_back(tap);
    }
    difftaps.push_back(0);
}

template <class T_IN, class T_OUT, class TAP_T>
std::vector<std::vector<TAP_T>> pfb_arb_resampler<T_IN, T_OUT, TAP_T>::taps() const
{
    return d_taps;
}

template <class T_IN, class T_OUT, class TAP_T>
void pfb_arb_resampler<T_IN, T_OUT, TAP_T>::print_taps()
{
    size_t i, j;
    std::cout << std::setprecision(4);
    for (i = 0; i < d_int_rate; i++) {
        std::cout << "filter[" << i << "]: [";
        for (j = 0; j < d_taps_per_filter; j++) {
            std::cout << " " << d_taps[i][j];
        }
        std::cout << "]" << std::endl;
    }
}


template <class T_IN, class T_OUT, class TAP_T>
int pfb_arb_resampler<T_IN, T_OUT, TAP_T>::filter(T_OUT* output,
                                                  const T_IN* input,
                                                  int n_to_read,
                                                  int& n_read)
{
    int i_out = 0, i_in = 0;
    size_t j = d_last_filter;
    ;
    T_OUT o0, o1;

    while (i_in < n_to_read) {
        // start j by wrapping around mod the number of channels
        while (j < d_int_rate) {
            // Take the current filter and derivative filter output
            o0 = d_filters[j].filter(&input[i_in]);
            o1 = d_diff_filters[j].filter(&input[i_in]);

            output[i_out] = o0 + o1 * d_acc; // linearly interpolate between samples
            i_out++;

            // Adjust accumulator and index into filterbank
            d_acc += d_flt_rate;
            j += d_dec_rate + (int)floor(d_acc);
            d_acc = fmodf(d_acc, 1.0);
        }
        i_in += (int)(j / d_int_rate);
        j = j % d_int_rate;
    }
    d_last_filter = j; // save last filter state for re-entry

    n_read = i_in; // return how much we've actually read
    return i_out;  // return how much we've produced
}


template <class T_IN, class T_OUT, class TAP_T>
pfb_arb_resampler<T_IN, T_OUT, TAP_T>::pfb_arb_resampler(float rate,
                                                         const std::vector<TAP_T>& taps,
                                                         size_t filter_size)
{
    d_acc = 0; // start accumulator at 0

    /* The number of filters is specified by the user as the
       filter size; this is also the interpolation rate of the
       filter. We use it and the rate provided to determine the
       decimation rate. This acts as a rational resampler. The
       flt_rate is calculated as the residual between the integer
       decimation rate and the real decimation rate and will be
       used to determine to interpolation point of the resampling
       process.
    */
    d_int_rate = filter_size;
    set_rate(rate);

    d_last_filter = (taps.size() / 2) % filter_size;

    // Create an FIR filter for each channel and zero out the taps
    const std::vector<TAP_T> vtaps(0, d_int_rate);
    d_filters.reserve(d_int_rate);
    d_diff_filters.reserve(d_int_rate);
    for (size_t i = 0; i < d_int_rate; i++) {
        d_filters.emplace_back(vtaps);
        d_diff_filters.emplace_back(vtaps);
    }

    // Now, actually set the filters' taps
    set_taps(taps);

    // Delay is based on number of taps per filter arm. Round to
    // the nearest integer.
    float delay = rate * (taps_per_filter() - 1.0) / 2.0;
    d_delay = static_cast<int>(std::lround(delay));

    // This calculation finds the phase offset induced by the
    // arbitrary resampling. It's based on which filter arm we are
    // at the filter's group delay plus the fractional offset
    // between the samples. Calculated here based on the rotation
    // around nfilts starting at start_filter.
    float accum = d_delay * d_flt_rate;
    int accum_int = static_cast<int>(accum);
    float accum_frac = accum - accum_int;
    int end_filter = static_cast<int>(
        std::lround(fmodf(d_last_filter + d_delay * d_dec_rate + accum_int,
                          static_cast<float>(d_int_rate))));

    d_est_phase_change = d_last_filter - (end_filter + accum_frac);
}

template class pfb_arb_resampler<gr_complex, gr_complex, gr_complex>;
template class pfb_arb_resampler<gr_complex, gr_complex, float>;
template class pfb_arb_resampler<float, float, float>;


} // namespace filter
} // namespace kernel
} // namespace gr

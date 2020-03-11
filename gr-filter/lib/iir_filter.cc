/* -*- c++ -*- */
/*
 * Copyright 2002,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/filter/iir_filter.h>

namespace gr {
namespace filter {
namespace kernel {

template <>
gr_complex
iir_filter<gr_complex, gr_complex, float, gr_complex>::filter(const gr_complex input)
{
    gr_complex acc;
    unsigned i = 0;
    unsigned n = ntaps_ff();
    unsigned m = ntaps_fb();

    if (n == 0)
        return (gr_complex)0;

    int latest_n = d_latest_n;
    int latest_m = d_latest_m;

    acc = d_fftaps[0] * input;
    for (i = 1; i < n; i++)
        acc += (d_fftaps[i] * d_prev_input[latest_n + i]);
    for (i = 1; i < m; i++)
        acc += (d_fbtaps[i] * d_prev_output[latest_m + i]);

    // store the values twice to avoid having to handle wrap-around in the loop
    d_prev_output[latest_m] = acc;
    d_prev_output[latest_m + m] = acc;
    d_prev_input[latest_n] = input;
    d_prev_input[latest_n + n] = input;

    latest_n--;
    latest_m--;
    if (latest_n < 0)
        latest_n += n;
    if (latest_m < 0)
        latest_m += m;

    d_latest_m = latest_m;
    d_latest_n = latest_n;
    return static_cast<gr_complex>(acc);
}

template <>
gr_complex
iir_filter<gr_complex, gr_complex, double, gr_complexd>::filter(const gr_complex input)
{
    gr_complexd acc;
    unsigned i = 0;
    unsigned n = ntaps_ff();
    unsigned m = ntaps_fb();

    if (n == 0)
        return (gr_complex)0;

    int latest_n = d_latest_n;
    int latest_m = d_latest_m;

    acc = d_fftaps[0] * static_cast<gr_complexd>(input);
    for (i = 1; i < n; i++)
        acc += (d_fftaps[i] * static_cast<gr_complexd>(d_prev_input[latest_n + i]));
    for (i = 1; i < m; i++)
        acc += (d_fbtaps[i] * static_cast<gr_complexd>(d_prev_output[latest_m + i]));

    // store the values twice to avoid having to handle wrap-around in the loop
    d_prev_output[latest_m] = acc;
    d_prev_output[latest_m + m] = acc;
    d_prev_input[latest_n] = input;
    d_prev_input[latest_n + n] = input;

    latest_n--;
    latest_m--;
    if (latest_n < 0)
        latest_n += n;
    if (latest_m < 0)
        latest_m += m;

    d_latest_m = latest_m;
    d_latest_n = latest_n;
    return static_cast<gr_complex>(acc);
}

template <>
gr_complex iir_filter<gr_complex, gr_complex, gr_complexd, gr_complexd>::filter(
    const gr_complex input)
{
    gr_complexd acc;
    unsigned i = 0;
    unsigned n = ntaps_ff();
    unsigned m = ntaps_fb();

    if (n == 0)
        return (gr_complex)0;

    int latest_n = d_latest_n;
    int latest_m = d_latest_m;

    acc = d_fftaps[0] * static_cast<gr_complexd>(input);
    for (i = 1; i < n; i++)
        acc += (d_fftaps[i] * static_cast<gr_complexd>(d_prev_input[latest_n + i]));
    for (i = 1; i < m; i++)
        acc += (d_fbtaps[i] * static_cast<gr_complexd>(d_prev_output[latest_m + i]));

    // store the values twice to avoid having to handle wrap-around in the loop
    d_prev_output[latest_m] = acc;
    d_prev_output[latest_m + m] = acc;
    d_prev_input[latest_n] = input;
    d_prev_input[latest_n + n] = input;

    latest_n--;
    latest_m--;
    if (latest_n < 0)
        latest_n += n;
    if (latest_m < 0)
        latest_m += m;

    d_latest_m = latest_m;
    d_latest_n = latest_n;
    return static_cast<gr_complex>(acc);
}

} /* namespace kernel */
} /* namespace filter */
} /* namespace gr */

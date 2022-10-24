/* -*- c++ -*- */
/*
 * Copyright 2010,2012,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#include <gnuradio/kernel/fft/fftw_fft.h>
#include <gnuradio/kernel/filter/fir_filter_with_buffer.h>
#include <volk/volk.h>
#include <algorithm>
#include <cstdio>
#include <cstring>

namespace gr {
namespace kernel {
namespace filter {


template <>
void fir_filter_with_buffer<float, float>::set_taps(const std::vector<float>& taps)
{
    d_ntaps = (int)taps.size();
    d_taps = taps;
    std::reverse(d_taps.begin(), d_taps.end());

    // We allocate enough to be able to look back and forth
    // d_naligned beyond the buffer boundaries and make sure these
    // are zeroed out (or they may be nan, which will cause
    // problems). We then set d_buffer to the position in the
    // d_buffer_ptr such that we only touch the internally
    // allocated space.
    d_buffer_ptr.clear();
    d_buffer_ptr.resize(2 * (d_ntaps + d_naligned));
    d_buffer = &d_buffer_ptr[d_naligned];

    // Allocate aligned taps
    for (int i = 0; i < d_naligned; i++) {
        d_aligned_taps[i].clear();
        d_aligned_taps[i].resize(d_ntaps + d_naligned - 1);
        std::copy(std::begin(d_taps), std::end(d_taps), &d_aligned_taps[i][i]);
    }

    d_idx = 0;
}

template <>
fir_filter_with_buffer<float, float>::fir_filter_with_buffer(
    const std::vector<float>& taps)
    : d_output(1),
      d_align(volk_get_alignment()),
      d_naligned(std::max((size_t)1, d_align / sizeof(float))),
      d_aligned_taps(d_naligned)
{
    set_taps(taps);
}

template <>
std::vector<float> fir_filter_with_buffer<float, float>::taps() const
{
    std::vector<float> t = d_taps;
    std::reverse(t.begin(), t.end());
    return t;
}

template <>
float fir_filter_with_buffer<float, float>::filter(float input)
{
    d_buffer[d_idx] = input;
    d_buffer[d_idx + ntaps()] = input;

    d_idx++;
    if (d_idx >= ntaps())
        d_idx = 0;

    const float* ar = (float*)((size_t)(&d_buffer[d_idx]) & ~(d_align - 1));
    unsigned al = (&d_buffer[d_idx]) - ar;

    volk_32f_x2_dot_prod_32f_a(
        d_output.data(), ar, d_aligned_taps[al].data(), ntaps() + al);
    return d_output[0];
}

template <>
float fir_filter_with_buffer<float, float>::filter(const float input[], size_t dec)
{
    size_t i;

    for (i = 0; i < dec; i++) {
        d_buffer[d_idx] = input[i];
        d_buffer[d_idx + ntaps()] = input[i];
        d_idx++;
        if (d_idx >= ntaps())
            d_idx = 0;
    }

    const float* ar = (float*)((size_t)(&d_buffer[d_idx]) & ~(d_align - 1));
    unsigned al = (&d_buffer[d_idx]) - ar;

    volk_32f_x2_dot_prod_32f_a(
        d_output.data(), ar, d_aligned_taps[al].data(), ntaps() + al);
    return d_output[0];
}

template <>
void fir_filter_with_buffer<float, float>::filterN(float output[],
                                                   const float input[],
                                                   size_t n)
{
    for (size_t i = 0; i < n; i++) {
        output[i] = filter(input[i]);
    }
}

template <>
void fir_filter_with_buffer<float, float>::filterNdec(float output[],
                                                      const float input[],
                                                      size_t n,
                                                      size_t decimate)
{
    size_t j = 0;
    for (size_t i = 0; i < n; i++) {
        output[i] = filter(&input[j], decimate);
        j += decimate;
    }
}


/**************************************************************/


template <>
void fir_filter_with_buffer<gr_complex, gr_complex>::set_taps(
    const std::vector<gr_complex>& taps)
{
    d_ntaps = (int)taps.size();
    d_taps = taps;
    std::reverse(d_taps.begin(), d_taps.end());

    // We allocate enough to be able to look back and forth
    // d_naligned beyond the buffer boundaries and make sure these
    // are zeroed out (or they may be nan, which will cause
    // problems). We then set d_buffer to the position in the
    // d_buffer_ptr such that we only touch the internally
    // allocated space.
    d_buffer_ptr.clear();
    d_buffer_ptr.resize(2 * (d_ntaps + d_naligned));
    d_buffer = &d_buffer_ptr[d_naligned];

    // Allocate aligned taps
    for (int i = 0; i < d_naligned; i++) {
        d_aligned_taps[i].clear();
        d_aligned_taps[i].resize(d_ntaps + d_naligned - 1);
        std::copy(std::begin(d_taps), std::end(d_taps), &d_aligned_taps[i][i]);
    }

    d_idx = 0;
}

template <>
fir_filter_with_buffer<gr_complex, gr_complex>::fir_filter_with_buffer(
    const std::vector<gr_complex>& taps)
    : d_output(1),
      d_align(volk_get_alignment()),
      d_naligned(std::max((size_t)1, d_align / sizeof(gr_complex))),
      d_aligned_taps(d_naligned)
{
    set_taps(taps);
}


template <>
std::vector<gr_complex> fir_filter_with_buffer<gr_complex, gr_complex>::taps() const
{
    std::vector<gr_complex> t = d_taps;
    std::reverse(t.begin(), t.end());
    return t;
}

template <>
gr_complex fir_filter_with_buffer<gr_complex, gr_complex>::filter(gr_complex input)
{
    d_buffer[d_idx] = input;
    d_buffer[d_idx + ntaps()] = input;

    d_idx++;
    if (d_idx >= ntaps())
        d_idx = 0;

    const gr_complex* ar = (gr_complex*)((size_t)(&d_buffer[d_idx]) & ~(d_align - 1));
    unsigned al = (&d_buffer[d_idx]) - ar;

    volk_32fc_x2_dot_prod_32fc_a(
        d_output.data(), ar, d_aligned_taps[al].data(), (ntaps() + al));
    return d_output[0];
}

template <>
gr_complex
fir_filter_with_buffer<gr_complex, gr_complex>::filter(const gr_complex input[],
                                                       size_t dec)
{
    size_t i;

    for (i = 0; i < dec; i++) {
        d_buffer[d_idx] = input[i];
        d_buffer[d_idx + ntaps()] = input[i];
        d_idx++;
        if (d_idx >= ntaps())
            d_idx = 0;
    }

    const gr_complex* ar = (gr_complex*)((size_t)(&d_buffer[d_idx]) & ~(d_align - 1));
    unsigned al = (&d_buffer[d_idx]) - ar;

    volk_32fc_x2_dot_prod_32fc_a(
        d_output.data(), ar, d_aligned_taps[al].data(), (ntaps() + al));
    return d_output[0];
}

template <>
void fir_filter_with_buffer<gr_complex, gr_complex>::filterN(gr_complex output[],
                                                             const gr_complex input[],
                                                             size_t n)
{
    for (size_t i = 0; i < n; i++) {
        output[i] = filter(input[i]);
    }
}

template <>
void fir_filter_with_buffer<gr_complex, gr_complex>::filterNdec(gr_complex output[],
                                                                const gr_complex input[],
                                                                size_t n,
                                                                size_t decimate)
{
    size_t j = 0;
    for (size_t i = 0; i < n; i++) {
        output[i] = filter(&input[j], decimate);
        j += decimate;
    }
}


/**************************************************************/


template <>
void fir_filter_with_buffer<gr_complex, float>::set_taps(const std::vector<float>& taps)
{
    d_ntaps = (int)taps.size();
    d_taps = taps;
    std::reverse(d_taps.begin(), d_taps.end());

    // We allocate enough to be able to look back and forth
    // d_naligned beyond the buffer boundaries and make sure these
    // are zeroed out (or they may be nan, which will cause
    // problems). We then set d_buffer to the position in the
    // d_buffer_ptr such that we only touch the internally
    // allocated space.
    d_buffer_ptr.clear();
    d_buffer_ptr.resize(2 * (d_ntaps + d_naligned));
    d_buffer = &d_buffer_ptr[d_naligned];

    // Allocate aligned taps
    for (int i = 0; i < d_naligned; i++) {
        d_aligned_taps[i].clear();
        d_aligned_taps[i].resize(d_ntaps + d_naligned - 1);
        std::copy(std::begin(d_taps), std::end(d_taps), &d_aligned_taps[i][i]);
    }

    d_idx = 0;
}

template <>
fir_filter_with_buffer<gr_complex, float>::fir_filter_with_buffer(
    const std::vector<float>& taps)
    : d_output(1),
      d_align(volk_get_alignment()),
      d_naligned(std::max((size_t)1, d_align / sizeof(gr_complex))),
      d_aligned_taps(d_naligned)
{
    set_taps(taps);
}

template <>
std::vector<float> fir_filter_with_buffer<gr_complex, float>::taps() const
{
    std::vector<float> t = d_taps;
    std::reverse(t.begin(), t.end());
    return t;
}

template <>
gr_complex fir_filter_with_buffer<gr_complex, float>::filter(gr_complex input)
{
    d_buffer[d_idx] = input;
    d_buffer[d_idx + ntaps()] = input;

    d_idx++;
    if (d_idx >= ntaps())
        d_idx = 0;

    const gr_complex* ar = (gr_complex*)((size_t)(&d_buffer[d_idx]) & ~(d_align - 1));
    unsigned al = (&d_buffer[d_idx]) - ar;

    volk_32fc_32f_dot_prod_32fc_a(
        d_output.data(), ar, d_aligned_taps[al].data(), ntaps() + al);
    return d_output[0];
}

template <>
gr_complex fir_filter_with_buffer<gr_complex, float>::filter(const gr_complex input[],
                                                             size_t dec)
{
    size_t i;

    for (i = 0; i < dec; i++) {
        d_buffer[d_idx] = input[i];
        d_buffer[d_idx + ntaps()] = input[i];
        d_idx++;
        if (d_idx >= ntaps())
            d_idx = 0;
    }

    const gr_complex* ar = (gr_complex*)((size_t)(&d_buffer[d_idx]) & ~(d_align - 1));
    unsigned al = (&d_buffer[d_idx]) - ar;

    volk_32fc_32f_dot_prod_32fc_a(
        d_output.data(), ar, d_aligned_taps[al].data(), ntaps() + al);
    return d_output[0];
}

template <>
void fir_filter_with_buffer<gr_complex, float>::filterN(gr_complex output[],
                                                        const gr_complex input[],
                                                        size_t n)
{
    for (size_t i = 0; i < n; i++) {
        output[i] = filter(input[i]);
    }
}

template <>
void fir_filter_with_buffer<gr_complex, float>::filterNdec(gr_complex output[],
                                                           const gr_complex input[],
                                                           size_t n,
                                                           size_t decimate)
{
    size_t j = 0;
    for (size_t i = 0; i < n; i++) {
        output[i] = filter(&input[j], decimate);
        j += decimate;
    }
}

template class fir_filter_with_buffer<float, float>;
template class fir_filter_with_buffer<gr_complex, float>;
template class fir_filter_with_buffer<gr_complex, gr_complex>;

} // namespace filter
} // namespace kernel
} // namespace gr

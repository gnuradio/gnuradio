/* -*- c++ -*- */
/*
 * Copyright 2010,2012,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/fft/fft.h>
#include <gnuradio/filter/fir_filter_with_buffer.h>
#include <volk/volk.h>
#include <algorithm>
#include <cstdio>
#include <cstring>

namespace gr {
namespace filter {
namespace kernel {

fir_filter_with_buffer_fff::fir_filter_with_buffer_fff(const std::vector<float>& taps)
    : d_output(1),
      d_align(volk_get_alignment()),
      d_naligned(std::max((size_t)1, d_align / sizeof(float))),
      d_aligned_taps(d_naligned)
{
    set_taps(taps);
}

void fir_filter_with_buffer_fff::set_taps(const std::vector<float>& taps)
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
        std::copy(
            std::begin(d_taps), std::end(d_taps), std::begin(d_aligned_taps[i]) + i);
    }

    d_idx = 0;
}

std::vector<float> fir_filter_with_buffer_fff::taps() const
{
    std::vector<float> t = d_taps;
    std::reverse(t.begin(), t.end());
    return t;
}

float fir_filter_with_buffer_fff::filter(float input)
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

float fir_filter_with_buffer_fff::filter(const float input[], unsigned long dec)
{
    unsigned int i;

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

void fir_filter_with_buffer_fff::filterN(float output[],
                                         const float input[],
                                         unsigned long n)
{
    for (unsigned long i = 0; i < n; i++) {
        output[i] = filter(input[i]);
    }
}

void fir_filter_with_buffer_fff::filterNdec(float output[],
                                            const float input[],
                                            unsigned long n,
                                            unsigned long decimate)
{
    unsigned long j = 0;
    for (unsigned long i = 0; i < n; i++) {
        output[i] = filter(&input[j], decimate);
        j += decimate;
    }
}


/**************************************************************/


fir_filter_with_buffer_ccc::fir_filter_with_buffer_ccc(
    const std::vector<gr_complex>& taps)
    : d_output(1),
      d_align(volk_get_alignment()),
      d_naligned(std::max((size_t)1, d_align / sizeof(gr_complex))),
      d_aligned_taps(d_naligned)
{
    set_taps(taps);
}

void fir_filter_with_buffer_ccc::set_taps(const std::vector<gr_complex>& taps)
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
        std::copy(
            std::begin(d_taps), std::end(d_taps), std::begin(d_aligned_taps[i]) + i);
    }

    d_idx = 0;
}

std::vector<gr_complex> fir_filter_with_buffer_ccc::taps() const
{
    std::vector<gr_complex> t = d_taps;
    std::reverse(t.begin(), t.end());
    return t;
}

gr_complex fir_filter_with_buffer_ccc::filter(gr_complex input)
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

gr_complex fir_filter_with_buffer_ccc::filter(const gr_complex input[], unsigned long dec)
{
    unsigned int i;

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

void fir_filter_with_buffer_ccc::filterN(gr_complex output[],
                                         const gr_complex input[],
                                         unsigned long n)
{
    for (unsigned long i = 0; i < n; i++) {
        output[i] = filter(input[i]);
    }
}

void fir_filter_with_buffer_ccc::filterNdec(gr_complex output[],
                                            const gr_complex input[],
                                            unsigned long n,
                                            unsigned long decimate)
{
    unsigned long j = 0;
    for (unsigned long i = 0; i < n; i++) {
        output[i] = filter(&input[j], decimate);
        j += decimate;
    }
}


/**************************************************************/


fir_filter_with_buffer_ccf::fir_filter_with_buffer_ccf(const std::vector<float>& taps)
    : d_output(1),
      d_align(volk_get_alignment()),
      d_naligned(std::max((size_t)1, d_align / sizeof(gr_complex))),
      d_aligned_taps(d_naligned)
{
    set_taps(taps);
}

void fir_filter_with_buffer_ccf::set_taps(const std::vector<float>& taps)
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
        std::copy(
            std::begin(d_taps), std::end(d_taps), std::begin(d_aligned_taps[i]) + i);
    }

    d_idx = 0;
}

std::vector<float> fir_filter_with_buffer_ccf::taps() const
{
    std::vector<float> t = d_taps;
    std::reverse(t.begin(), t.end());
    return t;
}

gr_complex fir_filter_with_buffer_ccf::filter(gr_complex input)
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

gr_complex fir_filter_with_buffer_ccf::filter(const gr_complex input[], unsigned long dec)
{
    unsigned int i;

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

void fir_filter_with_buffer_ccf::filterN(gr_complex output[],
                                         const gr_complex input[],
                                         unsigned long n)
{
    for (unsigned long i = 0; i < n; i++) {
        output[i] = filter(input[i]);
    }
}

void fir_filter_with_buffer_ccf::filterNdec(gr_complex output[],
                                            const gr_complex input[],
                                            unsigned long n,
                                            unsigned long decimate)
{
    unsigned long j = 0;
    for (unsigned long i = 0; i < n; i++) {
        output[i] = filter(&input[j], decimate);
        j += decimate;
    }
}


} /* namespace kernel */
} /* namespace filter */
} /* namespace gr */

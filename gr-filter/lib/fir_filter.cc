/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018,2019 Free Software Foundation, Inc.
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

#include <gnuradio/fft/fft.h>
#include <gnuradio/filter/fir_filter.h>
#include <volk/volk.h>
#include <cstdio>
#include <cstring>

namespace gr {
namespace filter {
namespace kernel {

template <class IN_T, class OUT_T, class TAP_T>
fir_filter<IN_T, OUT_T, TAP_T>::fir_filter(int decimation, const std::vector<TAP_T>& taps)
{
    d_align = volk_get_alignment();
    d_naligned = std::max((size_t)1, d_align / sizeof(IN_T));

    d_aligned_taps = NULL;
    set_taps(taps);

    // Make sure the output sample is always aligned, too.
    d_output = (OUT_T*)volk_malloc(1 * sizeof(OUT_T), d_align);
}

template <class IN_T, class OUT_T, class TAP_T>
fir_filter<IN_T, OUT_T, TAP_T>::~fir_filter()
{
    // Free all aligned taps
    if (d_aligned_taps != NULL) {
        for (int i = 0; i < d_naligned; i++) {
            volk_free(d_aligned_taps[i]);
        }
        ::free(d_aligned_taps);
        d_aligned_taps = NULL;
    }

    // Free output sample
    volk_free(d_output);
}

template <class IN_T, class OUT_T, class TAP_T>
void fir_filter<IN_T, OUT_T, TAP_T>::set_taps(const std::vector<TAP_T>& taps)
{
    // Free the taps if already allocated
    if (d_aligned_taps != NULL) {
        for (int i = 0; i < d_naligned; i++) {
            volk_free(d_aligned_taps[i]);
        }
        ::free(d_aligned_taps);
        d_aligned_taps = NULL;
    }

    d_ntaps = (int)taps.size();
    d_taps = taps;
    std::reverse(d_taps.begin(), d_taps.end());

    // Make a set of taps at all possible arch alignments
    d_aligned_taps = (TAP_T**)malloc(d_naligned * sizeof(TAP_T*));
    for (int i = 0; i < d_naligned; i++) {
        d_aligned_taps[i] =
            (TAP_T*)volk_malloc((d_ntaps + d_naligned - 1) * sizeof(TAP_T), d_align);
        std::fill_n(d_aligned_taps[i], d_ntaps + d_naligned - 1, 0);
        for (unsigned int j = 0; j < d_ntaps; j++)
            d_aligned_taps[i][i + j] = d_taps[j];
    }
}

template <class IN_T, class OUT_T, class TAP_T>
void fir_filter<IN_T, OUT_T, TAP_T>::update_tap(TAP_T t, unsigned int index)
{
    d_taps[index] = t;
    for (int i = 0; i < d_naligned; i++) {
        d_aligned_taps[i][i + index] = t;
    }
}

template <class IN_T, class OUT_T, class TAP_T>
std::vector<TAP_T> fir_filter<IN_T, OUT_T, TAP_T>::taps() const
{
    std::vector<TAP_T> t = d_taps;
    std::reverse(t.begin(), t.end());
    return t;
}

template <class IN_T, class OUT_T, class TAP_T>
unsigned int fir_filter<IN_T, OUT_T, TAP_T>::ntaps() const
{
    return d_ntaps;
}

template <class IN_T, class OUT_T, class TAP_T>
void fir_filter<IN_T, OUT_T, TAP_T>::filterN(OUT_T output[],
                                             const IN_T input[],
                                             unsigned long n)
{
    for (unsigned long i = 0; i < n; i++) {
        output[i] = filter(&input[i]);
    }
}

template <class IN_T, class OUT_T, class TAP_T>
void fir_filter<IN_T, OUT_T, TAP_T>::filterNdec(OUT_T output[],
                                                const IN_T input[],
                                                unsigned long n,
                                                unsigned int decimate)
{
    unsigned long j = 0;
    for (unsigned long i = 0; i < n; i++) {
        output[i] = filter(&input[j]);
        j += decimate;
    }
}

template <>
float fir_filter<float, float, float>::filter(const float input[])
{
    const float* ar = (float*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_32f_x2_dot_prod_32f_a(d_output, ar, d_aligned_taps[al], d_ntaps + al);
    return *d_output;
}

template <>
gr_complex fir_filter<gr_complex, gr_complex, float>::filter(const gr_complex input[])
{
    const gr_complex* ar = (gr_complex*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_32fc_32f_dot_prod_32fc_a(d_output, ar, d_aligned_taps[al], (d_ntaps + al));
    return *d_output;
}

template <>
gr_complex fir_filter<float, gr_complex, gr_complex>::filter(const float input[])
{
    const float* ar = (float*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_32fc_32f_dot_prod_32fc_a(d_output, d_aligned_taps[al], ar, (d_ntaps + al));
    return *d_output;
}

template <>
gr_complex
fir_filter<gr_complex, gr_complex, gr_complex>::filter(const gr_complex input[])
{
    const gr_complex* ar = (gr_complex*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_32fc_x2_dot_prod_32fc_a(d_output, ar, d_aligned_taps[al], (d_ntaps + al));
    return *d_output;
}

template <>
gr_complex
fir_filter<std::int16_t, gr_complex, gr_complex>::filter(const std::int16_t input[])
{
    const std::int16_t* ar = (std::int16_t*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_16i_32fc_dot_prod_32fc_a(d_output, ar, d_aligned_taps[al], (d_ntaps + al));

    return *d_output;
}

template <>
short fir_filter<float, std::int16_t, float>::filter(const float input[])
{
    const float* ar = (float*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_32f_x2_dot_prod_16i_a(d_output, ar, d_aligned_taps[al], (d_ntaps + al));

    return *d_output;
}
template class fir_filter<float, float, float>;
template class fir_filter<gr_complex, gr_complex, float>;
template class fir_filter<float, gr_complex, gr_complex>;
template class fir_filter<gr_complex, gr_complex, gr_complex>;
template class fir_filter<std::int16_t, gr_complex, gr_complex>;
template class fir_filter<float, std::int16_t, float>;
} /* namespace kernel */
} /* namespace filter */
} /* namespace gr */

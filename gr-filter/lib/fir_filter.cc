/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
fir_filter<IN_T, OUT_T, TAP_T>::fir_filter(const std::vector<TAP_T>& taps) : d_output(1)
{
    d_align = volk_get_alignment();
    d_naligned = std::max((size_t)1, d_align / sizeof(IN_T));
    set_taps(taps);
}

template <class IN_T, class OUT_T, class TAP_T>
void fir_filter<IN_T, OUT_T, TAP_T>::set_taps(const std::vector<TAP_T>& taps)
{
    d_ntaps = (int)taps.size();
    d_taps = taps;
    std::reverse(d_taps.begin(), d_taps.end());

    d_aligned_taps.clear();
    d_aligned_taps = std::vector<volk::vector<TAP_T>>(
        d_naligned, volk::vector<TAP_T>((d_ntaps + d_naligned - 1), 0));
    for (int i = 0; i < d_naligned; i++) {
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
float fir_filter<float, float, float>::filter(const float input[]) const
{
    const float* ar = (float*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_32f_x2_dot_prod_32f_a(
        const_cast<float*>(d_output.data()), ar, d_aligned_taps[al].data(), d_ntaps + al);
    return d_output[0];
}

template <>
gr_complex
fir_filter<gr_complex, gr_complex, float>::filter(const gr_complex input[]) const
{
    const gr_complex* ar = (gr_complex*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_32fc_32f_dot_prod_32fc_a(const_cast<gr_complex*>(d_output.data()),
                                  ar,
                                  d_aligned_taps[al].data(),
                                  (d_ntaps + al));
    return d_output[0];
}

template <>
gr_complex fir_filter<float, gr_complex, gr_complex>::filter(const float input[]) const
{
    const float* ar = (float*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_32fc_32f_dot_prod_32fc_a(const_cast<gr_complex*>(d_output.data()),
                                  d_aligned_taps[al].data(),
                                  ar,
                                  (d_ntaps + al));
    return d_output[0];
}

template <>
gr_complex
fir_filter<gr_complex, gr_complex, gr_complex>::filter(const gr_complex input[]) const
{
    const gr_complex* ar = (gr_complex*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_32fc_x2_dot_prod_32fc_a(const_cast<gr_complex*>(d_output.data()),
                                 ar,
                                 d_aligned_taps[al].data(),
                                 (d_ntaps + al));
    return d_output[0];
}

template <>
gr_complex
fir_filter<std::int16_t, gr_complex, gr_complex>::filter(const std::int16_t input[]) const
{
    const std::int16_t* ar = (std::int16_t*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_16i_32fc_dot_prod_32fc_a(const_cast<gr_complex*>(d_output.data()),
                                  ar,
                                  d_aligned_taps[al].data(),
                                  (d_ntaps + al));

    return d_output[0];
}

template <>
short fir_filter<float, std::int16_t, float>::filter(const float input[]) const
{
    const float* ar = (float*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_32f_x2_dot_prod_16i_a(const_cast<std::int16_t*>(d_output.data()),
                               ar,
                               d_aligned_taps[al].data(),
                               (d_ntaps + al));

    return d_output[0];
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

/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2008,2010,2012,2020 Free Software Foundation, Inc.
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "fft_cpu.h"
#include "fft_cpu_gen.h"

namespace gr {
namespace fft {

template <class T, bool forward>
fft_cpu<T, forward>::fft_cpu(const typename fft<T, forward>::block_args& args)
    : INHERITED_CONSTRUCTORS(T, forward),
      d_fft_size(args.fft_size),
      d_shift(args.shift),
      d_fft(args.fft_size)
{
    if (args.window.empty() || args.window.size() == d_fft_size) {
        d_window = args.window;
    }
    else {
        throw std::runtime_error("fft: window not the same length as fft_size");
    }
}

template <class T, bool forward>
void fft_cpu<T, forward>::set_nthreads(int n)
{
    d_fft.set_nthreads(n);
}

template <class T, bool forward>
int fft_cpu<T, forward>::nthreads() const
{
    return d_fft.nthreads();
}

template <>
void fft_cpu<gr_complex, true>::fft_and_shift(const gr_complex* in, gr_complex* out)
{
    if (!d_window.empty()) {
        gr_complex* dst = d_fft.get_inbuf();
        volk_32fc_32f_multiply_32fc(&dst[0], in, &d_window[0], d_fft_size);
    }
    else {
        memcpy(d_fft.get_inbuf(), in, sizeof(gr_complex) * d_fft_size);
    }
    d_fft.execute();
    if (d_shift) {
        unsigned int len = (unsigned int)(ceil(d_fft_size / 2.0));
        memcpy(
            &out[0], &d_fft.get_outbuf()[len], sizeof(gr_complex) * (d_fft_size - len));
        memcpy(&out[d_fft_size - len], &d_fft.get_outbuf()[0], sizeof(gr_complex) * len);
    }
    else {

        memcpy(out, d_fft.get_outbuf(), sizeof(gr_complex) * d_fft_size);
    }
}

template <>
void fft_cpu<gr_complex, false>::fft_and_shift(const gr_complex* in, gr_complex* out)
{
    if (!d_window.empty()) {
        gr_complex* dst = d_fft.get_inbuf();
        if (d_shift) {
            unsigned int offset = d_fft_size / 2;
            int fft_m_offset = d_fft_size - offset;
            volk_32fc_32f_multiply_32fc(&dst[fft_m_offset], &in[0], &d_window[0], offset);
            volk_32fc_32f_multiply_32fc(
                &dst[0], &in[offset], &d_window[offset], d_fft_size - offset);
        }
        else {
            volk_32fc_32f_multiply_32fc(&dst[0], in, &d_window[0], d_fft_size);
        }
    }
    else {
        if (d_shift) { // apply an ifft shift on the data
            gr_complex* dst = d_fft.get_inbuf();
            unsigned int len =
                (unsigned int)(floor(d_fft_size / 2.0)); // half length of complex array
            memcpy(&dst[0], &in[len], sizeof(gr_complex) * (d_fft_size - len));
            memcpy(&dst[d_fft_size - len], &in[0], sizeof(gr_complex) * len);
        }
        else {
            memcpy(d_fft.get_inbuf(), in, sizeof(gr_complex) * d_fft_size);
        }
    }
    d_fft.execute();
    memcpy(out, d_fft.get_outbuf(), sizeof(gr_complex) * d_fft_size);
}

template <>
void fft_cpu<float, true>::fft_and_shift(const float* in, gr_complex* out)
{
    // copy input into optimally aligned buffer
    if (!d_window.empty()) {
        gr_complex* dst = d_fft.get_inbuf();
        for (unsigned int i = 0; i < d_fft_size; i++) // apply window
            dst[i] = in[i] * d_window[i];
    }
    else {
        gr_complex* dst = d_fft.get_inbuf();
        for (unsigned int i = 0; i < d_fft_size; i++) // float to complex conversion
            dst[i] = in[i];
    }

    d_fft.execute();
    if (d_shift) {
        unsigned int len = (unsigned int)(ceil(d_fft_size / 2.0));
        memcpy(
            &out[0], &d_fft.get_outbuf()[len], sizeof(gr_complex) * (d_fft_size - len));
        memcpy(&out[d_fft_size - len], &d_fft.get_outbuf()[0], sizeof(gr_complex) * len);
    }
    else {

        memcpy(out, d_fft.get_outbuf(), sizeof(gr_complex) * d_fft_size);
    }
}

template <>
void fft_cpu<float, false>::fft_and_shift(const float* in, gr_complex* out)
{
    // copy input into optimally aligned buffer
    if (!d_window.empty()) {
        gr_complex* dst = d_fft.get_inbuf();
        if (d_shift) {
            unsigned int len =
                (unsigned int)(floor(d_fft_size / 2.0)); // half length of complex array
            for (unsigned int i = 0; i < len; i++) {
                dst[i] = in[len + i] * d_window[len + i];
            }
            for (unsigned int i = len; i < d_fft_size; i++) {
                dst[i] = in[i - len] * d_window[i - len];
            }
        }
        else {
            for (unsigned int i = 0; i < d_fft_size; i++) // apply window
                dst[i] = in[i] * d_window[i];
        }
    }
    else {
        gr_complex* dst = d_fft.get_inbuf();
        if (d_shift) {
            unsigned int len =
                (unsigned int)(floor(d_fft_size / 2.0)); // half length of complex array
            for (unsigned int i = 0; i < len; i++) {
                dst[i] = in[len + i];
            }
            for (unsigned int i = len; i < d_fft_size; i++) {
                dst[i] = in[i - len];
            }
        }
        else {
            for (unsigned int i = 0; i < d_fft_size; i++) // float to complex conversion
                dst[i] = in[i];
        }
    }

    // compute the fft
    d_fft.execute();

    // copy result to output stream
    memcpy(out, d_fft.get_outbuf(), sizeof(gr_complex) * d_fft_size);
}

template <class T, bool forward>
work_return_code_t fft_cpu<T, forward>::work(work_io& wio)
{
    auto in = wio.inputs()[0].items<T>();
    auto out = wio.outputs()[0].items<gr_complex>();
    auto noutput_items = wio.outputs()[0].n_items;

    size_t count = 0;

    while (count++ < noutput_items) {

        fft_and_shift(in, out);

        in += d_fft_size;
        out += d_fft_size;
    }


    wio.produce_each(noutput_items);
    return work_return_code_t::WORK_OK;
}


} // namespace fft
} // namespace gr
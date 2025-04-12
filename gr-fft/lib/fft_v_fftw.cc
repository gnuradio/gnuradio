/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2008,2010,2012,2020 Free Software Foundation, Inc.
 * Copyright 2024 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "fft_v_fftw.h"
#include <gnuradio/thread/thread.h>
#include <volk/volk.h>
#include <cstring>

namespace gr {
namespace fft {

template <class T, bool forward>
typename fft_v<T, forward>::sptr fft_v<T, forward>::make(int fft_size,
                                                         const std::vector<float>& window,
                                                         bool shift,
                                                         int nthreads)
{
    return gnuradio::make_block_sptr<fft_v_fftw<T, forward>>(
        fft_size, window, shift, nthreads);
}

template <class T, bool forward>
fft_v_fftw<T, forward>::fft_v_fftw(int fft_size,
                                   const std::vector<float>& window,
                                   bool shift,
                                   int nthreads)
    : sync_block("fft_v_fftw",
                 io_signature::make(1, 1, fft_size * sizeof(T)),
                 io_signature::make(1, 1, fft_size * sizeof(gr_complex))),
      d_fft_size(fft_size),
      d_fft(fft_size, nthreads),
      d_shift(shift)
{
    if (!set_window(window))
        throw std::runtime_error("fft_v: window not the same length as fft_size");
}

template <class T, bool forward>
void fft_v_fftw<T, forward>::set_nthreads(int n)
{
    gr::thread::scoped_lock window_lock(this->d_setlock);
    d_fft.set_nthreads(n);
}

template <class T, bool forward>
int fft_v_fftw<T, forward>::nthreads() const
{
    return d_fft.nthreads();
}

template <class T, bool forward>
bool fft_v_fftw<T, forward>::set_window(const std::vector<float>& window)
{
    gr::thread::scoped_lock window_lock(this->d_setlock);
    if (window.empty() || window.size() == d_fft_size) {
        d_window = window;
        return true;
    } else
        return false;
}

template <>
void fft_v_fftw<gr_complex, true>::fft_and_shift(const gr_complex* in, gr_complex* out)
{
    if (!d_window.empty()) {
        gr_complex* dst = d_fft.get_inbuf();
        volk_32fc_32f_multiply_32fc(&dst[0], in, &d_window[0], d_fft_size);
    } else {
        memcpy(d_fft.get_inbuf(), in, sizeof(gr_complex) * d_fft_size);
    }
    d_fft.execute();
    if (d_shift) {
        // round up
        unsigned int len = (d_fft_size + 1) / 2;
        memcpy(
            &out[0], &d_fft.get_outbuf()[len], sizeof(gr_complex) * (d_fft_size - len));
        memcpy(&out[d_fft_size - len], &d_fft.get_outbuf()[0], sizeof(gr_complex) * len);
    } else {
        memcpy(out, d_fft.get_outbuf(), sizeof(gr_complex) * d_fft_size);
    }
}

template <>
void fft_v_fftw<gr_complex, false>::fft_and_shift(const gr_complex* in, gr_complex* out)
{
    if (!d_window.empty()) {
        gr_complex* dst = d_fft.get_inbuf();
        if (d_shift) {
            unsigned int offset = d_fft_size / 2;
            int fft_m_offset = d_fft_size - offset;
            volk_32fc_32f_multiply_32fc(&dst[fft_m_offset], &in[0], &d_window[0], offset);
            volk_32fc_32f_multiply_32fc(
                &dst[0], &in[offset], &d_window[offset], d_fft_size - offset);
        } else {
            volk_32fc_32f_multiply_32fc(&dst[0], in, &d_window[0], d_fft_size);
        }
    } else {
        if (d_shift) { // apply an ifft shift on the data
            gr_complex* dst = d_fft.get_inbuf();
            // round down
            unsigned int len = d_fft_size / 2; // half length of complex array
            memcpy(&dst[0], &in[len], sizeof(gr_complex) * (d_fft_size - len));
            memcpy(&dst[d_fft_size - len], &in[0], sizeof(gr_complex) * len);
        } else {
            memcpy(d_fft.get_inbuf(), in, sizeof(gr_complex) * d_fft_size);
        }
    }

    d_fft.execute();
    memcpy(out, d_fft.get_outbuf(), sizeof(gr_complex) * d_fft_size);
}

template <>
void fft_v_fftw<float, true>::fft_and_shift(const float* in, gr_complex* out)
{
    // copy input into optimally aligned buffer
    if (!d_window.empty()) {
        gr_complex* dst = d_fft.get_inbuf();
        for (unsigned int i = 0; i < d_fft_size; i++) // apply window
            dst[i] = in[i] * d_window[i];
    } else {
        gr_complex* dst = d_fft.get_inbuf();
        for (unsigned int i = 0; i < d_fft_size; i++) // float to complex conversion
            dst[i] = in[i];
    }

    d_fft.execute();
    if (d_shift) {
        // round up
        unsigned int len = (d_fft_size + 1) / 2;
        memcpy(
            &out[0], &d_fft.get_outbuf()[len], sizeof(gr_complex) * (d_fft_size - len));
        memcpy(&out[d_fft_size - len], &d_fft.get_outbuf()[0], sizeof(gr_complex) * len);
    } else {

        memcpy(out, d_fft.get_outbuf(), sizeof(gr_complex) * d_fft_size);
    }
}

template <>
void fft_v_fftw<float, false>::fft_and_shift(const float* in, gr_complex* out)
{
    // copy input into optimally aligned buffer
    if (!d_window.empty()) {
        gr_complex* dst = d_fft.get_inbuf();
        if (d_shift) {
            // round down
            unsigned int len = d_fft_size / 2; // half length of complex array
            for (unsigned int i = 0; i < len; i++) {
                dst[i] = in[len + i] * d_window[len + i];
            }
            for (unsigned int i = len; i < d_fft_size; i++) {
                dst[i] = in[i - len] * d_window[i - len];
            }
        } else {
            for (unsigned int i = 0; i < d_fft_size; i++) // apply window
                dst[i] = in[i] * d_window[i];
        }
    } else {
        gr_complex* dst = d_fft.get_inbuf();
        if (d_shift) {
            // round down
            unsigned int len = d_fft_size / 2; // half length of complex array
            for (unsigned int i = 0; i < len; i++) {
                dst[i] = in[len + i];
            }
            for (unsigned int i = len; i < d_fft_size; i++) {
                dst[i] = in[i - len];
            }
        } else {
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
int fft_v_fftw<T, forward>::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    auto in = reinterpret_cast<const T*>(input_items[0]);
    auto out = reinterpret_cast<gr_complex*>(output_items[0]);

    int count = 0;

    gr::thread::scoped_lock window_lock(this->d_setlock);
    while (count++ < noutput_items) {

        fft_and_shift(in, out);

        in += d_fft_size;
        out += d_fft_size;
    }

    return noutput_items;
}

template class fft_v<gr_complex, true>;
template class fft_v<gr_complex, false>;
template class fft_v<float, true>;
template class fft_v<float, false>;
} /* namespace fft */
} /* namespace gr */

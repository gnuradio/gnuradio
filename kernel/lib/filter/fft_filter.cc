/* -*- c++ -*- */
/*
 * Copyright 2010,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/kernel/filter/fft_filter.h>
#include <gnuradio/logger.h>
#include <volk/volk.h>
#include <cstring>
#include <memory>

namespace gr {
namespace kernel {
namespace filter {


#define VERBOSE 0


/**************************************************************/

// determine and set d_ntaps, d_nsamples, d_fftsize
template <>
void fft_filter<gr_complex, gr_complex>::compute_sizes(int ntaps)
{
    int old_fftsize = d_fftsize;
    d_ntaps = ntaps;
    d_fftsize = (int)(2 * pow(2.0, ceil(log(double(ntaps)) / log(2.0))));
    d_nsamples = d_fftsize - d_ntaps + 1;

    if (VERBOSE) {
        d_logger->alert("fft_filter_ccc: ntaps = {:d} fftsize = {:d} nsamples = {:d}",
                        d_ntaps,
                        d_fftsize,
                        d_nsamples);
    }

    // compute new plans
    if (d_fftsize != old_fftsize) {
        d_fwdfft = std::make_unique<fft::fft_complex_fwd>(d_fftsize, d_nthreads);
        d_invfft = std::make_unique<fft::fft_complex_rev>(d_fftsize, d_nthreads);
        d_xformed_taps.resize(d_fftsize);
    }
}


/*
 * determines d_ntaps, d_nsamples, d_fftsize, d_xformed_taps
 */
template <>
int fft_filter<gr_complex, gr_complex>::set_taps(const std::vector<gr_complex>& taps)
{
    int i = 0;
    d_taps = taps;
    compute_sizes(taps.size());

    d_tail.resize(tailsize());
    for (i = 0; i < tailsize(); i++)
        d_tail[i] = 0;

    gr_complex* in = d_fwdfft->get_inbuf();
    gr_complex* out = d_fwdfft->get_outbuf();

    float scale = 1.0 / d_fftsize;

    // Compute forward xform of taps.
    // Copy taps into first ntaps slots, then pad with zeros
    for (i = 0; i < d_ntaps; i++)
        in[i] = taps[i] * scale;

    for (; i < d_fftsize; i++)
        in[i] = 0;

    d_fwdfft->execute(); // do the xform

    // now copy output to d_xformed_taps
    for (i = 0; i < d_fftsize; i++)
        d_xformed_taps[i] = out[i];

    return d_nsamples;
}


template <>
fft_filter<gr_complex, gr_complex>::fft_filter(int decimation,
                                               const std::vector<gr_complex>& taps,
                                               int nthreads)
    : d_fftsize(-1), d_decimation(decimation), d_nthreads(nthreads)
{
    gr::configure_default_loggers(d_logger, d_debug_logger, "fft_filter_ccc");
    set_taps(taps);
}


template <>
int fft_filter<gr_complex, gr_complex>::filter(int nitems,
                                               const gr_complex* input,
                                               gr_complex* output)
{
    int dec_ctr = 0;
    int j = 0;
    int ninput_items = nitems * d_decimation;

    for (int i = 0; i < ninput_items; i += d_nsamples) {
        memcpy(d_fwdfft->get_inbuf(), &input[i], d_nsamples * sizeof(gr_complex));

        for (j = d_nsamples; j < d_fftsize; j++)
            d_fwdfft->get_inbuf()[j] = 0;

        d_fwdfft->execute(); // compute fwd xform

        gr_complex* a = d_fwdfft->get_outbuf();
        gr_complex* b = d_xformed_taps.data();
        gr_complex* c = d_invfft->get_inbuf();

        volk_32fc_x2_multiply_32fc_a(c, a, b, d_fftsize);

        d_invfft->execute(); // compute inv xform

        // add in the overlapping tail

        for (j = 0; j < tailsize(); j++)
            d_invfft->get_outbuf()[j] += d_tail[j];

        // copy nsamples to output
        j = dec_ctr;
        while (j < d_nsamples) {
            *output++ = d_invfft->get_outbuf()[j];
            j += d_decimation;
        }
        dec_ctr = (j - d_nsamples);

        // stash the tail
        if (!d_tail.empty()) {
            memcpy(&d_tail[0],
                   d_invfft->get_outbuf() + d_nsamples,
                   tailsize() * sizeof(gr_complex));
        }
    }

    return nitems;
}


/**************************************************************/

// determine and set d_ntaps, d_nsamples, d_fftsize
template <>
void fft_filter<gr_complex, float>::compute_sizes(int ntaps)
{
    int old_fftsize = d_fftsize;
    d_ntaps = ntaps;
    d_fftsize = (int)(2 * pow(2.0, ceil(log(double(ntaps)) / log(2.0))));
    d_nsamples = d_fftsize - d_ntaps + 1;

    if (VERBOSE) {
        d_logger->alert("fft_filter_ccf: ntaps = {:d} fftsize = {:d} nsamples = {:d}",
                        d_ntaps,
                        d_fftsize,
                        d_nsamples);
    }

    // compute new plans
    if (d_fftsize != old_fftsize) {
        d_fwdfft = std::make_unique<fft::fft_complex_fwd>(d_fftsize, d_nthreads);
        d_invfft = std::make_unique<fft::fft_complex_rev>(d_fftsize, d_nthreads);
        d_xformed_taps.resize(d_fftsize);
    }
}


/*
 * determines d_ntaps, d_nsamples, d_fftsize, d_xformed_taps
 */
template <>
int fft_filter<gr_complex, float>::set_taps(const std::vector<float>& taps)
{
    int i = 0;
    d_taps = taps;
    compute_sizes(taps.size());

    d_tail.resize(tailsize());
    for (i = 0; i < tailsize(); i++)
        d_tail[i] = 0;

    gr_complex* in = d_fwdfft->get_inbuf();
    gr_complex* out = d_fwdfft->get_outbuf();

    float scale = 1.0 / d_fftsize;

    // Compute forward xform of taps.
    // Copy taps into first ntaps slots, then pad with zeros
    for (i = 0; i < d_ntaps; i++)
        in[i] = gr_complex(taps[i] * scale, 0.0f);

    for (; i < d_fftsize; i++)
        in[i] = gr_complex(0.0f, 0.0f);

    d_fwdfft->execute(); // do the xform

    // now copy output to d_xformed_taps
    for (i = 0; i < d_fftsize; i++)
        d_xformed_taps[i] = out[i];

    return d_nsamples;
}


template <>
fft_filter<gr_complex, float>::fft_filter(int decimation,
                                          const std::vector<float>& taps,
                                          int nthreads)
    : d_fftsize(-1), d_decimation(decimation), d_nthreads(nthreads)
{
    gr::configure_default_loggers(d_logger, d_debug_logger, "fft_filter_ccf");
    set_taps(taps);
}


template <>
int fft_filter<gr_complex, float>::filter(int nitems,
                                          const gr_complex* input,
                                          gr_complex* output)
{
    int dec_ctr = 0;
    int j = 0;
    int ninput_items = nitems * d_decimation;

    for (int i = 0; i < ninput_items; i += d_nsamples) {
        memcpy(d_fwdfft->get_inbuf(), &input[i], d_nsamples * sizeof(gr_complex));

        for (j = d_nsamples; j < d_fftsize; j++)
            d_fwdfft->get_inbuf()[j] = 0;

        d_fwdfft->execute(); // compute fwd xform

        gr_complex* a = d_fwdfft->get_outbuf();
        gr_complex* b = d_xformed_taps.data();
        gr_complex* c = d_invfft->get_inbuf();

        volk_32fc_x2_multiply_32fc_a(c, a, b, d_fftsize);

        d_invfft->execute(); // compute inv xform

        // add in the overlapping tail

        for (j = 0; j < tailsize(); j++)
            d_invfft->get_outbuf()[j] += d_tail[j];

        // copy nsamples to output
        j = dec_ctr;
        while (j < d_nsamples) {
            *output++ = d_invfft->get_outbuf()[j];
            j += d_decimation;
        }
        dec_ctr = (j - d_nsamples);

        // stash the tail
        if (!d_tail.empty()) {
            memcpy(&d_tail[0],
                   d_invfft->get_outbuf() + d_nsamples,
                   tailsize() * sizeof(gr_complex));
        }
    }

    return nitems;
}

template <class T, class TAPS_T>
fft_filter<T, TAPS_T>::fft_filter(int decimation,
                                  const std::vector<TAPS_T>& taps,
                                  int nthreads)
    : d_fftsize(-1), d_decimation(decimation), d_nthreads(nthreads)
{
    gr::configure_default_loggers(d_logger, d_debug_logger, "fft_filter_fff");
    set_taps(taps);
}

// determine and set d_ntaps, d_nsamples, d_fftsize
template <class T, class TAPS_T>
void fft_filter<T, TAPS_T>::compute_sizes(int ntaps)
{
    int old_fftsize = d_fftsize;
    d_ntaps = ntaps;
    d_fftsize = (int)(2 * pow(2.0, ceil(log(double(ntaps)) / log(2.0))));
    d_nsamples = d_fftsize - d_ntaps + 1;

    if (VERBOSE) {
        d_logger->alert("fft_filter_fff: ntaps = {:d} fftsize = {:d} nsamples = {:d}",
                        d_ntaps,
                        d_fftsize,
                        d_nsamples);
    }

    // compute new plans
    if (d_fftsize != old_fftsize) {
        d_fwdfft = std::make_unique<fft::fftw_fft<T, true>>(d_fftsize);
        d_invfft = std::make_unique<fft::fftw_fft<T, false>>(d_fftsize);
        d_xformed_taps.resize(d_fftsize / 2 + 1);
    }
}

/*
 * determines d_ntaps, d_nsamples, d_fftsize, d_xformed_taps
 */
template <>
int fft_filter<float, float>::set_taps(const std::vector<float>& taps)
{
    int i = 0;
    d_taps = taps;
    compute_sizes(taps.size());

    d_tail.resize(tailsize());
    for (i = 0; i < tailsize(); i++)
        d_tail[i] = 0;

    float* in = d_fwdfft->get_inbuf();
    gr_complex* out = d_fwdfft->get_outbuf();

    float scale = 1.0 / d_fftsize;

    // Compute forward xform of taps.
    // Copy taps into first ntaps slots, then pad with zeros
    for (i = 0; i < d_ntaps; i++)
        in[i] = taps[i] * scale;

    for (; i < d_fftsize; i++)
        in[i] = 0;

    d_fwdfft->execute(); // do the xform

    // now copy output to d_xformed_taps
    for (i = 0; i < d_fftsize / 2 + 1; i++)
        d_xformed_taps[i] = out[i];

    return d_nsamples;
}

template <class T, class TAPS_T>
void fft_filter<T, TAPS_T>::set_nthreads(int n)
{
    d_nthreads = n;
    if (d_fwdfft)
        d_fwdfft->set_nthreads(n);
    if (d_invfft)
        d_invfft->set_nthreads(n);
}

template <class T, class TAPS_T>
std::vector<TAPS_T> fft_filter<T, TAPS_T>::taps() const
{
    return d_taps;
}

template <class T, class TAPS_T>
unsigned int fft_filter<T, TAPS_T>::ntaps() const
{
    return d_ntaps;
}

template <class T, class TAPS_T>
int fft_filter<T, TAPS_T>::nthreads() const
{
    return d_nthreads;
}

template <class T, class TAPS_T>
int fft_filter<T, TAPS_T>::filter(int nitems, const T* input, T* output)
{
    int dec_ctr = 0;
    int j = 0;
    int ninput_items = nitems * d_decimation;

    for (int i = 0; i < ninput_items; i += d_nsamples) {

        memcpy(d_fwdfft->get_inbuf(), &input[i], d_nsamples * sizeof(T));

        for (j = d_nsamples; j < d_fftsize; j++)
            d_fwdfft->get_inbuf()[j] = 0;

        d_fwdfft->execute(); // compute fwd xform

        gr_complex* a = d_fwdfft->get_outbuf();
        gr_complex* c = d_invfft->get_inbuf();

        volk_32fc_x2_multiply_32fc_a(c, a, d_xformed_taps.data(), d_xformed_taps.size());

        d_invfft->execute(); // compute inv xform

        // add in the overlapping tail
        for (j = 0; j < tailsize(); j++)
            d_invfft->get_outbuf()[j] += d_tail[j];

        // copy nsamples to output
        j = dec_ctr;
        while (j < d_nsamples) {
            *output++ = d_invfft->get_outbuf()[j];
            j += d_decimation;
        }
        dec_ctr = (j - d_nsamples);

        // stash the tail
        if (!d_tail.empty()) {
            memcpy(
                &d_tail[0], d_invfft->get_outbuf() + d_nsamples, tailsize() * sizeof(T));
        }
    }

    return nitems;
}


template class fft_filter<float, float>;
template class fft_filter<gr_complex, gr_complex>;
template class fft_filter<gr_complex, float>;

} // namespace filter
} // namespace kernel
} /* namespace gr */

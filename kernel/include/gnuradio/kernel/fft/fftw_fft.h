/* -*- c++ -*- */
/*
 * Copyright 2003,2008,2012,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

/*
 * Wrappers for FFTW single precision 1d dft
 */

#include <gnuradio/gr_complex.h>
#include <gnuradio/kernel/api.h>
#include <gnuradio/logger.h>
#include <volk/volk_alloc.hh>

#include <mutex>

namespace gr {
namespace kernel {
namespace fft {

/*! \brief Helper function for allocating complex* buffers
 */
gr_complex* malloc_complex(int size);

/*! \brief Helper function for allocating float* buffers
 */
float* malloc_float(int size);

/*! \brief Helper function for allocating double* buffers
 */
double* malloc_double(int size);

/*! \brief Helper function for freeing fft buffers
 */
void free(void* b);


/*!
 * \brief Export reference to planner mutex for those apps that
 * want to use FFTW w/o using the fft_impl_fftw* classes.
 */
class planner
{
public:
    /*!
     * Return reference to planner mutex
     */
    static std::mutex& mutex();
};


/*!
  \brief FFT: templated
  \ingroup misc
 */


template <class T, bool forward>
struct fft_inbuf {
    using type = T;
};

template <>
struct fft_inbuf<float, false> {
    using type = gr_complex;
};


template <class T, bool forward>
struct fft_outbuf {
    using type = T;
};

template <>
struct fft_outbuf<float, true> {
    using type = gr_complex;
};

template <class T, bool forward>
class fftw_fft
{
    int d_nthreads;
    volk::vector<typename fft_inbuf<T, forward>::type> d_inbuf;
    volk::vector<typename fft_outbuf<T, forward>::type> d_outbuf;
    void* d_plan;
    gr::logger_ptr d_logger;
    gr::logger_ptr d_debug_logger;
    void initialize_plan(int fft_size);

public:
    fftw_fft(int fft_size, int nthreads = 1);
    // Copy disabled due to d_plan.
    fftw_fft(const fftw_fft&) = delete;
    fftw_fft& operator=(const fftw_fft&) = delete;
    virtual ~fftw_fft();

    /*
     * These return pointers to buffers owned by fft_impl_fft_complex
     * into which input and output take place. It's done this way in
     * order to ensure optimal alignment for SIMD instructions.
     */
    typename fft_inbuf<T, forward>::type* get_inbuf() { return d_inbuf.data(); }
    typename fft_outbuf<T, forward>::type* get_outbuf() { return d_outbuf.data(); }

    int inbuf_length() const { return d_inbuf.size(); }
    int outbuf_length() const { return d_outbuf.size(); }

    /*!
     *  Set the number of threads to use for calculation.
     */
    void set_nthreads(int n);

    /*!
     *  Get the number of threads being used by FFTW
     */
    int nthreads() const { return d_nthreads; }

    /*!
     * compute FFT. The input comes from inbuf, the output is placed in
     * outbuf.
     */
    void execute();
};

using fft_complex_fwd = fftw_fft<gr_complex, true>;
using fft_complex_rev = fftw_fft<gr_complex, false>;
using fft_real_fwd = fftw_fft<float, true>;
using fft_real_rev = fftw_fft<float, false>;

} // namespace fft
} // namespace kernel
} // namespace gr
/* -*- c++ -*- */
/*
 * Copyright 2003,2008,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef _FFT_FFT_H_
#define _FFT_FFT_H_

/*
 * Wrappers for FFTW single precision 1d dft
 */

#include <gnuradio/fft/api.h>
#include <gnuradio/gr_complex.h>
#include <volk/volk_alloc.hh>
#include <boost/thread.hpp>

namespace gr {
namespace fft {


/*! \brief Helper function for allocating complex* buffers
 * TODO: Remove once the single user of this stops using it.
 */
FFT_API gr_complex* malloc_complex(int size);

/*! \brief Helper function for freeing fft buffers
 * TODO: Remove once the single user of this stops using it.
 */
FFT_API void free(void* b);

/*!
 * \brief Export reference to planner mutex for those apps that
 * want to use FFTW w/o using the fft_impl_fftw* classes.
 */
class FFT_API planner
{
public:
    typedef boost::mutex::scoped_lock scoped_lock;
    /*!
     * Return reference to planner mutex
     */
    static boost::mutex& mutex();
};

/*!
 * \brief FFT: complex in, complex out
 * \ingroup misc
 */
class FFT_API fft_complex
{
    int d_nthreads;
    volk::vector<gr_complex> d_inbuf;
    volk::vector<gr_complex> d_outbuf;
    void* d_plan;

public:
    fft_complex(int fft_size, bool forward = true, int nthreads = 1);
    // Copy disabled due to d_plan.
    fft_complex(const fft_complex&) = delete;
    fft_complex& operator=(const fft_complex&) = delete;
    virtual ~fft_complex();

    /*
     * These return pointers to buffers owned by fft_impl_fft_complex
     * into which input and output take place. It's done this way in
     * order to ensure optimal alignment for SIMD instructions.
     */
    gr_complex* get_inbuf() { return d_inbuf.data(); }
    gr_complex* get_outbuf() { return d_outbuf.data(); }

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

/*!
 * \brief FFT: real in, complex out
 * \ingroup misc
 */
class FFT_API fft_real_fwd
{
    int d_nthreads;
    volk::vector<float> d_inbuf;
    volk::vector<gr_complex> d_outbuf;
    void* d_plan;

public:
    fft_real_fwd(int fft_size, int nthreads = 1);
    // Copy disabled due to d_plan.
    fft_real_fwd(const fft_real_fwd&) = delete;
    fft_real_fwd& operator=(const fft_real_fwd&) = delete;
    virtual ~fft_real_fwd();

    /*
     * These return pointers to buffers owned by fft_impl_fft_real_fwd
     * into which input and output take place. It's done this way in
     * order to ensure optimal alignment for SIMD instructions.
     */
    float* get_inbuf() { return d_inbuf.data(); }
    gr_complex* get_outbuf() { return d_outbuf.data(); }

    int inbuf_length() const { return d_inbuf.size(); }
    int outbuf_length() const { return d_outbuf.size(); }

    /*!
     *  Set the number of threads to use for caclulation.
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

/*!
 * \brief FFT: complex in, float out
 * \ingroup misc
 */
class FFT_API fft_real_rev
{
    int d_nthreads;
    volk::vector<gr_complex> d_inbuf;
    volk::vector<float> d_outbuf;
    void* d_plan;

public:
    fft_real_rev(int fft_size, int nthreads = 1);
    // Copy disabled due to d_plan.
    fft_real_rev(const fft_real_rev&) = delete;
    fft_real_rev& operator=(const fft_real_rev&) = delete;
    virtual ~fft_real_rev();

    /*
     * These return pointers to buffers owned by fft_impl_fft_real_rev
     * into which input and output take place. It's done this way in
     * order to ensure optimal alignment for SIMD instructions.
     */
    gr_complex* get_inbuf() { return d_inbuf.data(); }
    float* get_outbuf() { return d_outbuf.data(); }

    int inbuf_length() const { return d_inbuf.size(); }
    int outbuf_length() const { return d_outbuf.size(); }

    /*!
     *  Set the number of threads to use for caclulation.
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

} /* namespace fft */
} /*namespace gr */

#endif /* _FFT_FFT_H_ */

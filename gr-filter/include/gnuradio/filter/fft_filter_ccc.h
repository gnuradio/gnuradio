/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FILTER_FFT_FILTER_CCC_H
#define INCLUDED_FILTER_FFT_FILTER_CCC_H

#include <gnuradio/filter/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
namespace filter {

/*!
 * \brief Fast FFT filter with gr_complex input, gr_complex output and gr_complex taps
 * \ingroup filter_blk
 *
 * \details
 * This block implements a complex decimating filter using the
 * fast convolution method via an FFT. The decimation factor is an
 * integer that is greater than or equal to 1.
 *
 * The filter takes a set of complex (or real) taps to use in the
 * filtering operation. These taps can be defined as anything that
 * satisfies the user's filtering needs. For standard filters such
 * as lowpass, highpass, bandpass, etc., the filter.firdes and
 * filter.optfir classes provide convenient generating methods.
 *
 * This filter is implemented by using the FFTW package to perform
 * the required FFTs. An optional argument, nthreads, may be
 * passed to the constructor (or set using the set_nthreads member
 * function) to split the FFT among N number of threads. This can
 * improve performance on very large FFTs (that is, if the number
 * of taps used is very large) if you have enough threads/cores to
 * support it.
 */
class FILTER_API fft_filter_ccc : virtual public sync_decimator
{
public:
    // gr::filter::fft_filter_ccc::sptr
    typedef std::shared_ptr<fft_filter_ccc> sptr;

    /*!
     * Build an FFT filter blocks.
     *
     * \param decimation  >= 1
     * \param taps        complex filter taps
     * \param nthreads    number of threads for the FFT to use
     */
    static sptr
    make(int decimation, const std::vector<gr_complex>& taps, int nthreads = 1);

    virtual void set_taps(const std::vector<gr_complex>& taps) = 0;
    virtual std::vector<gr_complex> taps() const = 0;

    /*!
     * \brief Set number of threads to use.
     */
    virtual void set_nthreads(int n) = 0;

    /*!
     * \brief Get number of threads being used.
     */
    virtual int nthreads() const = 0;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_FFT_FILTER_CCC_H */

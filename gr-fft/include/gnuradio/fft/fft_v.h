/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2008,2012,2018,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FFT_FFT_V_H
#define INCLUDED_FFT_FFT_V_H

#include <gnuradio/fft/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace fft {

/*!
 * \brief Compute forward or reverse FFT. complex vector in / complex vector out.
 * \ingroup fourier_analysis_blk
 *
 * The FFT operation is defined for a vector \f$x\f$ with \f$N\f$ uniformly
 * sampled points by
 *
 * \f[ X(a) = \sum_{k=0}^{N-1} x(a) \cdot e^{-j 2\pi k a / N} \f]
 *
 * \f$ X = FFT\{x\} \f$ is the the FFT transform of \f$x(a)\f$, \f$j\f$ is
 * the imaginary unit, \f$k\f$ and \f$a\f$ range from \f$0\f$ to \f$N-1\f$.
 *
 * The IFFT operation is defined for a vector \f$y\f$ with \f$N\f$
 * uniformly sampled points by
 *
 * \f[ Y(b) = \sum_{k=0}^{N-1} y(b) \cdot e^{j 2\pi k b / N} \f]
 *
 * \f$Y = IFFT\{y\}\f$ is the the inverse FFT transform of \f$y(b)\f$,
 * \f$j\f$ is the imaginary unit, \f$k\f$ and \f$b\f$ range from \f$0\f$ to
 * \f$N-1\f$.
 *
 * \b Note, that due to the underlying FFTW library, the output of a FFT
 * followed by an IFFT (or the other way around) will be scaled i.e.
 * \f$FFT\{ \, IFFT\{x\} \,\} =  N \cdot x \neq x\f$.
 *
 * \see http://www.fftw.org/faq/section3.html#whyscaled
 */
template <class T, bool forward>
class FFT_API fft_v : virtual public sync_block
{
public:
    typedef std::shared_ptr<fft_v<T, forward>> sptr;
    /*! \brief
     * \param[in] fft_size N.
     * \param[in] window Window function to be used.
     * \param[in] shift True moves DC carrier to the middle.
     * \param[in] nthreads Number of underlying threads.
     */
    static sptr make(int fft_size,
                     const std::vector<float>& window,
                     bool shift = false,
                     int nthreads = 1);

    virtual void set_nthreads(int n) = 0;

    virtual int nthreads() const = 0;

    virtual bool set_window(const std::vector<float>& window) = 0;
};

} /* namespace fft */
} /* namespace gr */

#endif /* INCLUDED_FFT_FFT_V_H */

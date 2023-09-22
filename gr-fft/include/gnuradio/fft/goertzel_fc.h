/* -*- c++ -*- */
/*
 * Copyright 2006,2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FFT_GOERTZEL_FC_H
#define INCLUDED_FFT_GOERTZEL_FC_H

#include <gnuradio/fft/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
namespace fft {

/*!
 * \brief Goertzel single-bin DFT calculation.
 * \ingroup fourier_analysis_blk
 */
class FFT_API goertzel_fc : virtual public sync_decimator
{
public:
    // gr::fft::goertzel_fc::sptr
    typedef std::shared_ptr<goertzel_fc> sptr;

    static sptr make(int rate, int len, float freq);

    virtual void set_freq(float freq) = 0;

    virtual void set_rate(int rate) = 0;

    virtual float freq() const = 0;

    virtual int rate() const = 0;
};

} /* namespace fft */
} /* namespace gr */

#endif /* INCLUDED_FFT_GOERTZEL_FC_H */

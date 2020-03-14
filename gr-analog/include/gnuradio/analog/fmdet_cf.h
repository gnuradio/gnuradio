/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_FMDET_CF_H
#define INCLUDED_ANALOG_FMDET_CF_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace analog {

/*!
 * \brief Implements an IQ slope detector
 * \ingroup modulators_blk
 *
 * \details
 * input: stream of complex; output: stream of floats
 *
 * This implements a limiting slope detector. The limiter is in
 * the normalization by the magnitude of the sample
 */
class ANALOG_API fmdet_cf : virtual public sync_block
{
public:
    // gr::analog::fmdet_cf::sptr
    typedef std::shared_ptr<fmdet_cf> sptr;

    /*!
     * \brief Make FM detector block.
     *
     * \param samplerate sample rate of signal (is not used; to be removed)
     * \param freq_low lowest frequency of signal (Hz)
     * \param freq_high highest frequency of signal (Hz)
     * \param scl scale factor
     */
    static sptr make(float samplerate, float freq_low, float freq_high, float scl);

    virtual void set_scale(float scl) = 0;
    virtual void set_freq_range(float freq_low, float freq_high) = 0;

    virtual float freq() const = 0;
    virtual float freq_high() const = 0;
    virtual float freq_low() const = 0;
    virtual float scale() const = 0;
    virtual float bias() const = 0;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_FMDET_CF_H */

/* -*- c++ -*- */
/*
 * Copyright 2006,2011,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_COSTAS_LOOP_CC_H
#define INCLUDED_DIGITAL_COSTAS_LOOP_CC_H

#include <gnuradio/blocks/control_loop.h>
#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

/*!
 * \brief A Costas loop carrier recovery module.
 * \ingroup synchronizers_blk
 *
 * \details
 * The Costas loop locks to the center frequency of a signal and
 * downconverts it to baseband.
 *
 * \li When order=2: used for BPSK where the real part of the
 * output signal is the baseband BPSK signal and the imaginary
 * part is the error signal.
 *
 * \li When order=4: can be used for QPSK where both I and Q (real
 * and imaginary) are outputted.
 *
 * \li When order=8: used for 8PSK.
 *
 * More details can be found online:
 *
 * J. Feigin, "Practical Costas loop design: Designing a simple
 * and inexpensive BPSK Costas loop carrier recovery circuit," RF
 * signal processing, pp. 20-36, 2002.
 *
 * http://rfdesign.com/images/archive/0102Feigin20.pdf
 *
 * The Costas loop can have two output streams:
 * \li stream 1 (required) is the baseband I and Q;
 * \li stream 2 (optional) is the normalized frequency of the loop
 *
 * There is a single optional message input:
 * \li noise: A noise floor estimate used to calculate the SNR of a sample.
 */
class DIGITAL_API costas_loop_cc : virtual public sync_block,
                                   virtual public blocks::control_loop
{
public:
    // gr::digital::costas_loop_cc::sptr
    typedef std::shared_ptr<costas_loop_cc> sptr;

    /*!
     * Make a Costas loop carrier recovery block.
     *
     * \param loop_bw  internal 2nd order loop bandwidth (~ 2pi/100)
     * \param order the loop order, either 2, 4, or 8
     * \param use_snr Use or ignore SNR estimates (from noise message port)
     *        in measurements; also uses tanh instead of slicing.
     */
    static sptr make(float loop_bw, unsigned int order, bool use_snr = false);

    /*!
     * Returns the current value of the loop error.
     */
    virtual float error() const = 0;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_COSTAS_LOOP_CC_H */

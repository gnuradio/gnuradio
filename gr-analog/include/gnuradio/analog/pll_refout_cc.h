/* -*- c++ -*- */
/*
 * Copyright 2004,2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_PLL_REFOUT_CC_H
#define INCLUDED_ANALOG_PLL_REFOUT_CC_H

#include <gnuradio/analog/api.h>
#include <gnuradio/blocks/control_loop.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace analog {

/*!
 * \brief Implements a PLL which locks to the input frequency and outputs a carrier
 * \ingroup synchronizers_blk
 *
 * \details
 * Input stream 0: complex
 * Output stream 0: complex
 *
 * This PLL locks onto a [possibly noisy] reference carrier on the
 * input and outputs a clean version which is phase and frequency
 * aligned to it.
 *
 * All settings max_freq and min_freq are in terms of radians per
 * sample, NOT HERTZ. The loop bandwidth determines the lock range
 * and should be set around pi/200 -- 2pi/100.  \sa
 * pll_freqdet_cf, pll_carriertracking_cc
 */
class ANALOG_API pll_refout_cc : virtual public sync_block,
                                 virtual public blocks::control_loop
{
public:
    // gr::analog::pll_refout_cc::sptr
    typedef std::shared_ptr<pll_refout_cc> sptr;

    /* \brief Make PLL block that outputs the tracked carrier signal.
     *
     * \param loop_bw: control loop's bandwidth parameter.
     * \param max_freq: maximum (normalized) frequency PLL will lock to.
     * \param min_freq: minimum (normalized) frequency PLL will lock to.
     */
    static sptr make(float loop_bw, float max_freq, float min_freq);
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_PLL_REFOUT_CC_H */

/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_FEEDFORWARD_AGC_CC_H
#define INCLUDED_ANALOG_FEEDFORWARD_AGC_CC_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace analog {

/*!
 * \brief Non-causal AGC which computes required gain based on max
 * absolute value over nsamples
 * \ingroup level_controllers_blk
 */
class ANALOG_API feedforward_agc_cc : virtual public sync_block
{
public:
    // gr::analog::feedforward_agc_cc::sptr
    typedef std::shared_ptr<feedforward_agc_cc> sptr;

    /*!
     * Build a complex valued feed-forward AGC loop block.
     *
     * \param nsamples number of samples to look ahead.
     * \param reference reference value to adjust signal power to.
     */
    static sptr make(int nsamples, float reference);
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_GR_FEEDFORWARD_AGC_CC_H */

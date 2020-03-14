/* -*- c++ -*- */
/*
 * Copyright 2007,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_DPLL_BB_H
#define INCLUDED_ANALOG_DPLL_BB_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace analog {

/*!
 * \brief Detect the peak of a signal
 * \ingroup peak_detectors_blk
 *
 * \details
 * If a peak is detected, this block outputs a 1,
 * or it outputs 0's.
 */
class ANALOG_API dpll_bb : virtual public sync_block
{
public:
    // gr::analog::dpll_bb::sptr
    typedef std::shared_ptr<dpll_bb> sptr;

    static sptr make(float period, float gain);

    virtual void set_gain(float gain) = 0;
    virtual void set_decision_threshold(float thresh) = 0;

    virtual float gain() const = 0;
    virtual float freq() const = 0;
    virtual float phase() const = 0;
    virtual float decision_threshold() const = 0;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_DPLL_BB_H */

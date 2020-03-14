/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_SIMPLE_SQUELCH_CC_H
#define INCLUDED_ANALOG_SIMPLE_SQUELCH_CC_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace analog {

/*!
 * \brief simple squelch block based on average signal power and threshold in dB.
 * \ingroup level_controllers_blk
 */
class ANALOG_API simple_squelch_cc : virtual public sync_block
{
public:
    // gr::analog::simple_squelch_cc::sptr
    typedef std::shared_ptr<simple_squelch_cc> sptr;

    /*!
     * \brief Make a simple squelch block.
     *
     * \param threshold_db Threshold for muting.
     * \param alpha Gain parameter for the running average filter.
     */
    static sptr make(double threshold_db, double alpha);

    virtual bool unmuted() const = 0;

    virtual void set_alpha(double alpha) = 0;
    virtual void set_threshold(double decibels) = 0;

    virtual double threshold() const = 0;
    virtual std::vector<float> squelch_range() const = 0;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_SIMPLE_SQUELCH_CC_H */

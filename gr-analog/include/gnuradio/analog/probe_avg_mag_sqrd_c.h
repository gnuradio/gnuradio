/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef INCLUDED_ANALOG_PROBE_AVG_MAG_SQRD_C_H
#define INCLUDED_ANALOG_PROBE_AVG_MAG_SQRD_C_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace analog {

/*!
 * \brief compute avg magnitude squared.
 * \ingroup measurement_tools_blk
 *
 * \details
 * Input stream 0: complex
 *
 * Compute a running average of the magnitude squared of the the
 * input. The level and indication as to whether the level exceeds
 * threshold can be retrieved with the level and unmuted
 * accessors.
 */
class ANALOG_API probe_avg_mag_sqrd_c : virtual public sync_block
{
public:
    // gr::analog::probe_avg_mag_sqrd_c::sptr
    typedef std::shared_ptr<probe_avg_mag_sqrd_c> sptr;

    /*!
     * \brief Make a complex sink that computes avg magnitude squared.
     *
     * \param threshold_db Threshold for muting.
     * \param alpha Gain parameter for the running average filter.
     */
    static sptr make(double threshold_db, double alpha = 0.0001);

    virtual bool unmuted() const = 0;
    virtual double level() const = 0;
    virtual double threshold() const = 0;

    virtual void set_alpha(double alpha) = 0;
    virtual void set_threshold(double decibels) = 0;
    virtual void reset() = 0;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_PROBE_AVG_MAG_SQRD_C_H */

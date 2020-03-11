/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHANNELS_CFO_MODEL_H
#define INCLUDED_CHANNELS_CFO_MODEL_H

#include <gnuradio/channels/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/types.h>

namespace gr {
namespace channels {

/*!
 * \brief channel simulator
 * \ingroup channel_models_blk
 *
 * \details
 * This block implements a carrier frequency offset model that
 * can be used to simulate carrier frequency drift typically from
 * mixer LO drift on either transmit or receive hardware.
 *
 * A clipped gaussian random walk process is used.
 */
class CHANNELS_API cfo_model : virtual public sync_block
{
public:
    // gr::channels::cfo_model::sptr
    typedef boost::shared_ptr<cfo_model> sptr;

    /*! \brief Build the carrier frequency offset model
     *
     * \param sample_rate_hz Sample rate of the input signal in Hz
     * \param std_dev_hz  Desired standard deviation of the random walk
                          process every sample in Hz
     * \param max_dev_hz Maximum carrier frequency deviation in Hz.
     * \param noise_seed A random number generator seed for the noise source.
     */
    static sptr make(double sample_rate_hz,
                     double std_dev_hz,
                     double max_dev_hz,
                     double noise_seed = 0);

    virtual void set_std_dev(double _dev) = 0;
    virtual void set_max_dev(double _dev) = 0;
    virtual void set_samp_rate(double _rate) = 0;

    virtual double std_dev() const = 0;
    virtual double max_dev() const = 0;
    virtual double samp_rate() const = 0;
};

} /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_CFO_MODEL_H */

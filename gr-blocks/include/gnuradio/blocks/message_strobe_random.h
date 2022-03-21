/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_MESSAGE_STROBE_RANDOM_H
#define INCLUDED_GR_MESSAGE_STROBE_RANDOM_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*
 * strobing models
 */
typedef enum {
    STROBE_POISSON = 1,
    STROBE_GAUSSIAN = 2,
    STROBE_UNIFORM = 3,
    STROBE_EXPONENTIAL = 4
} message_strobe_random_distribution_t;

/*!
 * \brief Send message at defined interval
 * \ingroup message_tools_blk
 *
 * \details

 * Takes a PMT message and sends it out at random
 * intervals. The interval is based on a random distribution, \p
 * dist, with specified mean (\p mean_ms) and variance (\p
 * std_ms). Useful for testing/debugging the message system.
 */
class BLOCKS_API message_strobe_random : virtual public block
{
public:
    // gr::blocks::message_strobe_random::sptr
    typedef std::shared_ptr<message_strobe_random> sptr;

    /*!
     * Make a message stobe block to sends message \p msg at random
     * intervals defined by the distribution \p dist with mean \p
     * mean_ms and standard deviation \p std_ms.
     *
     * \param msg The message to send as a PMT.
     * \param dist The random distribution from which to draw the time between
     *             events.
     * \param mean_ms The mean of the distribution, in milliseconds.
     * \param std_ms The standard deviation of the Gaussian distribution,
     *               or the maximum absolute deviation of the Uniform
     *               distribution, in milliseconds. This argument is ignored
     *               for other distributions.
     */
    static sptr make(pmt::pmt_t msg,
                     message_strobe_random_distribution_t dist,
                     float mean_ms,
                     float std_ms);

    /*!
     * Reset the message being sent.
     * \param msg The message to send as a PMT.
     */
    virtual void set_msg(pmt::pmt_t msg) = 0;

    /*!
     * Get the value of the message being sent.
     */
    virtual pmt::pmt_t msg() const = 0;

    /*!
     * \param dist new distribution.
     */
    virtual void set_dist(message_strobe_random_distribution_t dist) = 0;

    /*!
     * get the current distribution.
     */
    virtual message_strobe_random_distribution_t dist() const = 0;

    /*!
     * Reset the sending interval.
     * \param mean delay in milliseconds.
     */
    virtual void set_mean(float mean) = 0;

    /*!
     * Get the time interval of the strobe_random.
     */
    virtual float mean() const = 0;

    /*!
     * Reset the sending interval.
     * \param std delay in milliseconds.
     */
    virtual void set_std(float std) = 0;

    /*!
     * Get the std of strobe_random.
     */
    virtual float std() const = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_STROBE_H */

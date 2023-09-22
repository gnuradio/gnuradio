/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_MESSAGE_STROBE_H
#define INCLUDED_GR_MESSAGE_STROBE_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*!
 * \brief Send message at defined interval
 * \ingroup message_tools_blk
 *
 * \details
 * Takes a PMT message and sends it out every \p period_ms
 * milliseconds. Useful for testing/debugging the message system.
 */
class BLOCKS_API message_strobe : virtual public block
{
public:
    // gr::blocks::message_strobe::sptr
    typedef std::shared_ptr<message_strobe> sptr;

    /*!
     * Make a message stobe block to send message \p msg every \p
     * period_ms milliseconds.
     *
     * \param msg The message to send as a PMT.
     * \param period_ms the time period in milliseconds in which to
     *                  send \p msg.
     */
    static sptr make(pmt::pmt_t msg, long period_ms);

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
     * Reset the sending interval.
     * \param period_ms the time period in milliseconds.
     */
    virtual void set_period(long period_ms) = 0;

    /*!
     * Get the time interval of the strobe.
     */
    virtual long period() const = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_STROBE_H */

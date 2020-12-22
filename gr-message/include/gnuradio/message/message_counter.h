/* -*- c++ -*- */
/*
 * Copyright 2020 NTESS LLC
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_MESSAGE_MESSAGE_COUNTER_H
#define INCLUDED_MESSAGE_MESSAGE_COUNTER_H

#include <gnuradio/block.h>
#include <gnuradio/message/api.h>
#include <string>

namespace gr {
namespace message {

/*!
 * \brief Counts the number of message received by the msg port
 *
 * Prints out message count when the flow graph stops
 *
 * \ingroup message
 *
 */
class MESSAGE_API message_counter : virtual public gr::block
{
public:
    typedef std::shared_ptr<message_counter> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of message::message_counter.
     *
     * To avoid accidental use of raw pointers, message::message_counter's
     * constructor is in a private implementation
     * class. message::message_counter::make is the public interface for
     * creating new instances.
     *
     * @param name - name of counter
     */
    static sptr make(std::string name);

    /**
     * resets the counter to zero
     */
    virtual void reset(void) = 0;

    /**
     * returns the current value of the counter
     *
     * @return uint64_t - counter value.
     */
    virtual uint64_t get_ctr(void) = 0;

    /**
     * returns the counter name
     *
     * @return std::string
     */
    virtual std::string get_name(void) = 0;
};

} // namespace message
} // namespace gr

#endif /* INCLUDED_MESSAGE_MESSAGE_COUNTER_H */

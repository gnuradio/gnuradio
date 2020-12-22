/* -*- c++ -*- */
/*
 * Copyright 2020 NTESS LLC
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_MESSAGE_MESSAGE_EMITTER_H
#define INCLUDED_MESSAGE_MESSAGE_EMITTER_H

#include <gnuradio/block.h>
#include <gnuradio/message/api.h>

namespace gr {
namespace message {

/*!
 * \brief Block emits messages when emit() is called. Designed to be useful
 * for QA tests.
 *
 * \ingroup message
 *
 */
class MESSAGE_API message_emitter : virtual public gr::block
{
public:
    typedef std::shared_ptr<message_emitter> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of message::message_emitter.
     *
     * @param msg - PMT message to emit
     */
    static sptr make(pmt::pmt_t msg = pmt::PMT_NIL);

    /**
     * Set Message to emit
     *
     * @param msg - message to emit
     */
    virtual void set_msg(pmt::pmt_t msg) = 0;

    /**
     * Returns count of total messages emitted
     *
     * @return uint64_t
     */
    virtual uint64_t get_n_msgs(void) = 0;

    /**
     * Emits pre-specified message
     */
    virtual void emit(void) = 0;

    /**
     * Emits passed message.
     * If passed message is invalid, pre-specified message is emitted
     *
     * @param msg - Message to emit
     */
    virtual void emit(pmt::pmt_t msg) = 0;
};

} // namespace message
} // namespace gr

#endif /* INCLUDED_MESSAGE_MESSAGE_EMITTER_H */

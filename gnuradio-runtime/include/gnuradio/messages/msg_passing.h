/* -*- c++ -*- */
/*
 * Copyright 2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_MSG_PASSING_H
#define INCLUDED_MSG_PASSING_H

/*!
 * \brief Include this header to use the message passing features
 */

#include <gnuradio/api.h>
#include <gnuradio/messages/msg_accepter.h>
#include <pmt/pmt.h>

namespace gr {
namespace messages {

/*!
 * \brief send message to msg_accepter
 *
 * \param accepter is the target of the send.
 * \param which_port A pmt symbol describing the port by name.
 * \param msg is the message to send. It's usually a pmt tuple.
 *
 * Sending a message is an asynchronous operation. The \p send
 * call will not wait for the message either to arrive at the
 * destination or to be received.
 *
 * \returns msg
 */
static inline pmt::pmt_t
send(msg_accepter_sptr accepter, const pmt::pmt_t& which_port, const pmt::pmt_t& msg)
{
    accepter->post(which_port, msg);
    return msg;
}

/*!
 * \brief send message to msg_accepter
 *
 * \param accepter is the target of the send.
 * \param which_port A pmt symbol describing the port by name.
 * \param msg is the message to send. It's usually a pmt tuple.
 *
 * Sending a message is an asynchronous operation. The \p send
 * call will not wait for the message either to arrive at the
 * destination or to be received.
 *
 * \returns msg
 */
static inline pmt::pmt_t
send(msg_accepter* accepter, const pmt::pmt_t& which_port, const pmt::pmt_t& msg)
{
    accepter->post(which_port, msg);
    return msg;
}

/*!
 * \brief send message to msg_accepter
 *
 * \param accepter is the target of the send.
 * \param which_port A pmt symbol describing the port by name.
 * \param msg is the message to send.  It's usually a pmt tuple.
 *
 * Sending a message is an asynchronous operation.  The \p send
 * call will not wait for the message either to arrive at the
 * destination or to be received.
 *
 * \returns msg
 */
static inline pmt::pmt_t
send(msg_accepter& accepter, const pmt::pmt_t& which_port, const pmt::pmt_t& msg)
{
    accepter.post(which_port, msg);
    return msg;
}

/*!
 * \brief send message to msg_accepter
 *
 * \param accepter is the target of the send.
 *        aprecond: pmt::is_msg_accepter(accepter)
 * \param which_port A pmt symbol describing the port by name.
 * \param msg is the message to send.  It's usually a pmt tuple.
 *
 * Sending a message is an asynchronous operation. The \p send
 * call will not wait for the message either to arrive at the
 * destination or to be received.
 *
 * \returns msg
 */
static inline pmt::pmt_t
send(pmt::pmt_t accepter, const pmt::pmt_t& which_port, const pmt::pmt_t& msg)
{
    return send(pmt::msg_accepter_ref(accepter), which_port, msg);
}

} /* namespace messages */
} /* namespace gr */

#endif /* INCLUDED_MSG_PASSING_H */

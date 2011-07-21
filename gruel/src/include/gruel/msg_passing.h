/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_GRUEL_MSG_PASSING_H
#define INCLUDED_GRUEL_MSG_PASSING_H

/*!
 * \brief Include this header to use the message passing features
 */

#include <gruel/api.h>
#include <gruel/pmt.h>
#include <gruel/msg_accepter.h>


namespace gruel {

  /*!
   * \brief send message to msg_accepter
   *
   * \param accepter is the target of the send.
   * \param msg is the message to send.  It's usually a pmt tuple.
   *
   * Sending a message is an asynchronous operation.  The \p send
   * call will not wait for the message either to arrive at the
   * destination or to be received.
   *
   * \returns msg
   */
  static inline pmt::pmt_t
  send(msg_accepter_sptr accepter, const pmt::pmt_t &msg)
  {
    accepter->post(msg);
    return msg;
  }

  /*!
   * \brief send message to msg_accepter
   *
   * \param accepter is the target of the send.
   * \param msg is the message to send.  It's usually a pmt tuple.
   *
   * Sending a message is an asynchronous operation.  The \p send
   * call will not wait for the message either to arrive at the
   * destination or to be received.
   *
   * \returns msg
   */
  static inline pmt::pmt_t
  send(msg_accepter *accepter, const pmt::pmt_t &msg)
  {
    accepter->post(msg);
    return msg;
  }

  /*!
   * \brief send message to msg_accepter
   *
   * \param accepter is the target of the send.
   * \param msg is the message to send.  It's usually a pmt tuple.
   *
   * Sending a message is an asynchronous operation.  The \p send
   * call will not wait for the message either to arrive at the
   * destination or to be received.
   *
   * \returns msg
   */
  static inline pmt::pmt_t
  send(msg_accepter &accepter, const pmt::pmt_t &msg)
  {
    accepter.post(msg);
    return msg;
  }

  /*!
   * \brief send message to msg_accepter
   *
   * \param accepter is the target of the send.  precond: pmt_is_msg_accepter(accepter)
   * \param msg is the message to send.  It's usually a pmt tuple.
   *
   * Sending a message is an asynchronous operation.  The \p send
   * call will not wait for the message either to arrive at the
   * destination or to be received.
   *
   * \returns msg
   */
  static inline pmt::pmt_t
  send(pmt::pmt_t accepter, const pmt::pmt_t &msg)
  {
    return send(pmt_msg_accepter_ref(accepter), msg);
  }

} /* namespace gruel */

#endif /* INCLUDED_GRUEL_MSG_PASSING_H */

/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_MESSAGE_DEBUG_H
#define INCLUDED_GR_MESSAGE_DEBUG_H

#include <gr_core_api.h>
#include <gr_block.h>
#include <gr_message.h>
#include <gr_msg_queue.h>
#include <gruel/thread.h>

class gr_message_debug;
typedef boost::shared_ptr<gr_message_debug> gr_message_debug_sptr;

GR_CORE_API gr_message_debug_sptr gr_make_message_debug();

/*!
 * \brief Print received messages to stdout
 * \ingroup sink_blk
 */
class GR_CORE_API gr_message_debug : public gr_block
{
 private:
  friend GR_CORE_API gr_message_debug_sptr
  gr_make_message_debug();

  /*!
   * \brief Messages received in this port are printed to stdout.
   *
   * This port receives messages from the scheduler's message handling
   * mechanism and prints it to stdout. This message handler function
   * is only meant to be used by the scheduler to handle messages
   * posted to port 'print'.
   *
   * \param msg A pmt message passed from the scheduler's message handling.
   */
  void print(pmt::pmt_t msg);

  /*!
   * \brief PDU formatted messages received in this port are printed to stdout.
   *
   * This port receives messages from the scheduler's message handling
   * mechanism and prints it to stdout. This message handler function
   * is only meant to be used by the scheduler to handle messages
   * posted to port 'print'.
   *
   * \param pdu A PDU message passed from the scheduler's message handling.
   */
  void print_pdu(pmt::pmt_t pdu);

  /*!
   * \brief Messages received in this port are stored in a vector.
   *
   * This port receives messages from the scheduler's message handling
   * mechanism and stores it in a vector. Messages can be retrieved
   * later using the 'get_message' function. This message handler
   * function is only meant to be used by the scheduler to handle
   * messages posted to port 'store'.
   *
   * \param msg A pmt message passed from the scheduler's message handling.
   */
  void store(pmt::pmt_t msg);

  gruel::mutex d_mutex;
  std::vector<pmt::pmt_t> d_messages;

 protected:
  gr_message_debug ();

 public:
  ~gr_message_debug ();

  /*! 
   * \brief Reports the number of messages received by this block.
   */
  int num_messages();

  /*!
   * \brief Get a message (as a PMT) from the message vector at index \p i.
   *
   * Messages passed to the 'store' port will be stored in a
   * vector. This function retrieves those messages by index. They are
   * index in order of when they were received (all messages are just
   * pushed onto the back of a vector). This is mostly useful in
   * debugging message passing graphs and in QA code.
   *
   * \param i The index in the vector for the message to retrieve.
   *
   * \return a message at index \p i as a pmt_t.
   */
  pmt::pmt_t get_message(int i);
};

#endif /* INCLUDED_GR_MESSAGE_DEBUG_H */

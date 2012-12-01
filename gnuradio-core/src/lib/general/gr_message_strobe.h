/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_MESSAGE_STROBE_H
#define INCLUDED_GR_MESSAGE_STROBE_H

#include <gr_core_api.h>
#include <gr_block.h>
#include <gr_message.h>
#include <gr_msg_queue.h>

class gr_message_strobe;
typedef boost::shared_ptr<gr_message_strobe> gr_message_strobe_sptr;

GR_CORE_API gr_message_strobe_sptr gr_make_message_strobe (pmt::pmt_t msg, float period_ms);

/*!
 * \brief Send message at defined interval
 * \ingroup msg_blk
 */
class GR_CORE_API gr_message_strobe : public gr_block
{
 private:
  friend GR_CORE_API gr_message_strobe_sptr
  gr_make_message_strobe(pmt::pmt_t msg, float period_ms);

  boost::shared_ptr<boost::thread> d_thread;
  bool d_finished;
  float d_period_ms;
  pmt::pmt_t d_msg;

  void run();

 protected:
  gr_message_strobe (pmt::pmt_t msg, float period_ms);

 public:
  ~gr_message_strobe ();

  void set_msg(pmt::pmt_t msg){ d_msg = msg; }
};

#endif /* INCLUDED_GR_MESSAGE_STROBE_H */

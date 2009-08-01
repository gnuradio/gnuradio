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

#ifndef INCLUDED_GR_MSG_ACCEPTER_H
#define INCLUDED_GR_MSG_ACCEPTER_H

#include <gruel/msg_accepter_msgq.h>

/*!
 * \brief Accepts messages and inserts them into a message queue, then notifies
 * subclass gr_basic_block there is a message pending.
 */
class gr_msg_accepter : gruel::msg_accepter_msgq
{
public:
  gr_msg_accepter(gruel::msg_queue_sptr msgq);
  ~gr_msg_accepter();
  
  void post(pmt::pmt_t msg);
};

#endif /* INCLUDED_GR_MSG_ACCEPTER_H */

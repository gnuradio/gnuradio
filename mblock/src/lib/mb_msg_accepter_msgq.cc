/* -*- c++ -*- */
/*
 * Copyright 2007,2008,2009 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <mb_msg_accepter_msgq.h>
#include <mblock/message.h>

using namespace pmt;

pmt_t s_sys_port = pmt_intern("%sys-port");

mb_msg_accepter_msgq::mb_msg_accepter_msgq(mb_msg_queue *msgq)
  : d_msgq(msgq)
{
}

mb_msg_accepter_msgq::~mb_msg_accepter_msgq()
{
}

void
mb_msg_accepter_msgq::operator()(pmt_t signal, pmt_t data,
				 pmt_t metadata, mb_pri_t priority)
{
  mb_message_sptr msg = mb_make_message(signal, data, metadata, priority);
  msg->set_port_id(s_sys_port);
  d_msgq->insert(msg);
}

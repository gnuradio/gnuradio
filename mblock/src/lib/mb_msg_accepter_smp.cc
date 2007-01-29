/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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
#include <mb_msg_accepter_smp.h>
#include <mb_common.h>
#include <mb_mblock.h>
#include <mb_mblock_impl.h>
#include <mb_message.h>

mb_msg_accepter_smp::mb_msg_accepter_smp(mb_mblock_sptr mblock, pmt_t port_name)
  : d_mb(mblock), d_port_name(port_name)
{
}

mb_msg_accepter_smp::~mb_msg_accepter_smp()
{
  // nop
}

void
mb_msg_accepter_smp::operator()(pmt_t signal, pmt_t data,
				pmt_t metadata, mb_pri_t priority)
{
  mb_message_sptr msg = mb_make_message(signal, data, metadata, priority);
  msg->set_port_id(d_port_name);
  d_mb->impl()->msgq().insert(msg);

  // FIXME tell runtime that we're ready to run
}

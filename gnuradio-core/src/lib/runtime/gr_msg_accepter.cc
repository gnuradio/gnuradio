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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <gr_msg_accepter.h>
#include <gr_block.h>
#include <gr_block_detail.h>

using namespace pmt;

gr_msg_accepter::gr_msg_accepter(gruel::msg_queue_sptr msgq)
  : gruel::msg_accepter_msgq(msgq)
{
}

gr_msg_accepter::~gr_msg_accepter()
{
  // NOP, required as virtual destructor
}

void
gr_msg_accepter::post(pmt_t msg)
{
  // Let parent class do whatever it would have
  gruel::msg_accepter_msgq::post(msg);

  // Notify derived class, handled case by case
  gr_block *p = dynamic_cast<gr_block *>(this);
  if (p) { 
    p->detail()->d_tpb.notify_msg();
    return;
  }

  // Test for other derived classes and handle
}

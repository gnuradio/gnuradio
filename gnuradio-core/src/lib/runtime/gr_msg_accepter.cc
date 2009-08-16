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
#include <gr_hier_block2.h>
#include <stdexcept>

using namespace pmt;

gr_msg_accepter::gr_msg_accepter()
{
}

gr_msg_accepter::~gr_msg_accepter()
{
  // NOP, required as virtual destructor
}

void
gr_msg_accepter::post(pmt_t msg)
{
  // Notify derived class, handled case by case
  gr_block *p = dynamic_cast<gr_block *>(this);
  if (p) { 
    p->detail()->_post(msg);
    return;
  }
  gr_hier_block2 *p2 = dynamic_cast<gr_hier_block2 *>(this);
  if (p2){
    // FIXME do the right thing
    return;
  }

  throw std::runtime_error("unknown derived class");
}

/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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
#include <gr_tpb_detail.h>
#include <gr_block.h>
#include <gr_block_detail.h>
#include <gr_buffer.h>

using namespace pmt;

/*
 * We assume that no worker threads are ever running when the
 * graph structure is being manipulated, thus it's safe for us to poke
 * around in our neighbors w/o holding any locks.
 */

void
gr_tpb_detail::notify_upstream(gr_block_detail *d)
{
  // For each of our inputs, tell the guy upstream that we've consumed
  // some input, and that he most likely has more output buffer space
  // available.

  for (size_t i = 0; i < d->d_input.size(); i++){
    // Can you say, "pointer chasing?"
    d->d_input[i]->buffer()->link()->detail()->d_tpb.set_output_changed();
  }
}

void
gr_tpb_detail::notify_downstream(gr_block_detail *d)
{
  // For each of our outputs, tell the guys downstream that they have
  // new input available.

  for (size_t i = 0; i < d->d_output.size(); i++){
    gr_buffer_sptr buf = d->d_output[i];
    for (size_t j = 0, k = buf->nreaders(); j < k; j++)
      buf->reader(j)->link()->detail()->d_tpb.set_input_changed();
  }
}

void
gr_tpb_detail::notify_neighbors(gr_block_detail *d)
{
  notify_downstream(d);
  notify_upstream(d);
}

void
gr_tpb_detail::insert_tail(pmt::pmt_t msg)
{
  gruel::scoped_lock guard(mutex);

  msg_queue.push_back(msg);

  // wake up thread if BLKD_IN or BLKD_OUT
  input_cond.notify_one();
  output_cond.notify_one();
}

pmt_t 
gr_tpb_detail::delete_head_nowait()
{
  gruel::scoped_lock guard(mutex);

  if (empty_p())
    return pmt_t();

  pmt_t m(msg_queue.front());
  msg_queue.pop_front();

  return m;
}

/*
 * Caller must already be holding the mutex
 */
pmt_t 
gr_tpb_detail::delete_head_nowait_already_holding_mutex()
{
  if (empty_p())
    return pmt_t();

  pmt_t m(msg_queue.front());
  msg_queue.pop_front();

  return m;
}

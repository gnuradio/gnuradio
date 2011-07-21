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
#ifndef INCLUDED_GR_TPB_DETAIL_H
#define INCLUDED_GR_TPB_DETAIL_H

#include <gr_core_api.h>
#include <gruel/thread.h>
#include <deque>
#include <gruel/pmt.h>

class gr_block_detail;

/*!
 * \brief used by thread-per-block scheduler
 */
struct GR_CORE_API gr_tpb_detail {

  gruel::mutex			mutex;			//< protects all vars
  bool				input_changed;
  gruel::condition_variable	input_cond;
  bool				output_changed;
  gruel::condition_variable	output_cond;

private:
  std::deque<pmt::pmt_t>	msg_queue;

public:
  gr_tpb_detail()
    : input_changed(false), output_changed(false) { }

  //! Called by us to tell all our upstream blocks that their output may have changed.
  void notify_upstream(gr_block_detail *d);

  //! Called by us to tell all our downstream blocks that their input may have changed.
  void notify_downstream(gr_block_detail *d);

  //! Called by us to notify both upstream and downstream
  void notify_neighbors(gr_block_detail *d);

  //! Called by us
  void clear_changed()
  {
    gruel::scoped_lock guard(mutex);
    input_changed = false;
    output_changed = false;
  }
  
  //! is the queue empty?
  bool empty_p() const { return msg_queue.empty(); }

  //| Acquires and release the mutex	
  void insert_tail(pmt::pmt_t msg);

  /*!
   * \returns returns pmt at head of queue or pmt_t() if empty.
   */
  pmt::pmt_t delete_head_nowait();

  /*!
   * \returns returns pmt at head of queue or pmt_t() if empty.
   * Caller must already be holding the mutex
   */
  pmt::pmt_t delete_head_nowait_already_holding_mutex();

private:

  //! Used by notify_downstream
  void set_input_changed()
  {
    gruel::scoped_lock guard(mutex);
    input_changed = true;
    input_cond.notify_one();
  }

  //! Used by notify_upstream
  void set_output_changed()
  {
    gruel::scoped_lock guard(mutex);
    output_changed = true;
    output_cond.notify_one();
  }

};

#endif /* INCLUDED_GR_TPB_DETAIL_H */

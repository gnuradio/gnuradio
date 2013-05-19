/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2013 Free Software Foundation, Inc.
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

#include <gnuradio/api.h>
#include <gnuradio/thread/thread.h>
#include <deque>
#include <pmt/pmt.h>

namespace gr {

  class block_detail;

  /*!
   * \brief used by thread-per-block scheduler
   */
  struct GR_RUNTIME_API tpb_detail {
    gr::thread::mutex			mutex;			//< protects all vars
    bool				input_changed;
    gr::thread::condition_variable	input_cond;
    bool				output_changed;
    gr::thread::condition_variable	output_cond;

  public:
    tpb_detail()
      : input_changed(false), output_changed(false) { }

    //! Called by us to tell all our upstream blocks that their output
    //! may have changed.
    void notify_upstream(block_detail *d);

    //! Called by us to tell all our downstream blocks that their
    //! input may have changed.
    void notify_downstream(block_detail *d);

    //! Called by us to notify both upstream and downstream
    void notify_neighbors(block_detail *d);

    //! Called by pmt msg posters
    void notify_msg() {
      input_cond.notify_one();
      output_cond.notify_one();
    }

    //! Called by us
    void clear_changed()
    {
      gr::thread::scoped_lock guard(mutex);
      input_changed = false;
      output_changed = false;
    }

  private:
    //! Used by notify_downstream
    void set_input_changed()
    {
      gr::thread::scoped_lock guard(mutex);
      input_changed = true;
      input_cond.notify_one();
    }

    //! Used by notify_upstream
    void set_output_changed()
    {
      gr::thread::scoped_lock guard(mutex);
      output_changed = true;
      output_cond.notify_one();
    }
  };

} /* namespace gr */

#endif /* INCLUDED_GR_TPB_DETAIL_H */

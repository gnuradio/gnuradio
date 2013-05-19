/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_SCHEDULER_STS_H
#define INCLUDED_GR_SCHEDULER_STS_H

#include <gnuradio/api.h>
#include <gnuradio/thread/thread_group.h>
#include "scheduler.h"

namespace gr {

  /*!
   * \brief Concrete scheduler that uses the single_threaded_scheduler
   */
  class GR_RUNTIME_API scheduler_sts : public scheduler
  {
    gr::thread::thread_group d_threads;

  protected:
    /*!
     * \brief Construct a scheduler and begin evaluating the graph.
     *
     * The scheduler will continue running until all blocks until they
     * report that they are done or the stop method is called.
     */
    scheduler_sts(flat_flowgraph_sptr ffg, int max_noutput_items);

  public:
    static scheduler_sptr make(flat_flowgraph_sptr ffg,
                               int max_noutput_items);

    ~scheduler_sts();

    /*!
     * \brief Tell the scheduler to stop executing.
     */
    void stop();

    /*!
     * \brief Block until the graph is done.
     */
    void wait();
  };

} /* namespace gr */

#endif /* INCLUDED_GR_SCHEDULER_STS_H */

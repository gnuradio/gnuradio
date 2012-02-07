/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_SCHEDULER_H
#define INCLUDED_GR_SCHEDULER_H

#include <gr_core_api.h>
#include <boost/utility.hpp>
#include <gr_block.h>
#include <gr_flat_flowgraph.h>


class gr_scheduler;
typedef boost::shared_ptr<gr_scheduler> gr_scheduler_sptr;


/*!
 * \brief Abstract scheduler that takes a flattened flow graph and runs it.
 *
 * Preconditions: details, buffers and buffer readers have been assigned.
 */
class GR_CORE_API gr_scheduler : boost::noncopyable
{

public:
  /*!
   * \brief Construct a scheduler and begin evaluating the graph.
   *
   * The scheduler will continue running until all blocks until they
   * report that they are done or the stop method is called.
   */
  gr_scheduler(gr_flat_flowgraph_sptr ffg, int max_noutput_items);

  virtual ~gr_scheduler();

  /*!
   * \brief Tell the scheduler to stop executing.
   */
  virtual void stop() = 0;

  /*!
   * \brief Block until the graph is done.
   */
  virtual void wait() = 0;
};

#endif /* INCLUDED_GR_SCHEDULER_H */

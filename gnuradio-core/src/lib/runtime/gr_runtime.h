/* -*- c++ -*- */
/*
 * Copyright 2006,2007 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_RUNTIME_H
#define INCLUDED_GR_RUNTIME_H

#include <gr_runtime_types.h>

class gr_runtime_impl;

gr_runtime_sptr gr_make_runtime(gr_hier_block2_sptr top_block);

/*!
 *\brief Runtime object that controls simple flow graph operation
 *
 * This class is instantiated with a top-level gr_hier_block2. The
 * runtime then flattens the hierarchical block into a gr_simple_flowgraph,
 * and allows control through start(), stop(), wait(), and run().
 * 
 */
class gr_runtime
{
private:
  gr_runtime(gr_hier_block2_sptr top_block);
  friend gr_runtime_sptr gr_make_runtime(gr_hier_block2_sptr top_block);

  gr_runtime_impl *d_impl;
    
public:
  ~gr_runtime();

  /*!
   * Start the flow graph.  Creates an undetached scheduler thread for
   * each flow graph partition. Returns to caller once created.
   */
  void start();
  
  /*!
   * Stop a running flow graph.  Tells each created scheduler thread
   * to exit, then returns to caller.
   */
  void stop();

  /*!
   * Wait for a stopped flow graph to complete.  Joins each completed
   * thread.
   */
  void wait();

  /*!
   * Calls start(), then wait().  Used to run a flow graph that will stop
   * on its own, or to run a flow graph indefinitely until SIGTERM is
   * received().
   */
  void run();

  /*!
   * Restart a running flow graph, after topology changes have
   * been made to its top_block (or children). Causes each created 
   * scheduler thread to end, recalculates the flow graph, and 
   * recreates new threads (possibly a different number from before.)
   */
  void restart();
};

#endif /* INCLUDED_GR_RUNTIME_H */

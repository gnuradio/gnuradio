/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_TOP_BLOCK_H
#define INCLUDED_GR_TOP_BLOCK_H

#include <gr_hier_block2.h>

class gr_top_block_impl;

gr_top_block_sptr gr_make_top_block(const std::string &name);

/*!
 *\brief Top-level hierarchical block representing a flowgraph
 * \ingroup internal
 *
 */
class gr_top_block : public gr_hier_block2
{
private:
  friend gr_top_block_sptr gr_make_top_block(const std::string &name);

  gr_top_block_impl *d_impl;

protected:
  gr_top_block(const std::string &name);
    
public:
  ~gr_top_block();

  /*!
   * \brief The simple interface to running a flowgraph.
   *
   * Calls start() then wait().  Used to run a flowgraph that will stop
   * on its own, or to run a flowgraph indefinitely until SIGINT is
   * received.
   */
  void run();

  /*!
   * Start the contained flowgraph.  Creates one or more threads to
   * execute the flow graph.  Returns to the caller once the threads
   * are created.
   */
  void start();
  
  /*!
   * Stop the running flowgraph.  Notifies each thread created by the
   * scheduler to shutdown, then returns to caller.
   */
  void stop();

  /*!
   * Wait for a flowgraph to complete.  Flowgraphs complete when
   * either (1) all blocks indicate that they are done (typically only
   * when using gr.file_source, or gr.head, or (2) after stop() has been
   * called to request shutdown.
   */
  void wait();

  /*!
   * Lock a flowgraph in preparation for reconfiguration.  When an equal
   * number of calls to lock() and unlock() have occurred, the flowgraph
   * will be restarted automatically.
   *
   * N.B. lock() and unlock() cannot be called from a flowgraph thread
   * (E.g., gr_block::work method) or deadlock will occur when
   * reconfiguration happens.
   */
  virtual void lock();

  /*!
   * Unlock a flowgraph in preparation for reconfiguration.  When an equal
   * number of calls to lock() and unlock() have occurred, the flowgraph
   * will be restarted automatically.
   *
   * N.B. lock() and unlock() cannot be called from a flowgraph thread
   * (E.g., gr_block::work method) or deadlock will occur when
   * reconfiguration happens.
   */
  virtual void unlock();

  /*!
   * Displays flattened flowgraph edges and block connectivity
   */
  void dump();
};

#endif /* INCLUDED_GR_TOP_BLOCK_H */

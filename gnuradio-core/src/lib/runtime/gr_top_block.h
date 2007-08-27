/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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
   * Start the enclosed flowgraph.  Creates an undetached scheduler thread for
   * each flow graph partition. Returns to caller once created.
   */
  void start();
  
  /*!
   * Stop the running flowgraph.  Tells each created scheduler thread
   * to exit, then returns to caller.
   */
  void stop();

  /*!
   * Wait for a stopped flowgraph to complete.  Joins each completed
   * thread.
   */
  void wait();

  /*!
   * Calls start(), then wait().  Used to run a flowgraph that will stop
   * on its own, or to run a flowgraph indefinitely until SIGKILL is
   * received().
   */
  void run();

  /*!
   * Lock a flowgraph in preparation for reconfiguration.  When an equal
   * number of calls to lock() and unlock() have occurred, the flowgraph
   * will be restarted automatically.
   *
   * N.B. lock() and unlock() cannot be called from a flowgraph thread or
   * deadlock will occur when reconfiguration happens.
   */
  virtual void lock();

  /*!
   * Lock a flowgraph in preparation for reconfiguration.  When an equal
   * number of calls to lock() and unlock() have occurred, the flowgraph
   * will be restarted automatically.
   *
   * N.B. lock() and unlock() cannot be called from a flowgraph thread or
   * deadlock will occur when reconfiguration happens.
   */
  virtual void unlock();
};

#endif /* INCLUDED_GR_TOP_BLOCK_H */

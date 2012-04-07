/* -*- c++ -*- */
/*
 * Copyright 2006,2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FLAT_FLOWGRAPH_H
#define INCLUDED_GR_FLAT_FLOWGRAPH_H

#include <gr_core_api.h>
#include <gr_flowgraph.h>
#include <gr_block.h>

// Create a shared pointer to a heap allocated gr_flat_flowgraph
// (types defined in gr_runtime_types.h)
GR_CORE_API gr_flat_flowgraph_sptr gr_make_flat_flowgraph();

/*!
 *\brief Class specializing gr_flat_flowgraph that has all nodes
 * as gr_blocks, with no hierarchy
 * \ingroup internal
 */
class GR_CORE_API gr_flat_flowgraph : public gr_flowgraph
{
public:
  friend GR_CORE_API gr_flat_flowgraph_sptr gr_make_flat_flowgraph();

  // Destruct an arbitrary gr_flat_flowgraph
  ~gr_flat_flowgraph();

  // Wire gr_blocks together in new flat_flowgraph
  void setup_connections();

  // Merge applicable connections from existing flat flowgraph
  void merge_connections(gr_flat_flowgraph_sptr sfg);

  void dump();

  /*!
   * Make a vector of gr_block from a vector of gr_basic_block
   */
  static gr_block_vector_t make_block_vector(gr_basic_block_vector_t &blocks);

private:
  gr_flat_flowgraph();

  gr_block_detail_sptr allocate_block_detail(gr_basic_block_sptr block);
  gr_buffer_sptr allocate_buffer(gr_basic_block_sptr block, int port);
  void connect_block_inputs(gr_basic_block_sptr block);

  /* When reusing a flowgraph's blocks, this call makes sure all of the 
   * buffer's are aligned at the machine's alignment boundary and tells
   * the blocks that they are aligned.
   *
   * Called from both setup_connections and merge_connections for
   * start and restarts.
   */
  void setup_buffer_alignment(gr_block_sptr block);
};

#endif /* INCLUDED_GR_FLAT_FLOWGRAPH_H */

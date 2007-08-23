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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_flat_flowgraph.h>
#include <gr_block_detail.h>
#include <gr_io_signature.h>
#include <gr_buffer.h>
#include <iostream>
#include <map>

#define GR_FLAT_FLOWGRAPH_DEBUG 0

gr_flat_flowgraph_sptr
gr_make_flat_flowgraph()
{
  return gr_flat_flowgraph_sptr(new gr_flat_flowgraph());
}

gr_flat_flowgraph::gr_flat_flowgraph()
{
}

gr_flat_flowgraph::~gr_flat_flowgraph()
{
}

void
gr_flat_flowgraph::setup_connections()
{
  gr_basic_block_vector_t blocks = calc_used_blocks();

  // Assign block details to blocks
  for (gr_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++)
    make_gr_block_sptr(*p)->set_detail(allocate_block_detail(*p));

  // Connect inputs to outputs for each block
  for(gr_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++)
    connect_block_inputs(*p);
}

gr_block_detail_sptr
gr_flat_flowgraph::allocate_block_detail(gr_basic_block_sptr block, gr_block_detail_sptr old_detail)
{
  int ninputs = calc_used_ports(block, true).size();
  int noutputs = calc_used_ports(block, false).size();
  gr_block_detail_sptr detail = gr_make_block_detail(ninputs, noutputs);

  if (GR_FLAT_FLOWGRAPH_DEBUG)
    std::cout << "Creating block detail for " << block << std::endl;

  // Re-use or allocate output buffers
  for (int i = 0; i < noutputs; i++) {
    gr_buffer_sptr buffer;

    if (!old_detail || i >= old_detail->noutputs()) {
      if (GR_FLAT_FLOWGRAPH_DEBUG)
	std::cout << "Allocating new buffer for output " << i << std::endl;
      buffer = allocate_buffer(block, i);
    }
    else {
      if (GR_FLAT_FLOWGRAPH_DEBUG)
	std::cout << "Reusing old buffer for output " << i << std::endl;
      buffer = old_detail->output(i);
    }

    detail->set_output(i, buffer);
  }

  return detail;
}

gr_buffer_sptr
gr_flat_flowgraph::allocate_buffer(gr_basic_block_sptr block, int port)
{
  gr_block_sptr grblock = make_gr_block_sptr(block);
  if (!grblock)
    throw std::runtime_error("allocate_buffer found non-gr_block");
  int item_size = block->output_signature()->sizeof_stream_item(port);
  int nitems = s_fixed_buffer_size/item_size;

  // Make sure there are at least twice the output_multiple no. of items
  if (nitems < 2*grblock->output_multiple())	// Note: this means output_multiple()
    nitems = 2*grblock->output_multiple();	// can't be changed by block dynamically

  // If any downstream blocks are decimators and/or have a large output_multiple,
  // ensure we have a buffer at least twice their decimation factor*output_multiple
  gr_basic_block_vector_t blocks = calc_downstream_blocks(block, port);

  for (gr_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
    gr_block_sptr dgrblock = make_gr_block_sptr(*p);
    if (!dgrblock)
      throw std::runtime_error("allocate_buffer found non-gr_block");

    double decimation = (1.0/dgrblock->relative_rate());
    int multiple      = dgrblock->output_multiple();
    int history       = dgrblock->history();
    nitems = std::max(nitems, static_cast<int>(2*(decimation*multiple+history)));
  }

  return gr_make_buffer(nitems, item_size);
}

void
gr_flat_flowgraph::connect_block_inputs(gr_basic_block_sptr block)
{
  gr_block_sptr grblock = make_gr_block_sptr(block);
  if (!grblock)
    throw std::runtime_error("found non-gr_block");
  
  // Get its detail and edges that feed into it
  gr_block_detail_sptr detail = grblock->detail();
  gr_edge_vector_t in_edges = calc_upstream_edges(block);
  
  // For each edge that feeds into it
  for (gr_edge_viter_t e = in_edges.begin(); e != in_edges.end(); e++) {
    // Set the buffer reader on the destination port to the output
    // buffer on the source port
    int dst_port = e->dst().port();
    int src_port = e->src().port();
    gr_basic_block_sptr src_block = e->src().block();
    gr_block_sptr src_grblock = make_gr_block_sptr(src_block);
    if (!src_grblock)
      throw std::runtime_error("found non-gr_block");
    gr_buffer_sptr src_buffer = src_grblock->detail()->output(src_port);
    
    if (GR_FLAT_FLOWGRAPH_DEBUG)
      std::cout << "Setting input " << dst_port << " from edge " << (*e) << std::endl;

    detail->set_input(dst_port, gr_buffer_add_reader(src_buffer, grblock->history()-1));
  }
}

void
gr_flat_flowgraph::merge_connections(gr_flat_flowgraph_sptr old_ffg)
{
  std::map<gr_block_sptr, gr_block_detail_sptr> old_details;

  // Allocate or reuse output buffers
  for (gr_basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
    gr_block_sptr block = make_gr_block_sptr(*p);

    gr_block_detail_sptr old_detail = block->detail();
    block->set_detail(allocate_block_detail(block, old_detail));

    // Save old detail for use in next step
    old_details[block] = old_detail;
  }

  for (gr_basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
    gr_block_sptr block = make_gr_block_sptr(*p);

    if (GR_FLAT_FLOWGRAPH_DEBUG)
      std::cout << "merge: testing " << (*p) << "...";
    
    if (old_ffg->has_block_p(*p)) {
      // Block exists in old flow graph
      if (GR_FLAT_FLOWGRAPH_DEBUG)
	std::cout << "used in old flow graph" << std::endl;
      gr_block_detail_sptr detail = block->detail();

      // Iterate through the inputs and see what needs to be done
      for (int i = 0; i < detail->ninputs(); i++) {
	if (GR_FLAT_FLOWGRAPH_DEBUG)
	  std::cout << "Checking input " << i << "...";
	gr_edge edge = calc_upstream_edge(*p, i);

	// Fish out old buffer reader and see if it matches correct buffer from edge list
	gr_block_sptr src_block = make_gr_block_sptr(edge.src().block());
	gr_block_detail_sptr src_detail = src_block->detail();
	gr_buffer_sptr src_buffer = src_detail->output(edge.src().port());
	gr_buffer_reader_sptr old_reader;
	gr_block_detail_sptr old_detail = old_details[block];
	if (old_detail && i < old_detail->ninputs())
	  old_reader = old_detail->input(i);
	
	// If there's a match, use it
	if (old_reader && (src_buffer == old_reader->buffer())) {
	  if (GR_FLAT_FLOWGRAPH_DEBUG)
	    std::cout << "matched" << std::endl;
	  detail->set_input(i, old_reader);

	}
	else {
	  if (GR_FLAT_FLOWGRAPH_DEBUG)
	    std::cout << "needs a new reader" << std::endl;

	  // Create new buffer reader and assign
	  detail->set_input(i, gr_buffer_add_reader(src_buffer, block->history()-1));
	}
      }
    }
    else {
      // Block is new, it just needs buffer readers at this point
      if (GR_FLAT_FLOWGRAPH_DEBUG)
	std::cout << "new block" << std::endl;
      connect_block_inputs(block);
    }
  }  
}


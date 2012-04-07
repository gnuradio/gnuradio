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
#include <volk/volk.h>
#include <iostream>
#include <map>

#define GR_FLAT_FLOWGRAPH_DEBUG 0

// 32Kbyte buffer size between blocks
#define GR_FIXED_BUFFER_SIZE (32*(1L<<10))

static const unsigned int s_fixed_buffer_size = GR_FIXED_BUFFER_SIZE;

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
    cast_to_block_sptr(*p)->set_detail(allocate_block_detail(*p));

  // Connect inputs to outputs for each block
  for(gr_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
    connect_block_inputs(*p);

    gr_block_sptr block = cast_to_block_sptr(*p);
    block->set_unaligned(0);
    block->set_is_unaligned(false);
  }

}

gr_block_detail_sptr
gr_flat_flowgraph::allocate_block_detail(gr_basic_block_sptr block)
{
  int ninputs = calc_used_ports(block, true).size();
  int noutputs = calc_used_ports(block, false).size();
  gr_block_detail_sptr detail = gr_make_block_detail(ninputs, noutputs);

  if (GR_FLAT_FLOWGRAPH_DEBUG)
    std::cout << "Creating block detail for " << block << std::endl;

  for (int i = 0; i < noutputs; i++) {
    gr_buffer_sptr buffer = allocate_buffer(block, i);
    if (GR_FLAT_FLOWGRAPH_DEBUG)
      std::cout << "Allocated buffer for output " << block << ":" << i << std::endl;
    detail->set_output(i, buffer);
  }

  return detail;
}

gr_buffer_sptr
gr_flat_flowgraph::allocate_buffer(gr_basic_block_sptr block, int port)
{
  gr_block_sptr grblock = cast_to_block_sptr(block);
  if (!grblock)
    throw std::runtime_error("allocate_buffer found non-gr_block");
  int item_size = block->output_signature()->sizeof_stream_item(port);

  // *2 because we're now only filling them 1/2 way in order to
  // increase the available parallelism when using the TPB scheduler.
  // (We're double buffering, where we used to single buffer)
  int nitems = s_fixed_buffer_size * 2 / item_size;

  // Make sure there are at least twice the output_multiple no. of items
  if (nitems < 2*grblock->output_multiple())	// Note: this means output_multiple()
    nitems = 2*grblock->output_multiple();	// can't be changed by block dynamically

  // If any downstream blocks are decimators and/or have a large output_multiple,
  // ensure we have a buffer at least twice their decimation factor*output_multiple
  gr_basic_block_vector_t blocks = calc_downstream_blocks(block, port);

  for (gr_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
    gr_block_sptr dgrblock = cast_to_block_sptr(*p);
    if (!dgrblock)
      throw std::runtime_error("allocate_buffer found non-gr_block");

    double decimation = (1.0/dgrblock->relative_rate());
    int multiple      = dgrblock->output_multiple();
    int history       = dgrblock->history();
    nitems = std::max(nitems, static_cast<int>(2*(decimation*multiple+history)));
  }

  return gr_make_buffer(nitems, item_size, grblock);
}

void
gr_flat_flowgraph::connect_block_inputs(gr_basic_block_sptr block)
{
  gr_block_sptr grblock = cast_to_block_sptr(block);
  if (!grblock)
    throw std::runtime_error("connect_block_inputs found non-gr_block");
  
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
    gr_block_sptr src_grblock = cast_to_block_sptr(src_block);
    if (!src_grblock)
      throw std::runtime_error("connect_block_inputs found non-gr_block");
    gr_buffer_sptr src_buffer = src_grblock->detail()->output(src_port);
    
    if (GR_FLAT_FLOWGRAPH_DEBUG)
      std::cout << "Setting input " << dst_port << " from edge " << (*e) << std::endl;

    detail->set_input(dst_port, gr_buffer_add_reader(src_buffer, grblock->history()-1, grblock));
  }
}

void
gr_flat_flowgraph::merge_connections(gr_flat_flowgraph_sptr old_ffg)
{
  // Allocate block details if needed.  Only new blocks that aren't pruned out
  // by flattening will need one; existing blocks still in the new flowgraph will
  // already have one.
  for (gr_basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
    gr_block_sptr block = cast_to_block_sptr(*p);
    
    if (!block->detail()) {
      if (GR_FLAT_FLOWGRAPH_DEBUG)
        std::cout << "merge: allocating new detail for block " << (*p) << std::endl;
        block->set_detail(allocate_block_detail(block));
    }
    else
      if (GR_FLAT_FLOWGRAPH_DEBUG)
        std::cout << "merge: reusing original detail for block " << (*p) << std::endl;
  }

  // Calculate the old edges that will be going away, and clear the buffer readers
  // on the RHS.
  for (gr_edge_viter_t old_edge = old_ffg->d_edges.begin(); old_edge != old_ffg->d_edges.end(); old_edge++) {
    if (GR_FLAT_FLOWGRAPH_DEBUG)
      std::cout << "merge: testing old edge " << (*old_edge) << "...";
      
    gr_edge_viter_t new_edge;
    for (new_edge = d_edges.begin(); new_edge != d_edges.end(); new_edge++)
      if (new_edge->src() == old_edge->src() &&
	  new_edge->dst() == old_edge->dst())
	break;

    if (new_edge == d_edges.end()) { // not found in new edge list
      if (GR_FLAT_FLOWGRAPH_DEBUG)
	std::cout << "not in new edge list" << std::endl;
      // zero the buffer reader on RHS of old edge
      gr_block_sptr block(cast_to_block_sptr(old_edge->dst().block()));
      int port = old_edge->dst().port();
      block->detail()->set_input(port, gr_buffer_reader_sptr());
    }
    else {
      if (GR_FLAT_FLOWGRAPH_DEBUG)
	std::cout << "found in new edge list" << std::endl;
    }
  }  

  // Now connect inputs to outputs, reusing old buffer readers if they exist
  for (gr_basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
    gr_block_sptr block = cast_to_block_sptr(*p);

    if (GR_FLAT_FLOWGRAPH_DEBUG)
      std::cout << "merge: merging " << (*p) << "...";
    
    if (old_ffg->has_block_p(*p)) {
      // Block exists in old flow graph
      if (GR_FLAT_FLOWGRAPH_DEBUG)
	std::cout << "used in old flow graph" << std::endl;
      gr_block_detail_sptr detail = block->detail();
	
      // Iterate through the inputs and see what needs to be done
      int ninputs = calc_used_ports(block, true).size(); // Might be different now
      for (int i = 0; i < ninputs; i++) {
	if (GR_FLAT_FLOWGRAPH_DEBUG)
	  std::cout << "Checking input " << block << ":" << i << "...";
	gr_edge edge = calc_upstream_edge(*p, i);

	// Fish out old buffer reader and see if it matches correct buffer from edge list
	gr_block_sptr src_block = cast_to_block_sptr(edge.src().block());
	gr_block_detail_sptr src_detail = src_block->detail();
	gr_buffer_sptr src_buffer = src_detail->output(edge.src().port());
	gr_buffer_reader_sptr old_reader;
	if (i < detail->ninputs()) // Don't exceed what the original detail has 
	  old_reader = detail->input(i);
	
	// If there's a match, use it
	if (old_reader && (src_buffer == old_reader->buffer())) {
	  if (GR_FLAT_FLOWGRAPH_DEBUG)
	    std::cout << "matched, reusing" << std::endl;
	}
	else {
	  if (GR_FLAT_FLOWGRAPH_DEBUG)
	    std::cout << "needs a new reader" << std::endl;

	  // Create new buffer reader and assign
	  detail->set_input(i, gr_buffer_add_reader(src_buffer, block->history()-1, block));
	}
      }
    }
    else {
      // Block is new, it just needs buffer readers at this point
      if (GR_FLAT_FLOWGRAPH_DEBUG)
	std::cout << "new block" << std::endl;
      connect_block_inputs(block);

      // Make sure all buffers are aligned
      setup_buffer_alignment(block);
    }

    // Now deal with the fact that the block details might have changed numbers of 
    // inputs and outputs vs. in the old flowgraph.
  }  
}

void
gr_flat_flowgraph::setup_buffer_alignment(gr_block_sptr block)
{
  const int alignment = volk_get_alignment();
  for(int i = 0; i < block->detail()->ninputs(); i++) {
    void *r = (void*)block->detail()->input(i)->read_pointer();
    unsigned long int ri = (unsigned long int)r % alignment;
    //std::cout << "reader: " << r << "  alignment: " << ri << std::endl;
    if(ri != 0) {
      size_t itemsize = block->detail()->input(i)->get_sizeof_item();
      block->detail()->input(i)->update_read_pointer(ri/itemsize);
    }
    block->set_unaligned(0);
    block->set_is_unaligned(false);
  }

  for(int i = 0; i < block->detail()->noutputs(); i++) {
    void *w = (void*)block->detail()->output(i)->write_pointer();
    unsigned long int wi = (unsigned long int)w % alignment;
    size_t itemsize = block->detail()->output(i)->get_sizeof_item();
    //std::cout << "writer: " << w << "  alignment: " << wi << std::endl;
    if(wi != 0) {
      block->detail()->output(i)->update_write_pointer(wi/itemsize);
    }
    block->set_unaligned(0);
    block->set_is_unaligned(false);
  }
}

void gr_flat_flowgraph::dump()
{
  for (gr_edge_viter_t e = d_edges.begin(); e != d_edges.end(); e++)
     std::cout << " edge: " << (*e) << std::endl;

  for (gr_basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
    std::cout << " block: " << (*p) << std::endl;
    gr_block_detail_sptr detail = cast_to_block_sptr(*p)->detail();
    std::cout << "  detail @" << detail << ":" << std::endl;
     
    int ni = detail->ninputs();
    int no = detail->noutputs();
    for (int i = 0; i < no; i++) {
      gr_buffer_sptr buffer = detail->output(i);
      std::cout << "   output " << i << ": " << buffer << std::endl;
    }

    for (int i = 0; i < ni; i++) {
      gr_buffer_reader_sptr reader = detail->input(i);
      std::cout << "   reader " <<  i << ": " << reader
                << " reading from buffer=" << reader->buffer() << std::endl;
    }
  }

}

gr_block_vector_t
gr_flat_flowgraph::make_block_vector(gr_basic_block_vector_t &blocks)
{
  gr_block_vector_t result;
  for (gr_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
    result.push_back(cast_to_block_sptr(*p));
  }

  return result;
}

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_simple_flowgraph.h>
#include <gr_simple_flowgraph_detail.h>
#include <gr_io_signature.h>
#include <gr_block_detail.h>
#include <gr_buffer.h>
#include <iostream>
#include <stdexcept>

gr_edge_sptr
gr_make_edge(const gr_endpoint &src, const gr_endpoint &dst)
{
  return gr_edge_sptr(new gr_edge(src, dst));
}

gr_edge::~gr_edge()
{
}

gr_simple_flowgraph_detail::~gr_simple_flowgraph_detail()
{
}

void
gr_simple_flowgraph_detail::reset()
{
  // Boost shared pointers will deallocate as needed
  d_edges.clear();
  d_blocks.clear();
}

void
gr_simple_flowgraph_detail::connect(const gr_endpoint &src, const gr_endpoint &dst)
{
  check_valid_port(src.block()->output_signature(), src.port());
  check_valid_port(dst.block()->input_signature(), dst.port());
  check_dst_not_used(dst);
  check_type_match(src, dst);
  d_edges.push_back(gr_make_edge(src,dst));
}

void
gr_simple_flowgraph_detail::disconnect(const gr_endpoint &src, const gr_endpoint &dst)
{
  for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++) {
    if (src.block() == (*p)->src().block() && src.port() == (*p)->src().port() && 
	dst.block() == (*p)->dst().block() && dst.port() == (*p)->dst().port()) {
      d_edges.erase(p);
      return;
    }
  }

  throw std::invalid_argument("edge to disconnect not found");
}

void
gr_simple_flowgraph_detail::check_valid_port(gr_io_signature_sptr sig, int port)
{
  if (port < 0)
    throw std::invalid_argument("negative port number");
  if (sig->max_streams() >= 0 && port >= sig->max_streams())
    throw std::invalid_argument("port number exceeds max");
}

void
gr_simple_flowgraph_detail::check_dst_not_used(const gr_endpoint &dst)
{
  for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++)
    if ((*p)->dst().block() == dst.block() &&
        (*p)->dst().port() == dst.port())
      throw std::invalid_argument("dst already in use");
}

void
gr_simple_flowgraph_detail::check_type_match(const gr_endpoint &src, const gr_endpoint &dst)
{
  int src_size = src.block()->output_signature()->sizeof_stream_item(src.port());
  int dst_size = dst.block()->input_signature()->sizeof_stream_item(dst.port());

  if (src_size != dst_size)
    throw std::invalid_argument("type size mismatch while attempting to connect");
}

void
gr_simple_flowgraph_detail::validate()
{
  d_blocks = calc_used_blocks();

  for (gr_basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
    std::vector<int> used_ports;
    int ninputs, noutputs;

    used_ports = calc_used_ports(*p, true); // inputs
    ninputs = used_ports.size();
    check_contiguity(*p, used_ports, true); // inputs

    used_ports = calc_used_ports(*p, false); // outputs
    noutputs = used_ports.size();
    check_contiguity(*p, used_ports, false); // outputs

    if (!((*p)->check_topology(ninputs, noutputs)))
      throw std::runtime_error("check topology failed");
  }
}

std::vector<int>
gr_simple_flowgraph_detail::calc_used_ports(gr_basic_block_sptr block, bool check_inputs)
{
  std::vector<int> tmp, result;
  std::insert_iterator<std::vector<int> > inserter(result, result.begin());

  gr_edge_vector_t edges = calc_connections(block, check_inputs);
  for (gr_edge_viter_t p = edges.begin(); p != edges.end(); p++) {
    if (check_inputs == true)
      tmp.push_back((*p)->dst().port());
    else
      tmp.push_back((*p)->src().port());
  }

  // remove duplicates
  std::sort(tmp.begin(), tmp.end());
  std::unique_copy(tmp.begin(), tmp.end(), inserter);
  return result;
}

gr_edge_vector_t
gr_simple_flowgraph_detail::calc_connections(gr_basic_block_sptr block, bool check_inputs)
{
  gr_edge_vector_t result;

  for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++) {
    if (check_inputs) {
      if ((*p)->dst().block() == block)
	result.push_back(*p);
    }
    else {
      if ((*p)->src().block() == block)
	result.push_back(*p);
    }
  }

  return result;    // assumes no duplicates
}

void
gr_simple_flowgraph_detail::check_contiguity(gr_basic_block_sptr block,
                                             const std::vector<int> &used_ports,
                                             bool check_inputs)
{
  gr_io_signature_sptr sig =
    check_inputs ? block->input_signature() : block->output_signature();

  int nports = used_ports.size();
  int min_ports = sig->min_streams();

  if (nports == 0) {
    if (min_ports == 0)
      return;
    else
      throw std::runtime_error("insufficient ports");
  }

  if (used_ports[nports-1]+1 != nports) {
    for (int i = 0; i < nports; i++)
      if (used_ports[i] != i)
	throw std::runtime_error("missing input assignment");
  }
}

void
gr_simple_flowgraph_detail::setup_connections()
{
  // Assign block details to blocks
  for (gr_basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
    int ninputs = calc_used_ports(*p, true).size();
    int noutputs = calc_used_ports(*p, false).size();
    gr_block_detail_sptr detail = gr_make_block_detail(ninputs, noutputs);
    for (int i = 0; i < noutputs; i++)
      detail->set_output(i, allocate_buffer(*p, i));

    boost::dynamic_pointer_cast<gr_block, gr_basic_block>(*p)->set_detail(detail);
  }

  // Connect inputs to outputs for each block
  for(gr_basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
    gr_block_sptr grblock(boost::dynamic_pointer_cast<gr_block, gr_basic_block>(*p));
    if (!grblock)
      throw std::runtime_error("setup_connections found non-gr_block");

    // Get its detail and edges that feed into it
    gr_block_detail_sptr detail = grblock->detail();
    gr_edge_vector_t in_edges = calc_upstream_edges(*p);

    // For each edge that feeds into it
    for (gr_edge_viter_t e = in_edges.begin(); e != in_edges.end(); e++) {
      // Set the input reader on the destination port to the output
      // buffer on the source port
      int dst_port = (*e)->dst().port();
      int src_port = (*e)->src().port();
      gr_basic_block_sptr src_block = (*e)->src().block();
      gr_block_sptr src_grblock(boost::dynamic_pointer_cast<gr_block, gr_basic_block>(src_block));
      if (!grblock)
	throw std::runtime_error("setup_connections found non-gr_block");
      gr_buffer_sptr src_buffer = src_grblock->detail()->output(src_port);

      detail->set_input(dst_port, gr_buffer_add_reader(src_buffer, grblock->history()-1));
    }
  }
}

gr_buffer_sptr
gr_simple_flowgraph_detail::allocate_buffer(gr_basic_block_sptr block, int port)
{
  gr_block_sptr grblock(boost::dynamic_pointer_cast<gr_block, gr_basic_block>(block));
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
    gr_block_sptr dgrblock(boost::dynamic_pointer_cast<gr_block, gr_basic_block>(*p));
      if (!dgrblock)
	throw std::runtime_error("allocate_buffer found non-gr_block");
    int decimation = (int)(1.0/dgrblock->relative_rate());
    int multiple   = dgrblock->output_multiple();
    int history    = dgrblock->history();
    nitems = std::max(nitems, 2*(decimation*multiple+history));
  }

  return gr_make_buffer(nitems, item_size);
}

gr_basic_block_vector_t
gr_simple_flowgraph_detail::calc_downstream_blocks(gr_basic_block_sptr block, int port)
{
  gr_basic_block_vector_t tmp, result;
  std::insert_iterator<gr_basic_block_vector_t> inserter(result, result.begin());

  for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++)
    if ((*p)->src().block() == block && (*p)->src().port() == port)
      tmp.push_back((*p)->dst().block());

  // Remove duplicates
  sort(tmp.begin(), tmp.end());
  unique_copy(tmp.begin(), tmp.end(), inserter);
  return result;
}

gr_basic_block_vector_t
gr_simple_flowgraph_detail::calc_downstream_blocks(gr_basic_block_sptr block)
{
  gr_basic_block_vector_t tmp, result;
  std::insert_iterator<gr_basic_block_vector_t> inserter(result, result.begin());

  for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++)
    if ((*p)->src().block() == block)
      tmp.push_back((*p)->dst().block());

  // Remove duplicates
  sort(tmp.begin(), tmp.end());
  unique_copy(tmp.begin(), tmp.end(), inserter);
  return result;
}

gr_edge_vector_t
gr_simple_flowgraph_detail::calc_upstream_edges(gr_basic_block_sptr block)
{
  gr_edge_vector_t result;

  for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++)
    if ((*p)->dst().block() == block)
      result.push_back(*p);

  return result; // Assume no duplicates
}

gr_basic_block_vector_t
gr_simple_flowgraph_detail::calc_used_blocks()
{
  gr_basic_block_vector_t tmp, result;
  std::insert_iterator<gr_basic_block_vector_t> inserter(result, result.begin());

  for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++) {
    tmp.push_back((*p)->src().block());
    tmp.push_back((*p)->dst().block());
  }

  sort(tmp.begin(), tmp.end());
  unique_copy(tmp.begin(), tmp.end(), inserter);
  return result;
}

std::vector<gr_block_vector_t>
gr_simple_flowgraph_detail::partition()
{
  std::vector<gr_block_vector_t> result;
  gr_basic_block_vector_t blocks = calc_used_blocks();
  gr_basic_block_vector_t graph;

  while (blocks.size() > 0) {
    graph = calc_reachable_blocks(blocks[0], blocks);
    assert(graph.size());
    result.push_back(topological_sort(graph));

    for (gr_basic_block_viter_t p = graph.begin(); p != graph.end(); p++)
      blocks.erase(find(blocks.begin(), blocks.end(), *p));
  }

  return result;
}

gr_basic_block_vector_t
gr_simple_flowgraph_detail::calc_reachable_blocks(gr_basic_block_sptr block, gr_basic_block_vector_t &blocks)
{
  gr_basic_block_vector_t result;

  // Mark all blocks as unvisited
  for (gr_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++)
    boost::dynamic_pointer_cast<gr_block, gr_basic_block>(*p)->detail()->set_color(gr_block_detail::WHITE);

  // Recursively mark all reachable blocks
  reachable_dfs_visit(block, blocks);

  // Collect all the blocks that have been visited
  for (gr_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++)
    if ((boost::dynamic_pointer_cast<gr_block, gr_basic_block>(*p))->detail()->color() == gr_block_detail::BLACK)
      result.push_back(*p);

  return result;
}

// Recursively mark all reachable blocks from given block and block list
void 
gr_simple_flowgraph_detail::reachable_dfs_visit(gr_basic_block_sptr block, gr_basic_block_vector_t &blocks)
{
  // Mark the current one as visited
  boost::dynamic_pointer_cast<gr_block, gr_basic_block>(block)->detail()->set_color(gr_block_detail::BLACK);

  // Recurse into adjacent vertices
  gr_basic_block_vector_t adjacent = calc_adjacent_blocks(block, blocks);

  for (gr_basic_block_viter_t p = adjacent.begin(); p != adjacent.end(); p++)
    if (boost::dynamic_pointer_cast<gr_block, gr_basic_block>(*p)->detail()->color() == gr_block_detail::WHITE)
      reachable_dfs_visit(*p, blocks);
}

// Return a list of block adjacent to a given block along any edge
gr_basic_block_vector_t 
gr_simple_flowgraph_detail::calc_adjacent_blocks(gr_basic_block_sptr block, gr_basic_block_vector_t &blocks)
{
  gr_basic_block_vector_t tmp, result;
  std::insert_iterator<gr_basic_block_vector_t> inserter(result, result.begin());
    
  // Find any blocks that are inputs or outputs
  for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++) {

    if ((*p)->src().block() == block)
      tmp.push_back((*p)->dst().block());
    if ((*p)->dst().block() == block)
      tmp.push_back((*p)->src().block());
  }    

  // Remove duplicates
  sort(tmp.begin(), tmp.end());
  unique_copy(tmp.begin(), tmp.end(), inserter);
  return result;
}

gr_block_vector_t
gr_simple_flowgraph_detail::topological_sort(gr_basic_block_vector_t &blocks)
{
  gr_basic_block_vector_t tmp;
  gr_block_vector_t result;
  tmp = sort_sources_first(blocks);

  // Start 'em all white
  for (gr_basic_block_viter_t p = tmp.begin(); p != tmp.end(); p++)
    boost::dynamic_pointer_cast<gr_block, gr_basic_block>(*p)->detail()->set_color(gr_block_detail::WHITE);

  for (gr_basic_block_viter_t p = tmp.begin(); p != tmp.end(); p++) {
    if (boost::dynamic_pointer_cast<gr_block, gr_basic_block>(*p)->detail()->color() == gr_block_detail::WHITE)
      topological_dfs_visit(*p, result);
  }    

  reverse(result.begin(), result.end());

  return result;
}

bool
gr_simple_flowgraph_detail::source_p(gr_basic_block_sptr block)
{
  return (calc_upstream_edges(block).size() == 0);
}

gr_basic_block_vector_t
gr_simple_flowgraph_detail::sort_sources_first(gr_basic_block_vector_t &blocks)
{
  gr_basic_block_vector_t sources, nonsources, result;

  for (gr_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
    if (source_p(*p))
      sources.push_back(*p);
    else
      nonsources.push_back(*p);
  }

  for (gr_basic_block_viter_t p = sources.begin(); p != sources.end(); p++)
    result.push_back(*p);

  for (gr_basic_block_viter_t p = nonsources.begin(); p != nonsources.end(); p++)
    result.push_back(*p);

  return result;
}

void
gr_simple_flowgraph_detail::topological_dfs_visit(gr_basic_block_sptr block, gr_block_vector_t &output)
{
  boost::dynamic_pointer_cast<gr_block, gr_basic_block>(block)->detail()->set_color(gr_block_detail::GREY);

  gr_basic_block_vector_t blocks(calc_downstream_blocks(block));

  for (gr_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
    switch (boost::dynamic_pointer_cast<gr_block, gr_basic_block>(*p)->detail()->color()) {
    case gr_block_detail::WHITE:           
      topological_dfs_visit(*p, output);
      break;

    case gr_block_detail::GREY:            
      throw std::runtime_error("flow graph has loops!");

    case gr_block_detail::BLACK:
      continue;

    default:
      throw std::runtime_error("invalid color on block!");
    }
  }

  gr_block_sptr result_block(boost::dynamic_pointer_cast<gr_block, gr_basic_block>(block));

  result_block->detail()->set_color(gr_block_detail::BLACK);
  output.push_back(result_block);
}

void
gr_simple_flowgraph_detail::merge_connections(gr_simple_flowgraph_sptr sfg)
{
    // NOT IMPLEMENTED
}

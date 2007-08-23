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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_hier_block2_detail.h>
#include <gr_io_signature.h>
#include <gr_runtime.h>
#include <stdexcept>
#include <iostream>

#define GR_HIER_BLOCK2_DETAIL_DEBUG 0

gr_hier_block2_detail::gr_hier_block2_detail(gr_hier_block2 *owner) :
  d_owner(owner), 
  d_parent_detail(0),
  d_fg(gr_make_flowgraph()),
  d_inputs(owner->input_signature()->max_streams()),
  d_outputs(owner->output_signature()->max_streams()),
  d_runtime()
{
}

gr_hier_block2_detail::~gr_hier_block2_detail()
{
  d_owner = 0; // Don't use delete, we didn't allocate
}

void 
gr_hier_block2_detail::connect(gr_basic_block_sptr src, int src_port, 
                               gr_basic_block_sptr dst, int dst_port)
{
  if (GR_HIER_BLOCK2_DETAIL_DEBUG)
    std::cout << "connecting: " << gr_endpoint(src, src_port)
              << " -> " << gr_endpoint(dst, dst_port) << std::endl;

  if (src.get() == dst.get())
    throw std::invalid_argument("src and destination blocks cannot be the same");

  gr_hier_block2_sptr src_block(boost::dynamic_pointer_cast<gr_hier_block2, gr_basic_block>(src));
  gr_hier_block2_sptr dst_block(boost::dynamic_pointer_cast<gr_hier_block2, gr_basic_block>(dst));

  if (src_block && src.get() != d_owner) {
    if (GR_HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << "connect: src is hierarchical, setting parent to " << this << std::endl;
    src_block->d_detail->d_parent_detail = this;
  }
		
  if (dst_block && dst.get() != d_owner) {
    if (GR_HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << "connect: dst is hierarchical, setting parent to " << this << std::endl;
    dst_block->d_detail->d_parent_detail = this;
  }

  // Connections to block inputs or outputs
  int max_port;
  if (src.get() == d_owner) {
    max_port = src->input_signature()->max_streams();
    if ((max_port != -1 && (src_port >= max_port)) || src_port < 0)
      throw std::invalid_argument("source port out of range");
    return connect_input(src_port, dst_port, dst);
  }

  if (dst.get() == d_owner) {
    max_port = dst->output_signature()->max_streams();
    if ((max_port != -1 && (dst_port >= max_port)) || dst_port < 0)
      throw std::invalid_argument("source port out of range");
    return connect_output(dst_port, src_port, src);
  }

  // Internal connections
  d_fg->connect(src, src_port, dst, dst_port);

  // TODO: connects to NC
}

void 
gr_hier_block2_detail::disconnect(gr_basic_block_sptr src, int src_port, 
                                  gr_basic_block_sptr dst, int dst_port)
{
  if (GR_HIER_BLOCK2_DETAIL_DEBUG)
    std::cout << "disconnecting: " << gr_endpoint(src, src_port)
              << " -> " << gr_endpoint(dst, dst_port) << std::endl;

  if (src.get() == dst.get())
    throw std::invalid_argument("src and destination blocks cannot be the same");

  gr_hier_block2_sptr src_block(boost::dynamic_pointer_cast<gr_hier_block2, gr_basic_block>(src));
  gr_hier_block2_sptr dst_block(boost::dynamic_pointer_cast<gr_hier_block2, gr_basic_block>(dst));

  if (src_block && src.get() != d_owner) {
    if (GR_HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << "connect: src is hierarchical, clearing parent" << std::endl;
    src_block->d_detail->d_parent_detail = 0;
  }
		
  if (dst_block && dst.get() != d_owner) {
    if (GR_HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << "connect: dst is hierarchical, clearing parent" << std::endl;
    dst_block->d_detail->d_parent_detail = 0;
  }

  if (src.get() == d_owner)
    return disconnect_input(src_port, dst_port, dst);

  if (dst.get() == d_owner)
    return disconnect_output(dst_port, src_port, src);

  // Internal connections
  d_fg->disconnect(src, src_port, dst, dst_port);
}

void
gr_hier_block2_detail::connect_input(int my_port, int port, gr_basic_block_sptr block)
{
  if (my_port < 0 || my_port >= (signed)d_inputs.size())
    throw std::invalid_argument("input port number out of range");

  if (d_inputs[my_port].block())
    throw std::invalid_argument("input port in use");

  d_inputs[my_port] = gr_endpoint(block, port);
}

void
gr_hier_block2_detail::connect_output(int my_port, int port, gr_basic_block_sptr block)
{
  if (my_port < 0 || my_port >= (signed)d_outputs.size())
    throw std::invalid_argument("output port number out of range");

  if (d_outputs[my_port].block())
    throw std::invalid_argument("output port in use");

  d_outputs[my_port] = gr_endpoint(block, port);
}

void
gr_hier_block2_detail::disconnect_input(int my_port, int port, gr_basic_block_sptr block)
{
  if (my_port < 0 || my_port >= (signed)d_inputs.size())
    throw std::invalid_argument("input port number out of range");

  if (d_inputs[my_port].block() != block)
    throw std::invalid_argument("block not assigned to given input, can't disconnect");

  d_inputs[my_port] = gr_endpoint();
}

void
gr_hier_block2_detail::disconnect_output(int my_port, int port, gr_basic_block_sptr block)
{
  if (my_port < 0 || my_port >= (signed)d_outputs.size())
    throw std::invalid_argument("input port number out of range");

  if (d_outputs[my_port].block() != block)
    throw std::invalid_argument("block not assigned to given output, can't disconnect");

  d_outputs[my_port] = gr_endpoint();
}

gr_endpoint
gr_hier_block2_detail::resolve_port(int port, bool is_input)
{
  if (GR_HIER_BLOCK2_DETAIL_DEBUG)
    std::cout << "Resolving port " << port << " as an "
	      << (is_input ? "input" : "output")
	      << " of " << d_owner->name() << std::endl;

  gr_endpoint result;

  if (is_input) {
    if (port < 0 || port >= (signed)d_inputs.size())
      throw std::runtime_error("input port number out of range");
    result = resolve_endpoint(d_inputs[port], true);
  }
  else {
    if (port < 0 || port >= (signed)d_outputs.size())
      throw std::runtime_error("output port number out of range");
    result = resolve_endpoint(d_outputs[port], false);
  }

  if (!result.block())
    throw std::runtime_error("unable to resolve port");

  return result;
}

gr_endpoint
gr_hier_block2_detail::resolve_endpoint(const gr_endpoint &endp, bool is_input)
{
  // Check if endpoint is a leaf node
  if (boost::dynamic_pointer_cast<gr_block, gr_basic_block>(endp.block()))
    return endp;
  
  // Check if endpoint is a hierarchical block
  gr_hier_block2_sptr hier_block2(boost::dynamic_pointer_cast<gr_hier_block2, gr_basic_block>(endp.block()));
  if (hier_block2) {
    if (GR_HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << "Resolving endpoint " << endp << " as an " 
		<< (is_input ? "input" : "output")
		<< ", recursing" << std::endl;
    return hier_block2->d_detail->resolve_port(endp.port(), is_input);
  }

  // Shouldn't ever get here
  throw std::runtime_error("block is not a valid gr_block or gr_hier_block2!");
}

void
gr_hier_block2_detail::flatten(gr_flat_flowgraph_sptr sfg)
{
  if (GR_HIER_BLOCK2_DETAIL_DEBUG)
    std::cout << "flattening " << d_owner->name() << std::endl;

  // Add my edges to the flow graph, resolving references to actual endpoints
  gr_edge_vector_t edges = d_fg->edges();

  for (gr_edge_viter_t p = edges.begin(); p != edges.end(); p++) {
    if (GR_HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << "Flattening edge " << (*p) << std::endl;

    gr_endpoint src_endp = resolve_endpoint(p->src(), false);
    gr_endpoint dst_endp = resolve_endpoint(p->dst(), true);
    sfg->connect(src_endp, dst_endp);
  }

  gr_basic_block_vector_t blocks = d_fg->calc_used_blocks();

  // Recurse hierarchical children
  for (gr_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
    gr_hier_block2_sptr hier_block2(boost::dynamic_pointer_cast<gr_hier_block2, gr_basic_block>(*p));
    if (hier_block2)
      hier_block2->d_detail->flatten(sfg);
  }
}

void
gr_hier_block2_detail::lock()
{
  if (GR_HIER_BLOCK2_DETAIL_DEBUG)
    std::cout << "lock: entered in " << this << std::endl;

  if (d_parent_detail)
    d_parent_detail->lock();
  else
    if (d_runtime)
      d_runtime->lock();
}

void
gr_hier_block2_detail::unlock()
{
  if (GR_HIER_BLOCK2_DETAIL_DEBUG)
    std::cout << "unlock: entered in " << this << std::endl;

  if (d_parent_detail)
    d_parent_detail->unlock();
  else
    if (d_runtime)
      d_runtime->unlock();
}

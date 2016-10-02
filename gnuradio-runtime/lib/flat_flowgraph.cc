/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#include "flat_flowgraph.h"
#include <gnuradio/block_detail.h>
#include <gnuradio/buffer.h>
#include <gnuradio/prefs.h>
#include <volk/volk.h>
#include <iostream>
#include <map>
#include <boost/format.hpp>

namespace gr {

#define FLAT_FLOWGRAPH_DEBUG  0

// 32Kbyte buffer size between blocks
#define GR_FIXED_BUFFER_SIZE (32*(1L<<10))

  static const unsigned int s_fixed_buffer_size = GR_FIXED_BUFFER_SIZE;

  flat_flowgraph_sptr
  make_flat_flowgraph()
  {
    return flat_flowgraph_sptr(new flat_flowgraph());
  }

  flat_flowgraph::flat_flowgraph()
  {
    configure_default_loggers(d_logger, d_debug_logger, "flat_flowgraph");
  }

  flat_flowgraph::~flat_flowgraph()
  {
  }

  void
  flat_flowgraph::setup_connections()
  {
    basic_block_vector_t blocks = calc_used_blocks();

    // Assign block details to blocks
    for(basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++)
      cast_to_block_sptr(*p)->set_detail(allocate_block_detail(*p));

    // Connect inputs to outputs for each block
    for(basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
      connect_block_inputs(*p);

      block_sptr block = cast_to_block_sptr(*p);
      block->set_unaligned(0);
      block->set_is_unaligned(false);
    }

    // Connect message ports connetions
    for(msg_edge_viter_t i = d_msg_edges.begin(); i != d_msg_edges.end(); i++) {
      if(FLAT_FLOWGRAPH_DEBUG)
        std::cout << boost::format("flat_fg connecting msg primitives: (%s, %s)->(%s, %s)\n") %
          i->src().block() % i->src().port() %
          i->dst().block() % i->dst().port();
      i->src().block()->message_port_sub(i->src().port(), pmt::cons(i->dst().block()->alias_pmt(), i->dst().port()));
    }
  }

  block_detail_sptr
  flat_flowgraph::allocate_block_detail(basic_block_sptr block)
  {
    int ninputs = calc_used_ports(block, true).size();
    int noutputs = calc_used_ports(block, false).size();
    block_detail_sptr detail = make_block_detail(ninputs, noutputs);

    block_sptr grblock = cast_to_block_sptr(block);
    if(!grblock)
      throw std::runtime_error(
        (boost::format("allocate_block_detail found non-gr::block (%s)")%
        block->alias()).str());

    if(FLAT_FLOWGRAPH_DEBUG)
      std::cout << "Creating block detail for " << block << std::endl;

    for(int i = 0; i < noutputs; i++) {
      grblock->expand_minmax_buffer(i);

      buffer_sptr buffer = allocate_buffer(block, i);
      if(FLAT_FLOWGRAPH_DEBUG)
        std::cout << "Allocated buffer for output " << block << ":" << i << std::endl;
      detail->set_output(i, buffer);

      // Update the block's max_output_buffer based on what was actually allocated.
      if((grblock->max_output_buffer(i) != buffer->bufsize()) && (grblock->max_output_buffer(i) != -1))
        GR_LOG_WARN(d_logger, boost::format("Block (%1%) max output buffer set to %2%"
                                            " instead of requested %3%") \
                    % grblock->alias() % buffer->bufsize() % grblock->max_output_buffer(i));
      grblock->set_max_output_buffer(i, buffer->bufsize());
    }

    return detail;
  }

  buffer_sptr
  flat_flowgraph::allocate_buffer(basic_block_sptr block, int port)
  {
    block_sptr grblock = cast_to_block_sptr(block);
    if(!grblock)
      throw std::runtime_error("allocate_buffer found non-gr::block");
    int item_size = block->output_signature()->sizeof_stream_item(port);

    // *2 because we're now only filling them 1/2 way in order to
    // increase the available parallelism when using the TPB scheduler.
    // (We're double buffering, where we used to single buffer)
    int nitems = s_fixed_buffer_size * 2 / item_size;

    // Make sure there are at least twice the output_multiple no. of items
    if(nitems < 2*grblock->output_multiple())	// Note: this means output_multiple()
      nitems = 2*grblock->output_multiple();	// can't be changed by block dynamically

    // If any downstream blocks are decimators and/or have a large output_multiple,
    // ensure we have a buffer at least twice their decimation factor*output_multiple
    basic_block_vector_t blocks = calc_downstream_blocks(block, port);

    // limit buffer size if indicated
    if(grblock->max_output_buffer(port) > 0) {
      //std::cout << "constraining output items to " << block->max_output_buffer(port) << "\n";
      nitems = std::min((long)nitems, (long)grblock->max_output_buffer(port));
      nitems -= nitems%grblock->output_multiple();
      if(nitems < 1)
        throw std::runtime_error("problems allocating a buffer with the given max output buffer constraint!");
    }
    else if(grblock->min_output_buffer(port) > 0) {
      nitems = std::max((long)nitems, (long)grblock->min_output_buffer(port));
      nitems -= nitems%grblock->output_multiple();
      if(nitems < 1)
        throw std::runtime_error("problems allocating a buffer with the given min output buffer constraint!");
    }

    for(basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
      block_sptr dgrblock = cast_to_block_sptr(*p);
      if(!dgrblock)
        throw std::runtime_error("allocate_buffer found non-gr::block");

      double decimation = (1.0/dgrblock->relative_rate());
      int multiple      = dgrblock->output_multiple();
      int history       = dgrblock->history();
      nitems = std::max(nitems, static_cast<int>(2*(decimation*multiple+history)));
    }

    //  std::cout << "make_buffer(" << nitems << ", " << item_size << ", " << grblock << "\n";
    // We're going to let this fail once and retry. If that fails,
    // throw and exit.
    buffer_sptr b;
    try {
      b = make_buffer(nitems, item_size, grblock);
    }
    catch(std::bad_alloc&) {
      b = make_buffer(nitems, item_size, grblock);
    }

    // Set the max noutput items size here to make sure it's always
    // set in the block and available in the start() method.
    // But don't overwrite if the user has set this externally.
    if(!grblock->is_set_max_noutput_items())
      grblock->set_max_noutput_items(nitems);

    return b;
  }

  void
  flat_flowgraph::connect_block_inputs(basic_block_sptr block)
  {
    block_sptr grblock = cast_to_block_sptr(block);
    if (!grblock)
      throw std::runtime_error("connect_block_inputs found non-gr::block");

    // Get its detail and edges that feed into it
    block_detail_sptr detail = grblock->detail();
    edge_vector_t in_edges = calc_upstream_edges(block);

    // For each edge that feeds into it
    for(edge_viter_t e = in_edges.begin(); e != in_edges.end(); e++) {
      // Set the buffer reader on the destination port to the output
      // buffer on the source port
      int dst_port = e->dst().port();
      int src_port = e->src().port();
      basic_block_sptr src_block = e->src().block();
      block_sptr src_grblock = cast_to_block_sptr(src_block);
      if(!src_grblock)
        throw std::runtime_error("connect_block_inputs found non-gr::block");
      buffer_sptr src_buffer = src_grblock->detail()->output(src_port);

      if(FLAT_FLOWGRAPH_DEBUG)
        std::cout << "Setting input " << dst_port << " from edge " << (*e) << std::endl;

      detail->set_input(dst_port, buffer_add_reader(src_buffer, grblock->history()-1, grblock,
                                                    grblock->sample_delay(src_port)));
    }
  }

  void
  flat_flowgraph::merge_connections(flat_flowgraph_sptr old_ffg)
  {
    // Allocate block details if needed.  Only new blocks that aren't pruned out
    // by flattening will need one; existing blocks still in the new flowgraph will
    // already have one.
    for(basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
      block_sptr block = cast_to_block_sptr(*p);

      if(!block->detail()) {
        if(FLAT_FLOWGRAPH_DEBUG)
          std::cout << "merge: allocating new detail for block " << (*p) << std::endl;
        block->set_detail(allocate_block_detail(block));
      }
      else {
        if(FLAT_FLOWGRAPH_DEBUG)
          std::cout << "merge: reusing original detail for block " << (*p) << std::endl;
      }
    }

    // Calculate the old edges that will be going away, and clear the
    // buffer readers on the RHS.
    for(edge_viter_t old_edge = old_ffg->d_edges.begin(); old_edge != old_ffg->d_edges.end(); old_edge++) {
      if(FLAT_FLOWGRAPH_DEBUG)
        std::cout << "merge: testing old edge " << (*old_edge) << "...";

      edge_viter_t new_edge;
      for(new_edge = d_edges.begin(); new_edge != d_edges.end(); new_edge++)
        if(new_edge->src() == old_edge->src() &&
           new_edge->dst() == old_edge->dst())
          break;

      if(new_edge == d_edges.end()) { // not found in new edge list
        if(FLAT_FLOWGRAPH_DEBUG)
          std::cout << "not in new edge list" << std::endl;
        // zero the buffer reader on RHS of old edge
        block_sptr block(cast_to_block_sptr(old_edge->dst().block()));
        int port = old_edge->dst().port();
        block->detail()->set_input(port, buffer_reader_sptr());
      }
      else {
        if (FLAT_FLOWGRAPH_DEBUG)
          std::cout << "found in new edge list" << std::endl;
      }
    }

    // Now connect inputs to outputs, reusing old buffer readers if they exist
    for(basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
      block_sptr block = cast_to_block_sptr(*p);

      if(FLAT_FLOWGRAPH_DEBUG)
        std::cout << "merge: merging " << (*p) << "...";

      if(old_ffg->has_block_p(*p)) {
        // Block exists in old flow graph
        if(FLAT_FLOWGRAPH_DEBUG)
          std::cout << "used in old flow graph" << std::endl;
        block_detail_sptr detail = block->detail();

        // Iterate through the inputs and see what needs to be done
        int ninputs = calc_used_ports(block, true).size(); // Might be different now
        for(int i = 0; i < ninputs; i++) {
          if(FLAT_FLOWGRAPH_DEBUG)
            std::cout << "Checking input " << block << ":" << i << "...";
          edge edge = calc_upstream_edge(*p, i);

          // Fish out old buffer reader and see if it matches correct buffer from edge list
          block_sptr src_block = cast_to_block_sptr(edge.src().block());
          block_detail_sptr src_detail = src_block->detail();
          buffer_sptr src_buffer = src_detail->output(edge.src().port());
          buffer_reader_sptr old_reader;
          if(i < detail->ninputs()) // Don't exceed what the original detail has
            old_reader = detail->input(i);

          // If there's a match, use it
          if(old_reader && (src_buffer == old_reader->buffer())) {
            if(FLAT_FLOWGRAPH_DEBUG)
              std::cout << "matched, reusing" << std::endl;
          }
          else {
            if(FLAT_FLOWGRAPH_DEBUG)
              std::cout << "needs a new reader" << std::endl;

            // Create new buffer reader and assign
            detail->set_input(i, buffer_add_reader(src_buffer, block->history()-1, block));
          }
        }
      }
      else {
        // Block is new, it just needs buffer readers at this point
        if(FLAT_FLOWGRAPH_DEBUG)
          std::cout << "new block" << std::endl;
        connect_block_inputs(block);

        // Make sure all buffers are aligned
        setup_buffer_alignment(block);
      }

      // Connect message ports connetions
      for(msg_edge_viter_t i = d_msg_edges.begin(); i != d_msg_edges.end(); i++) {
          if(FLAT_FLOWGRAPH_DEBUG)
              std::cout << boost::format("flat_fg connecting msg primitives: (%s, %s)->(%s, %s)\n") %
                  i->src().block() % i->src().port() %
                  i->dst().block() % i->dst().port();
          i->src().block()->message_port_sub(i->src().port(), pmt::cons(i->dst().block()->alias_pmt(), i->dst().port()));
      }

      // Now deal with the fact that the block details might have
      // changed numbers of inputs and outputs vs. in the old
      // flowgraph.

      block->detail()->reset_nitem_counters();
      block->detail()->clear_tags();
    }
  }

  void
  flat_flowgraph::setup_buffer_alignment(block_sptr block)
  {
    const int alignment = volk_get_alignment();
    for(int i = 0; i < block->detail()->ninputs(); i++) {
      void *r = (void*)block->detail()->input(i)->read_pointer();
      uintptr_t ri = (uintptr_t)r % alignment;
      //std::cerr << "reader: " << r << "  alignment: " << ri << std::endl;
      if(ri != 0) {
        size_t itemsize = block->detail()->input(i)->get_sizeof_item();
        block->detail()->input(i)->update_read_pointer((alignment-ri)/itemsize);
      }
      block->set_unaligned(0);
      block->set_is_unaligned(false);
    }

    for(int i = 0; i < block->detail()->noutputs(); i++) {
      void *w = (void*)block->detail()->output(i)->write_pointer();
      uintptr_t wi = (uintptr_t)w % alignment;
      //std::cerr << "writer: " << w << "  alignment: " << wi << std::endl;
      if(wi != 0) {
        size_t itemsize = block->detail()->output(i)->get_sizeof_item();
        block->detail()->output(i)->update_write_pointer((alignment-wi)/itemsize);
      }
      block->set_unaligned(0);
      block->set_is_unaligned(false);
    }
  }

  std::string
  flat_flowgraph::edge_list()
  {
    std::stringstream s;
    for(edge_viter_t e = d_edges.begin(); e != d_edges.end(); e++)
      s << (*e) << std::endl;
    return s.str();
  }

  std::string
  flat_flowgraph::msg_edge_list()
  {
    std::stringstream s;
    for(msg_edge_viter_t e = d_msg_edges.begin(); e != d_msg_edges.end(); e++)
      s << (*e) << std::endl;
    return s.str();
  }

  void flat_flowgraph::dump()
  {
    for(edge_viter_t e = d_edges.begin(); e != d_edges.end(); e++)
      std::cout << " edge: " << (*e) << std::endl;

    for(basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
      std::cout << " block: " << (*p) << std::endl;
      block_detail_sptr detail = cast_to_block_sptr(*p)->detail();
      std::cout << "  detail @" << detail << ":" << std::endl;

      int ni = detail->ninputs();
      int no = detail->noutputs();
      for(int i = 0; i < no; i++) {
        buffer_sptr buffer = detail->output(i);
        std::cout << "   output " << i << ": " << buffer << std::endl;
      }

      for(int i = 0; i < ni; i++) {
        buffer_reader_sptr reader = detail->input(i);
        std::cout << "   reader " <<  i << ": " << reader
                  << " reading from buffer=" << reader->buffer() << std::endl;
      }
    }
  }

  block_vector_t
  flat_flowgraph::make_block_vector(basic_block_vector_t &blocks)
  {
    block_vector_t result;
    for(basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
      result.push_back(cast_to_block_sptr(*p));
    }

    return result;
  }

  void
  flat_flowgraph::clear_endpoint(const msg_endpoint &e, bool is_src)
  {
    for(size_t i=0; i<d_msg_edges.size(); i++) {
      if(is_src) {
        if(d_msg_edges[i].src() == e) {
          d_msg_edges.erase(d_msg_edges.begin() + i);
          i--;
        }
      }
      else {
        if(d_msg_edges[i].dst() == e) {
          d_msg_edges.erase(d_msg_edges.begin() + i);
          i--;
        }
      }
    }
  }

  void
  flat_flowgraph::clear_hier()
  {
    if(FLAT_FLOWGRAPH_DEBUG)
      std::cout << "Clear_hier()" << std::endl;
    for(size_t i=0; i<d_msg_edges.size(); i++) {
      if(FLAT_FLOWGRAPH_DEBUG)
        std::cout << "edge: " << d_msg_edges[i].src() << "-->" << d_msg_edges[i].dst() << std::endl;
      if(d_msg_edges[i].src().is_hier() || d_msg_edges[i].dst().is_hier()){
        if(FLAT_FLOWGRAPH_DEBUG)
          std::cout << "is hier" << std::endl;
        d_msg_edges.erase(d_msg_edges.begin() + i);
        i--;
      }
    }
  }

  void
  flat_flowgraph::replace_endpoint(const msg_endpoint &e, const msg_endpoint &r, bool is_src)
  {
    size_t n_replr(0);
    if(FLAT_FLOWGRAPH_DEBUG)
      std::cout << boost::format("flat_flowgraph::replace_endpoint( %s, %s, %d )\n") % e.block()% r.block()% is_src;
    for(size_t i=0; i<d_msg_edges.size(); i++) {
      if(is_src) {
        if(d_msg_edges[i].src() == e) {
          if(FLAT_FLOWGRAPH_DEBUG)
            std::cout << boost::format("flat_flowgraph::replace_endpoint() flattening to ( %s, %s )\n") \
              % r% d_msg_edges[i].dst();
          d_msg_edges.push_back( msg_edge(r, d_msg_edges[i].dst() ) );
          n_replr++;
        }
      }
      else {
        if(d_msg_edges[i].dst() == e) {
          if(FLAT_FLOWGRAPH_DEBUG)
            std::cout << boost::format("flat_flowgraph::replace_endpoint() flattening to ( %s, %s )\n") \
              % r% d_msg_edges[i].src();
          d_msg_edges.push_back( msg_edge(d_msg_edges[i].src(), r ) );
          n_replr++;
        }
      }
    }
  }

  void
  flat_flowgraph::enable_pc_rpc()
  {
#ifdef GR_PERFORMANCE_COUNTERS
    if(prefs::singleton()->get_bool("PerfCounters", "on", false)) {
      basic_block_viter_t p;
      for(p = d_blocks.begin(); p != d_blocks.end(); p++) {
        block_sptr block = cast_to_block_sptr(*p);
        if(!block->is_pc_rpc_set())
          block->setup_pc_rpc();
      }
    }
#endif /* GR_PERFORMANCE_COUNTERS */
  }

} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2009,2013 Free Software Foundation, Inc.
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

#include "hier_block2_detail.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <stdexcept>
#include <sstream>
#include <boost/format.hpp>

namespace gr {

#define HIER_BLOCK2_DETAIL_DEBUG 0

  hier_block2_detail::hier_block2_detail(hier_block2 *owner)
    : d_owner(owner),
      d_parent_detail(0),
      d_fg(make_flowgraph())
  {
    int min_inputs = owner->input_signature()->min_streams();
    int max_inputs = owner->input_signature()->max_streams();
    int min_outputs = owner->output_signature()->min_streams();
    int max_outputs = owner->output_signature()->max_streams();

    if(max_inputs == io_signature::IO_INFINITE ||
       max_outputs == io_signature::IO_INFINITE ||
       (min_inputs != max_inputs) ||(min_outputs != max_outputs) ) {
      std::stringstream msg;
      msg << "Hierarchical blocks do not yet support arbitrary or"
          << " variable numbers of inputs or outputs (" << d_owner->name() << ")";
      throw std::runtime_error(msg.str());
    }

    d_inputs = std::vector<endpoint_vector_t>(max_inputs);
    d_outputs = endpoint_vector_t(max_outputs);
  }

  hier_block2_detail::~hier_block2_detail()
  {
    d_owner = 0; // Don't use delete, we didn't allocate
  }

  void
  hier_block2_detail::connect(basic_block_sptr block)
  {
    std::stringstream msg;

    // Check if duplicate
    if(std::find(d_blocks.begin(), d_blocks.end(), block) != d_blocks.end()) {
      msg << "Block " << block << " already connected.";
      throw std::invalid_argument(msg.str());
    }

    // Check if has inputs or outputs
    if(block->input_signature()->max_streams() != 0 ||
       block->output_signature()->max_streams() != 0) {
      msg << "Block " << block << " must not have any input or output ports";
      throw std::invalid_argument(msg.str());
    }

    hier_block2_sptr hblock(cast_to_hier_block2_sptr(block));

    if(hblock && hblock.get() != d_owner) {
      if(HIER_BLOCK2_DETAIL_DEBUG)
        std::cout << "connect: block is hierarchical, setting parent to " << this << std::endl;
      hblock->d_detail->d_parent_detail = this;
    }

    d_blocks.push_back(block);
  }

  void
  hier_block2_detail::connect(basic_block_sptr src, int src_port,
                              basic_block_sptr dst, int dst_port)
  {
    std::stringstream msg;

    if(HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << "connecting: " << endpoint(src, src_port)
                << " -> " << endpoint(dst, dst_port) << std::endl;

    if(src.get() == dst.get())
      throw std::invalid_argument("connect: src and destination blocks cannot be the same");

    hier_block2_sptr src_block(cast_to_hier_block2_sptr(src));
    hier_block2_sptr dst_block(cast_to_hier_block2_sptr(dst));

    if(src_block && src.get() != d_owner) {
      if(HIER_BLOCK2_DETAIL_DEBUG)
        std::cout << "connect: src is hierarchical, setting parent to " << this << std::endl;
      src_block->d_detail->d_parent_detail = this;
    }

    if(dst_block && dst.get() != d_owner) {
      if(HIER_BLOCK2_DETAIL_DEBUG)
        std::cout << "connect: dst is hierarchical, setting parent to " << this << std::endl;
      dst_block->d_detail->d_parent_detail = this;
    }

    // Connections to block inputs or outputs
    int max_port;
    if(src.get() == d_owner) {
      max_port = src->input_signature()->max_streams();
      if((max_port != -1 && (src_port >= max_port)) || src_port < 0) {
        msg << "source port " << src_port << " out of range for " << src;
        throw std::invalid_argument(msg.str());
      }

      return connect_input(src_port, dst_port, dst);
    }

    if(dst.get() == d_owner) {
      max_port = dst->output_signature()->max_streams();
      if((max_port != -1 && (dst_port >= max_port)) || dst_port < 0) {
        msg << "destination port " << dst_port << " out of range for " << dst;
        throw std::invalid_argument(msg.str());
      }

      return connect_output(dst_port, src_port, src);
    }

    // Internal connections
    d_fg->connect(src, src_port, dst, dst_port);

    // TODO: connects to NC
  }

  void
  hier_block2_detail::msg_connect(basic_block_sptr src, pmt::pmt_t srcport,
                                  basic_block_sptr dst, pmt::pmt_t dstport)
  {
    if(HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << "connecting message port..." << std::endl;
    
    // register the subscription
    // this is done later...
    //  src->message_port_sub(srcport, pmt::cons(dst->alias_pmt(), dstport));

    // add block uniquely to list to internal blocks
    if(std::find(d_blocks.begin(), d_blocks.end(), dst) == d_blocks.end()){
      d_blocks.push_back(src);
      d_blocks.push_back(dst);
    }

    bool hier_out = (d_owner == src.get()) && src->message_port_is_hier_out(srcport);;
    bool hier_in = (d_owner == dst.get()) && dst->message_port_is_hier_in(dstport);

    hier_block2_sptr src_block(cast_to_hier_block2_sptr(src));
    hier_block2_sptr dst_block(cast_to_hier_block2_sptr(dst));

    if(src_block && src.get() != d_owner) {
      if(HIER_BLOCK2_DETAIL_DEBUG)
        std::cout << "connect: src is hierarchical, setting parent to " << this << std::endl;
      src_block->d_detail->d_parent_detail = this;
    }

    if(dst_block && dst.get() != d_owner) {
      if(HIER_BLOCK2_DETAIL_DEBUG)
        std::cout << "connect: dst is hierarchical, setting parent to " << this << std::endl;
      dst_block->d_detail->d_parent_detail = this;
    }

    // add edge for this message connection
    if(HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << boost::format("connect( (%s, %s, %d), (%s, %s, %d) )\n") % \
        src % srcport % hier_out %
        dst % dstport % hier_in;
    d_fg->connect(msg_endpoint(src, srcport, hier_out), msg_endpoint(dst, dstport, hier_in));
  }

  void
  hier_block2_detail::msg_disconnect(basic_block_sptr src, pmt::pmt_t srcport,
                                     basic_block_sptr dst, pmt::pmt_t dstport)
  {
    if(HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << "disconnecting message port..." << std::endl;
    
    // unregister the subscription - if already subscribed
    src->message_port_unsub(srcport, pmt::cons(dst->alias_pmt(), dstport));

    // remove edge for this message connection
    bool hier_out = (d_owner == src.get()) && src->message_port_is_hier_out(srcport);;
    bool hier_in = (d_owner == dst.get()) && dst->message_port_is_hier_in(dstport);
    d_fg->disconnect(msg_endpoint(src, srcport, hier_out), msg_endpoint(dst, dstport, hier_in));
  }

  void
  hier_block2_detail::disconnect(basic_block_sptr block)
  {
    // Check on singleton list
    for(basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
      if(*p == block) {
        d_blocks.erase(p);

        hier_block2_sptr hblock(cast_to_hier_block2_sptr(block));
        if(block && block.get() != d_owner) {
          if(HIER_BLOCK2_DETAIL_DEBUG)
            std::cout << "disconnect: block is hierarchical, clearing parent" << std::endl;
          hblock->d_detail->d_parent_detail = 0;
        }

        return;
      }
    }

    // Otherwise find all edges containing block
    edge_vector_t edges, tmp = d_fg->edges();
    edge_vector_t::iterator p;
    for(p = tmp.begin(); p != tmp.end(); p++) {
      if((*p).src().block() == block || (*p).dst().block() == block) {
        edges.push_back(*p);

        if(HIER_BLOCK2_DETAIL_DEBUG)
          std::cout << "disconnect: block found in edge " << (*p) << std::endl;
      }
    }

    if(edges.size() == 0) {
      std::stringstream msg;
      msg << "cannot disconnect block " << block << ", not found";
      throw std::invalid_argument(msg.str());
    }

    for(p = edges.begin(); p != edges.end(); p++) {
      disconnect((*p).src().block(), (*p).src().port(),
                 (*p).dst().block(), (*p).dst().port());
    }
  }

  void
  hier_block2_detail::disconnect(basic_block_sptr src, int src_port,
                                 basic_block_sptr dst, int dst_port)
  {
    if(HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << "disconnecting: " << endpoint(src, src_port)
                << " -> " << endpoint(dst, dst_port) << std::endl;

    if(src.get() == dst.get())
      throw std::invalid_argument("disconnect: source and destination blocks cannot be the same");

    hier_block2_sptr src_block(cast_to_hier_block2_sptr(src));
    hier_block2_sptr dst_block(cast_to_hier_block2_sptr(dst));

    if(src_block && src.get() != d_owner) {
      if(HIER_BLOCK2_DETAIL_DEBUG)
        std::cout << "disconnect: src is hierarchical, clearing parent" << std::endl;
      src_block->d_detail->d_parent_detail = 0;
    }

    if(dst_block && dst.get() != d_owner) {
      if(HIER_BLOCK2_DETAIL_DEBUG)
        std::cout << "disconnect: dst is hierarchical, clearing parent" << std::endl;
      dst_block->d_detail->d_parent_detail = 0;
    }

    if(src.get() == d_owner)
      return disconnect_input(src_port, dst_port, dst);

    if(dst.get() == d_owner)
      return disconnect_output(dst_port, src_port, src);

    // Internal connections
    d_fg->disconnect(src, src_port, dst, dst_port);
  }

  void
  hier_block2_detail::connect_input(int my_port, int port,
                                    basic_block_sptr block)
  {
    std::stringstream msg;

    if(my_port < 0 || my_port >= (signed)d_inputs.size()) {
      msg << "input port " << my_port << " out of range for " << block;
      throw std::invalid_argument(msg.str());
    }

    endpoint_vector_t &endps = d_inputs[my_port];
    endpoint endp(block, port);

    endpoint_viter_t p = std::find(endps.begin(), endps.end(), endp);
    if(p != endps.end()) {
      msg << "external input port " << my_port << " already wired to " << endp;
      throw std::invalid_argument(msg.str());
    }

    endps.push_back(endp);
  }

  void
  hier_block2_detail::connect_output(int my_port, int port,
                                     basic_block_sptr block)
  {
    std::stringstream msg;

    if(my_port < 0 || my_port >= (signed)d_outputs.size()) {
      msg << "output port " << my_port << " out of range for " << block;
      throw std::invalid_argument(msg.str());
    }

    if(d_outputs[my_port].block()) {
      msg << "external output port " << my_port << " already connected from "
          << d_outputs[my_port];
      throw std::invalid_argument(msg.str());
    }

    d_outputs[my_port] = endpoint(block, port);
  }

  void
  hier_block2_detail::disconnect_input(int my_port, int port,
                                       basic_block_sptr block)
  {
    std::stringstream msg;

    if(my_port < 0 || my_port >= (signed)d_inputs.size()) {
      msg << "input port number " << my_port << " out of range for " << block;
      throw std::invalid_argument(msg.str());
    }

    endpoint_vector_t &endps = d_inputs[my_port];
    endpoint endp(block, port);

    endpoint_viter_t p = std::find(endps.begin(), endps.end(), endp);
    if(p == endps.end()) {
      msg << "external input port " << my_port << " not connected to " << endp;
      throw std::invalid_argument(msg.str());
    }

    endps.erase(p);
  }

  void
  hier_block2_detail::disconnect_output(int my_port, int port,
                                        basic_block_sptr block)
  {
    std::stringstream msg;

    if(my_port < 0 || my_port >= (signed)d_outputs.size()) {
      msg << "output port number " << my_port << " out of range for " << block;
      throw std::invalid_argument(msg.str());
    }

    if(d_outputs[my_port].block() != block) {
      msg << "block " << block << " not assigned to output "
          << my_port << ", can't disconnect";
      throw std::invalid_argument(msg.str());
    }

    d_outputs[my_port] = endpoint();
  }

  endpoint_vector_t
  hier_block2_detail::resolve_port(int port, bool is_input)
  {
    std::stringstream msg;

    if(HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << "Resolving port " << port << " as an "
                << (is_input ? "input" : "output")
                << " of " << d_owner->name() << std::endl;

    endpoint_vector_t result;

    if(is_input) {
      if(port < 0 || port >= (signed)d_inputs.size()) {
        msg << "resolve_port: hierarchical block '" << d_owner->name()
            << "': input " << port << " is out of range";
        throw std::runtime_error(msg.str());
      }

      if(d_inputs[port].empty()) {
        msg << "resolve_port: hierarchical block '" << d_owner->name()
            << "': input " << port << " is not connected internally";
        throw std::runtime_error(msg.str());
      }

      endpoint_vector_t &endps = d_inputs[port];
      endpoint_viter_t p;
      for(p = endps.begin(); p != endps.end(); p++) {
        endpoint_vector_t tmp = resolve_endpoint(*p, true);
        std::copy(tmp.begin(), tmp.end(), back_inserter(result));
      }
    }
    else {
      if(port < 0 || port >= (signed)d_outputs.size()) {
        msg << "resolve_port: hierarchical block '" << d_owner->name()
            << "': output " << port << " is out of range";
        throw std::runtime_error(msg.str());
      }

      if(d_outputs[port] == endpoint()) {
        msg << "resolve_port: hierarchical block '" << d_owner->name()
            << "': output " << port << " is not connected internally";
        throw std::runtime_error(msg.str());
      }

      result = resolve_endpoint(d_outputs[port], false);
    }

    if(result.empty()) {
      msg << "resolve_port: hierarchical block '" << d_owner->name()
          << "': unable to resolve "
          << (is_input ? "input port " : "output port ")
          << port;
      throw std::runtime_error(msg.str());
    }

    return result;
  }

  void
  hier_block2_detail::disconnect_all()
  {
    d_fg->clear();
    d_blocks.clear();

    int max_inputs = d_owner->input_signature()->max_streams();
    int max_outputs = d_owner->output_signature()->max_streams();
    d_inputs = std::vector<endpoint_vector_t>(max_inputs);
    d_outputs = endpoint_vector_t(max_outputs);
  }

  endpoint_vector_t
  hier_block2_detail::resolve_endpoint(const endpoint &endp, bool is_input) const
  {
    std::stringstream msg;
    endpoint_vector_t result;

    // Check if endpoint is a leaf node
    if(cast_to_block_sptr(endp.block())) {
      if(HIER_BLOCK2_DETAIL_DEBUG)
        std::cout << "Block " << endp.block() << " is a leaf node, returning." << std::endl;
      result.push_back(endp);
      return result;
    }

    // Check if endpoint is a hierarchical block
    hier_block2_sptr hier_block2(cast_to_hier_block2_sptr(endp.block()));
    if(hier_block2) {
      if(HIER_BLOCK2_DETAIL_DEBUG)
        std::cout << "Resolving endpoint " << endp << " as an "
                  << (is_input ? "input" : "output")
                  << ", recursing" << std::endl;
      return hier_block2->d_detail->resolve_port(endp.port(), is_input);
    }

    msg << "unable to resolve" << (is_input ? " input " : " output ")
        << "endpoint " << endp;
    throw std::runtime_error(msg.str());
  }

  void
  hier_block2_detail::flatten_aux(flat_flowgraph_sptr sfg) const
  {
    if(HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << " ** Flattening " << d_owner->name() << std::endl;

    // Add my edges to the flow graph, resolving references to actual endpoints
    edge_vector_t edges = d_fg->edges();
    msg_edge_vector_t msg_edges = d_fg->msg_edges();
    edge_viter_t p;
    msg_edge_viter_t q,u;

    // Only run setup_rpc if ControlPort config param is enabled.
    bool ctrlport_on = prefs::singleton()->get_bool("ControlPort", "on", false);

    // For every block (gr::block and gr::hier_block2), set up the RPC
    // interface.
    for(p = edges.begin(); p != edges.end(); p++) {
      basic_block_sptr b;
      b = p->src().block();

      if(ctrlport_on) {
        if(!b->is_rpc_set()) {
          b->setup_rpc();
          b->rpc_set();
        }
      }

      b = p->dst().block();
      if(ctrlport_on) {
        if(!b->is_rpc_set()) {
          b->setup_rpc();
          b->rpc_set();
        }
      }
    }

    if(HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << "Flattening stream connections: " << std::endl;

    for(p = edges.begin(); p != edges.end(); p++) {
      if(HIER_BLOCK2_DETAIL_DEBUG)
        std::cout << "Flattening edge " << (*p) << std::endl;

      endpoint_vector_t src_endps = resolve_endpoint(p->src(), false);
      endpoint_vector_t dst_endps = resolve_endpoint(p->dst(), true);

      endpoint_viter_t s, d;
      for(s = src_endps.begin(); s != src_endps.end(); s++) {
        for(d = dst_endps.begin(); d != dst_endps.end(); d++) {
          if(HIER_BLOCK2_DETAIL_DEBUG)
            std::cout << (*s) << "->" << (*d) << std::endl;
          sfg->connect(*s, *d);
        }
      }
    }

    // loop through flattening hierarchical connections
    if(HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << "Flattening msg connections: " << std::endl;

    std::vector<std::pair<msg_endpoint, bool> > resolved_endpoints;
    for(q = msg_edges.begin(); q != msg_edges.end(); q++) {
      if(HIER_BLOCK2_DETAIL_DEBUG)
        std::cout << boost::format(" flattening edge ( %s, %s, %d) -> ( %s, %s, %d)\n") % \
          q->src().block() % q->src().port() % q->src().is_hier() % q->dst().block() % \
          q->dst().port() % q->dst().is_hier();

      bool normal_connection = true;

      // resolve existing connections to hier ports
      if(q->dst().is_hier()) {
        if(HIER_BLOCK2_DETAIL_DEBUG)
          std::cout << boost::format("  resolve hier output (%s, %s)") % \
            q->dst().block() % q->dst().port() << std::endl;
        sfg->replace_endpoint( q->dst(), q->src(), true );
        resolved_endpoints.push_back(std::pair<msg_endpoint, bool>(q->dst(),true));
        normal_connection = false;
      }

      if(q->src().is_hier()) {
        if(HIER_BLOCK2_DETAIL_DEBUG)
          std::cout << boost::format("  resolve hier input (%s, %s)") % \
            q->src().block() % q->src().port() << std::endl;
        sfg->replace_endpoint( q->src(), q->dst(), false );
        resolved_endpoints.push_back(std::pair<msg_endpoint, bool>(q->src(),false));
        normal_connection = false;
      }

      // propogate non hier connections through
      if(normal_connection){
        sfg->connect( q->src(), q->dst() );
      } 
    }

    for(std::vector<std::pair<msg_endpoint, bool> >::iterator it = resolved_endpoints.begin();
        it != resolved_endpoints.end(); it++) {
      sfg->clear_endpoint((*it).first, (*it).second);
    }

    /*
    // connect primitive edges in the new fg
    for(q = msg_edges.begin(); q != msg_edges.end(); q++) {
      if((!q->src().is_hier()) && (!q->dst().is_hier())) {
        sfg->connect(q->src(), q->dst());
      }
      else {
        std::cout << "not connecting hier connection!" << std::endl;
      }
    }
    */
    
    // Construct unique list of blocks used either in edges, inputs,
    // outputs, or by themselves.  I still hate STL.
    basic_block_vector_t blocks; // unique list of used blocks
    basic_block_vector_t tmp = d_fg->calc_used_blocks();

    // First add the list of singleton blocks
    std::vector<basic_block_sptr>::const_iterator b;   // Because flatten_aux is const
    for(b = d_blocks.begin(); b != d_blocks.end(); b++)
      tmp.push_back(*b);

    // Now add the list of connected input blocks
    std::stringstream msg;
    for(unsigned int i = 0; i < d_inputs.size(); i++) {
      if(d_inputs[i].size() == 0) {
        msg << "In hierarchical block " << d_owner->name() << ", input " << i
            << " is not connected internally";
        throw std::runtime_error(msg.str());
      }

      for(unsigned int j = 0; j < d_inputs[i].size(); j++)
        tmp.push_back(d_inputs[i][j].block());
    }

    for(unsigned int i = 0; i < d_outputs.size(); i++) {
      basic_block_sptr blk = d_outputs[i].block();
      if(!blk) {
        msg << "In hierarchical block " << d_owner->name() << ", output " << i
            << " is not connected internally";
        throw std::runtime_error(msg.str());
      }
      tmp.push_back(blk);
    }
    sort(tmp.begin(), tmp.end());

    std::insert_iterator<basic_block_vector_t> inserter(blocks, blocks.begin());
    unique_copy(tmp.begin(), tmp.end(), inserter);

    // Recurse hierarchical children
    for(basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
      hier_block2_sptr hier_block2(cast_to_hier_block2_sptr(*p));
      if(hier_block2 && (hier_block2.get() != d_owner)) {
        if(HIER_BLOCK2_DETAIL_DEBUG)
          std::cout << "flatten_aux: recursing into hierarchical block "
                    << hier_block2 << std::endl;
        hier_block2->d_detail->flatten_aux(sfg);
      }
    }
  }

  void
  hier_block2_detail::lock()
  {
    if(HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << "lock: entered in " << this << std::endl;

    if(d_parent_detail)
      d_parent_detail->lock();
    else
      d_owner->lock();
  }

  void
  hier_block2_detail::unlock()
  {
    if(HIER_BLOCK2_DETAIL_DEBUG)
      std::cout << "unlock: entered in " << this << std::endl;

    if(d_parent_detail)
      d_parent_detail->unlock();
    else
      d_owner->unlock();
  }

  void
  hier_block2_detail::set_processor_affinity(const std::vector<int> &mask)
  {
    basic_block_vector_t tmp = d_fg->calc_used_blocks();
    for(basic_block_viter_t p = tmp.begin(); p != tmp.end(); p++) {
      (*p)->set_processor_affinity(mask);
    }
  }

  void
  hier_block2_detail::unset_processor_affinity()
  {
    basic_block_vector_t tmp = d_fg->calc_used_blocks();
    for(basic_block_viter_t p = tmp.begin(); p != tmp.end(); p++) {
      (*p)->unset_processor_affinity();
    }
  }

  std::vector<int>
  hier_block2_detail::processor_affinity()
  {
    basic_block_vector_t tmp = d_fg->calc_used_blocks();
    return tmp[0]->processor_affinity();
  }

} /* namespace gr */

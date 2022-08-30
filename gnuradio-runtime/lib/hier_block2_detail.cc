/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "hier_block2_detail.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <gnuradio/top_block.h>
#include <sstream>
#include <stdexcept>

// TODO: Replace with GNU Radio logging
#include <iostream>

namespace gr {

hier_block2_detail::hier_block2_detail(hier_block2* owner)
    : d_owner(owner), d_parent(), d_parent_refcnt(0), d_fg(make_flowgraph())
{
    int min_inputs = owner->input_signature()->min_streams();
    int max_inputs = owner->input_signature()->max_streams();
    int min_outputs = owner->output_signature()->min_streams();
    int max_outputs = owner->output_signature()->max_streams();

    if (max_inputs == io_signature::IO_INFINITE ||
        max_outputs == io_signature::IO_INFINITE || (min_inputs != max_inputs) ||
        (min_outputs != max_outputs)) {
        std::stringstream msg;
        msg << "Hierarchical blocks do not yet support arbitrary or"
            << " variable numbers of inputs or outputs (" << d_owner->name() << ")";
        throw std::runtime_error(msg.str());
    }

    d_inputs = std::vector<endpoint_vector_t>(max_inputs);
    d_outputs = endpoint_vector_t(max_outputs);

    d_max_output_buffer = std::vector<size_t>(std::max(max_outputs, 1), 0);
    d_min_output_buffer = std::vector<size_t>(std::max(max_outputs, 1), 0);

    configure_default_loggers(d_logger, d_debug_logger, "hier_block2_detail");
}

hier_block2_detail::~hier_block2_detail()
{
    d_owner = 0; // Don't use delete, we didn't allocate
}

void hier_block2_detail::connect(basic_block_sptr block)
{
    std::stringstream msg;

    // Check if duplicate
    if (std::find(d_blocks.begin(), d_blocks.end(), block) != d_blocks.end()) {
        msg << "Block " << block << " already connected.";
        throw std::invalid_argument(msg.str());
    }

    // Check if has inputs or outputs
    if (block->input_signature()->max_streams() != 0 ||
        block->output_signature()->max_streams() != 0) {
        msg << "Block " << block << " must not have any input or output ports";
        throw std::invalid_argument(msg.str());
    }

    hier_block2_sptr hblock(cast_to_hier_block2_sptr(block));

    if (hblock && hblock.get() != d_owner) {
        d_debug_logger->debug("connect: block is hierarchical, setting parent to {:p}",
                              (void*)this);
        hblock->d_detail->set_parent(this->d_owner);
    }

    d_blocks.push_back(block);
}

void hier_block2_detail::connect(basic_block_sptr src,
                                 int src_port,
                                 basic_block_sptr dst,
                                 int dst_port)
{
    std::stringstream msg;

    d_debug_logger->debug("connecting: {} -> {}",
                          endpoint(src, src_port).identifier(),
                          endpoint(dst, dst_port).identifier());

    if (src.get() == dst.get())
        throw std::invalid_argument(
            "connect: src and destination blocks cannot be the same");

    hier_block2_sptr src_block(cast_to_hier_block2_sptr(src));
    hier_block2_sptr dst_block(cast_to_hier_block2_sptr(dst));

    if (src_block && src.get() != d_owner) {
        d_debug_logger->debug("connect: src is hierarchical, setting parent to {:p}",
                              (void*)this);
        src_block->d_detail->set_parent(this->d_owner);
    }

    if (dst_block && dst.get() != d_owner) {
        d_debug_logger->debug("connect: dst is hierarchical, setting parent to {:p}",
                              (void*)this);
        dst_block->d_detail->set_parent(this->d_owner);
    }

    // Connections to block inputs or outputs
    int max_port;
    if (src.get() == d_owner) {
        max_port = src->input_signature()->max_streams();
        if ((max_port != -1 && (src_port >= max_port)) || src_port < 0) {
            msg << "source port " << src_port << " out of range for " << src;
            throw std::invalid_argument(msg.str());
        }

        return connect_input(src_port, dst_port, dst);
    }

    if (dst.get() == d_owner) {
        max_port = dst->output_signature()->max_streams();
        if ((max_port != -1 && (dst_port >= max_port)) || dst_port < 0) {
            msg << "destination port " << dst_port << " out of range for " << dst;
            throw std::invalid_argument(msg.str());
        }

        return connect_output(dst_port, src_port, src);
    }

    // Internal connections
    d_fg->connect(src, src_port, dst, dst_port);

    // TODO: connects to NC
}

void hier_block2_detail::msg_connect(basic_block_sptr src,
                                     pmt::pmt_t srcport,
                                     basic_block_sptr dst,
                                     pmt::pmt_t dstport)
{
    d_debug_logger->debug("connecting message port...");

    // add block uniquely to list to internal blocks
    if (std::find(d_blocks.begin(), d_blocks.end(), dst) == d_blocks.end()) {
        d_blocks.push_back(src);
        d_blocks.push_back(dst);
    }


    bool hier_in = false, hier_out = false;
    if (d_owner == src.get()) {
        hier_out = src->message_port_is_hier_in(srcport);
    } else if (d_owner == dst.get()) {
        hier_in = dst->message_port_is_hier_out(dstport);
        ;
    } else {
        hier_out = src->message_port_is_hier_out(srcport);
        hier_in = dst->message_port_is_hier_in(dstport);
    }

    hier_block2_sptr src_block(cast_to_hier_block2_sptr(src));
    hier_block2_sptr dst_block(cast_to_hier_block2_sptr(dst));

    if (src_block && src.get() != d_owner) {
        d_debug_logger->debug("msg_connect: src is hierarchical, setting parent to {:p}",
                              (void*)this);
        src_block->d_detail->set_parent(this->d_owner);
    }

    if (dst_block && dst.get() != d_owner) {
        d_debug_logger->debug("msg_connect: dst is hierarchical, setting parent to {:p}",
                              (void*)this);
        dst_block->d_detail->set_parent(this->d_owner);
    }

    // add edge for this message connection
    d_debug_logger->debug("msg_connect( ({}, {}, {:d}), ({}, {}, {:d}) )",
                          src->identifier(),
                          pmt::write_string(srcport),
                          hier_out,
                          dst->identifier(),
                          pmt::write_string(dstport),
                          hier_in);
    d_fg->connect(msg_endpoint(src, srcport, hier_out),
                  msg_endpoint(dst, dstport, hier_in));
}

void hier_block2_detail::msg_disconnect(basic_block_sptr src,
                                        pmt::pmt_t srcport,
                                        basic_block_sptr dst,
                                        pmt::pmt_t dstport)
{
    d_debug_logger->debug("disconnecting message port...");

    // remove edge for this message connection
    bool hier_in = false, hier_out = false;
    if (d_owner == src.get()) {
        hier_out = src->message_port_is_hier_in(srcport);
    } else if (d_owner == dst.get()) {
        hier_in = dst->message_port_is_hier_out(dstport);
        ;
    } else {
        hier_out = src->message_port_is_hier_out(srcport);
        hier_in = dst->message_port_is_hier_in(dstport);
    }

    d_fg->disconnect(msg_endpoint(src, srcport, hier_out),
                     msg_endpoint(dst, dstport, hier_in));

    hier_block2_sptr src_block(cast_to_hier_block2_sptr(src));
    hier_block2_sptr dst_block(cast_to_hier_block2_sptr(dst));

    if (src_block && src.get() != d_owner) {
        // if the source is hier, we need to resolve the endpoint before calling unsub
        msg_edge_vector_t edges = src_block->d_detail->d_fg->msg_edges();
        for (msg_edge_viter_t it = edges.begin(); it != edges.end(); ++it) {
            if ((*it).dst().block() == src) {
                src = (*it).src().block();
                srcport = (*it).src().port();
            }
        }
        src_block->d_detail->reset_parent();
    }

    if (dst_block && dst.get() != d_owner) {
        // if the destination is hier, we need to resolve the endpoint before calling
        // unsub
        msg_edge_vector_t edges = dst_block->d_detail->d_fg->msg_edges();
        for (msg_edge_viter_t it = edges.begin(); it != edges.end(); ++it) {
            if ((*it).src().block() == dst) {
                dst = (*it).dst().block();
                dstport = (*it).dst().port();
            }
        }
        dst_block->d_detail->reset_parent();
    }

    // unregister the subscription - if already subscribed
    src->message_port_unsub(srcport, pmt::cons(dst->alias_pmt(), dstport));
}

void hier_block2_detail::disconnect(basic_block_sptr block)
{
    // Check on singleton list
    for (basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
        if (*p == block) {
            d_blocks.erase(p);

            hier_block2_sptr hblock(cast_to_hier_block2_sptr(block));
            if (hblock && hblock.get() != d_owner) {
                d_debug_logger->debug(
                    "disconnect: block is hierarchical, clearing parent");
                hblock->d_detail->reset_parent();
            }

            return;
        }
    }

    // Otherwise find all edges containing block
    edge_vector_t edges, tmp = d_fg->edges();
    edge_vector_t::iterator p;
    for (p = tmp.begin(); p != tmp.end(); p++) {
        if ((*p).src().block() == block || (*p).dst().block() == block) {
            edges.push_back(*p);

            d_debug_logger->debug("disconnect: block found in edge {}",
                                  (*p).identifier());
        }
    }

    if (edges.empty()) {
        std::stringstream msg;
        msg << "cannot disconnect block " << block << ", not found";
        throw std::invalid_argument(msg.str());
    }

    for (p = edges.begin(); p != edges.end(); p++) {
        disconnect(
            (*p).src().block(), (*p).src().port(), (*p).dst().block(), (*p).dst().port());
    }
}

void hier_block2_detail::disconnect(basic_block_sptr src,
                                    int src_port,
                                    basic_block_sptr dst,
                                    int dst_port)
{
    d_debug_logger->debug("disconnecting: {} -> {}",
                          endpoint(src, src_port).identifier(),
                          endpoint(dst, dst_port).identifier());

    if (src.get() == dst.get())
        throw std::invalid_argument(
            "disconnect: source and destination blocks cannot be the same");

    hier_block2_sptr src_block(cast_to_hier_block2_sptr(src));
    hier_block2_sptr dst_block(cast_to_hier_block2_sptr(dst));

    if (src_block && src.get() != d_owner) {
        d_debug_logger->debug("disconnect: src is hierarchical, clearing parent");
        src_block->d_detail->reset_parent();
    }

    if (dst_block && dst.get() != d_owner) {
        d_debug_logger->debug("disconnect: dst is hierarchical, clearing parent");
        dst_block->d_detail->reset_parent();
    }

    if (src.get() == d_owner)
        return disconnect_input(src_port, dst_port, dst);

    if (dst.get() == d_owner)
        return disconnect_output(dst_port, src_port, src);

    // Internal connections
    d_fg->disconnect(src, src_port, dst, dst_port);
}

void hier_block2_detail::refresh_io_signature()
{
    int min_inputs = d_owner->input_signature()->min_streams();
    int max_inputs = d_owner->input_signature()->max_streams();
    int min_outputs = d_owner->output_signature()->min_streams();
    int max_outputs = d_owner->output_signature()->max_streams();

    if (max_inputs == io_signature::IO_INFINITE ||
        max_outputs == io_signature::IO_INFINITE || (min_inputs != max_inputs) ||
        (min_outputs != max_outputs)) {
        std::stringstream msg;
        msg << "Hierarchical blocks do not yet support arbitrary or"
            << " variable numbers of inputs or outputs (" << d_owner->name() << ")";
        throw std::runtime_error(msg.str());
    }

    // Check for # input change
    if ((signed)d_inputs.size() != max_inputs) {
        d_inputs.resize(max_inputs);
    }

    // Check for # output change
    if ((signed)d_outputs.size() != max_outputs) {
        d_outputs.resize(max_outputs);
        d_min_output_buffer.resize(max_outputs, 0);
        d_max_output_buffer.resize(max_outputs, 0);
    }
}

void hier_block2_detail::connect_input(int my_port, int port, basic_block_sptr block)
{
    std::stringstream msg;

    refresh_io_signature();

    if (my_port < 0 || my_port >= (signed)d_inputs.size()) {
        msg << "input port " << my_port << " out of range for " << block;
        throw std::invalid_argument(msg.str());
    }

    endpoint_vector_t& endps = d_inputs[my_port];
    endpoint endp(block, port);

    endpoint_viter_t p = std::find(endps.begin(), endps.end(), endp);
    if (p != endps.end()) {
        msg << "external input port " << my_port << " already wired to " << endp;
        throw std::invalid_argument(msg.str());
    }

    endps.push_back(endp);
}

void hier_block2_detail::connect_output(int my_port, int port, basic_block_sptr block)
{
    std::stringstream msg;

    refresh_io_signature();

    if (my_port < 0 || my_port >= (signed)d_outputs.size()) {
        msg << "output port " << my_port << " out of range for " << block;
        throw std::invalid_argument(msg.str());
    }

    if (d_outputs[my_port].block()) {
        msg << "external output port " << my_port << " already connected from "
            << d_outputs[my_port];
        throw std::invalid_argument(msg.str());
    }

    d_outputs[my_port] = endpoint(block, port);
}

void hier_block2_detail::disconnect_input(int my_port, int port, basic_block_sptr block)
{
    std::stringstream msg;

    refresh_io_signature();

    if (my_port < 0 || my_port >= (signed)d_inputs.size()) {
        msg << "input port number " << my_port << " out of range for " << block;
        throw std::invalid_argument(msg.str());
    }

    endpoint_vector_t& endps = d_inputs[my_port];
    endpoint endp(block, port);

    endpoint_viter_t p = std::find(endps.begin(), endps.end(), endp);
    if (p == endps.end()) {
        msg << "external input port " << my_port << " not connected to " << endp;
        throw std::invalid_argument(msg.str());
    }

    endps.erase(p);
}

void hier_block2_detail::disconnect_output(int my_port, int port, basic_block_sptr block)
{
    std::stringstream msg;

    refresh_io_signature();

    if (my_port < 0 || my_port >= (signed)d_outputs.size()) {
        msg << "output port number " << my_port << " out of range for " << block;
        throw std::invalid_argument(msg.str());
    }

    if (d_outputs[my_port].block() != block) {
        msg << "block " << block << " not assigned to output " << my_port
            << ", can't disconnect";
        throw std::invalid_argument(msg.str());
    }

    d_outputs[my_port] = endpoint();
}

endpoint_vector_t hier_block2_detail::resolve_port(int port, bool is_input)
{
    std::stringstream msg;

    d_debug_logger->debug("Resolving port {:d} as an {:s} of {:s}",
                          port,
                          is_input ? "input" : "output",
                          d_owner->name());

    endpoint_vector_t result;

    if (is_input) {
        if (port < 0 || port >= (signed)d_inputs.size()) {
            msg << "resolve_port: hierarchical block '" << d_owner->name() << "': input "
                << port << " is out of range";
            throw std::runtime_error(msg.str());
        }

        if (d_inputs[port].empty()) {
            msg << "resolve_port: hierarchical block '" << d_owner->name() << "': input "
                << port << " is not connected internally";
            throw std::runtime_error(msg.str());
        }

        endpoint_vector_t& endps = d_inputs[port];
        endpoint_viter_t p;
        for (p = endps.begin(); p != endps.end(); p++) {
            endpoint_vector_t tmp = resolve_endpoint(*p, true);
            std::copy(tmp.begin(), tmp.end(), back_inserter(result));
        }
    } else {
        if (port < 0 || port >= (signed)d_outputs.size()) {
            msg << "resolve_port: hierarchical block '" << d_owner->name() << "': output "
                << port << " is out of range";
            throw std::runtime_error(msg.str());
        }

        if (d_outputs[port] == endpoint()) {
            msg << "resolve_port: hierarchical block '" << d_owner->name() << "': output "
                << port << " is not connected internally";
            throw std::runtime_error(msg.str());
        }

        result = resolve_endpoint(d_outputs[port], false);
    }

    if (result.empty()) {
        msg << "resolve_port: hierarchical block '" << d_owner->name()
            << "': unable to resolve " << (is_input ? "input port " : "output port ")
            << port;
        throw std::runtime_error(msg.str());
    }

    return result;
}

void hier_block2_detail::disconnect_all()
{
    d_debug_logger->debug("Disconnect all...");
    reset_hier_blocks_parent();
    d_fg->clear();
    d_blocks.clear();

    int max_inputs = d_owner->input_signature()->max_streams();
    int max_outputs = d_owner->output_signature()->max_streams();
    d_inputs = std::vector<endpoint_vector_t>(max_inputs);
    d_outputs = endpoint_vector_t(max_outputs);
}

endpoint_vector_t hier_block2_detail::resolve_endpoint(const endpoint& endp,
                                                       bool is_input) const
{
    std::stringstream msg;
    endpoint_vector_t result;

    // Check if endpoint is a leaf node
    if (cast_to_block_sptr(endp.block())) {
        d_debug_logger->debug("Block {} is a leaf node, returning.",
                              endp.block()->identifier());
        result.push_back(endp);
        return result;
    }

    // Check if endpoint is a hierarchical block
    hier_block2_sptr hier_block2(cast_to_hier_block2_sptr(endp.block()));
    if (hier_block2) {
        d_debug_logger->debug("Resolving endpoint {} as an {:s}, recursing",
                              endp.identifier(),
                              is_input ? "input" : "output");
        return hier_block2->d_detail->resolve_port(endp.port(), is_input);
    }

    msg << "unable to resolve" << (is_input ? " input " : " output ") << "endpoint "
        << endp;
    throw std::runtime_error(msg.str());
}

void hier_block2_detail::flatten_aux(flat_flowgraph_sptr sfg) const
{
    hier_block2_sptr parent = d_parent.lock();
    d_debug_logger->debug(" ** Flattening {:s} parent: {:s}",
                          d_owner->name(),
                          parent ? parent->name() : "NULL");
    bool is_top_block = (dynamic_cast<top_block*>(d_owner) != NULL);

    // Add my edges to the flow graph, resolving references to actual endpoints
    edge_vector_t edges = d_fg->edges();
    msg_edge_vector_t msg_edges = d_fg->msg_edges();
    edge_viter_t p;
    msg_edge_viter_t q;

    // Only run setup_rpc if ControlPort config param is enabled.
    bool ctrlport_on = prefs::singleton()->get_bool("ControlPort", "on", false);

    int min_buff = 0;
    int max_buff = 0;
    // Determine how the buffers should be set
    bool set_all_min_buff = d_owner->all_min_output_buffer_p();
    bool set_all_max_buff = d_owner->all_max_output_buffer_p();
    // Get the min and max buffer length
    if (set_all_min_buff) {
        d_debug_logger->debug("Getting ({:s}) min buffer", d_owner->alias());
        min_buff = d_owner->min_output_buffer();
    }
    if (set_all_max_buff) {
        d_debug_logger->debug("Getting ({:s}) max buffer", d_owner->alias());
        max_buff = d_owner->max_output_buffer();
    }

    // For every block (gr::block and gr::hier_block2), set up the RPC
    // interface.
    for (p = edges.begin(); p != edges.end(); p++) {
        basic_block_sptr b;
        b = p->src().block();

        if (set_all_min_buff) {
            // sets the min buff for every block within hier_block2
            if (min_buff != 0) {
                block_sptr bb = std::dynamic_pointer_cast<block>(b);
                if (bb != 0) {
                    if (bb->min_output_buffer(0) != min_buff) {
                        d_debug_logger->debug(
                            "Block ({:s}) min_buff ({:d})", bb->alias(), min_buff);
                        bb->set_min_output_buffer(min_buff);
                    }
                } else {
                    hier_block2_sptr hh = std::dynamic_pointer_cast<hier_block2>(b);
                    if (hh != 0) {
                        if (hh->min_output_buffer(0) != min_buff) {
                            d_debug_logger->debug(
                                "HBlock ({:s}) min_buff ({:d})", hh->alias(), min_buff);
                            hh->set_min_output_buffer(min_buff);
                        }
                    }
                }
            }
        }
        if (set_all_max_buff) {
            // sets the max buff for every block within hier_block2
            if (max_buff != 0) {
                block_sptr bb = std::dynamic_pointer_cast<block>(b);
                if (bb != 0) {
                    if (bb->max_output_buffer(0) != max_buff) {
                        d_debug_logger->debug(
                            "Block ({:s}) max_buff ({:d})", bb->alias(), max_buff);
                        bb->set_max_output_buffer(max_buff);
                    }
                } else {
                    hier_block2_sptr hh = std::dynamic_pointer_cast<hier_block2>(b);
                    if (hh != 0) {
                        if (hh->max_output_buffer(0) != max_buff) {
                            d_debug_logger->debug(
                                "HBlock ({:s}) max_buff ({:d})", hh->alias(), max_buff);
                            hh->set_max_output_buffer(max_buff);
                        }
                    }
                }
            }
        }

        b = p->dst().block();
        if (set_all_min_buff) {
            // sets the min buff for every block within hier_block2
            if (min_buff != 0) {
                block_sptr bb = std::dynamic_pointer_cast<block>(b);
                if (bb != 0) {
                    if (bb->min_output_buffer(0) != min_buff) {
                        d_debug_logger->debug(
                            "Block ({:s}) min_buff ({:d})", bb->alias(), min_buff);
                        bb->set_min_output_buffer(min_buff);
                    }
                } else {
                    hier_block2_sptr hh = std::dynamic_pointer_cast<hier_block2>(b);
                    if (hh != 0) {
                        if (hh->min_output_buffer(0) != min_buff) {
                            d_debug_logger->debug(
                                "HBlock ({:s}) min_buff ({:d})", hh->alias(), min_buff);
                            hh->set_min_output_buffer(min_buff);
                        }
                    }
                }
            }
        }
        if (set_all_max_buff) {
            // sets the max buff for every block within hier_block2
            if (max_buff != 0) {
                block_sptr bb = std::dynamic_pointer_cast<block>(b);
                if (bb != 0) {
                    if (bb->max_output_buffer(0) != max_buff) {
                        d_debug_logger->debug(
                            "Block ({:s}) max_buff ({:d})", bb->alias(), max_buff);
                        bb->set_max_output_buffer(max_buff);
                    }
                } else {
                    hier_block2_sptr hh = std::dynamic_pointer_cast<hier_block2>(b);
                    if (hh != 0) {
                        if (hh->max_output_buffer(0) != max_buff) {
                            d_debug_logger->debug(
                                "HBlock ({:s}) max_buff ({:d})", hh->alias(), max_buff);
                            hh->set_max_output_buffer(max_buff);
                        }
                    }
                }
            }
        }
    }

    d_debug_logger->debug("Flattening stream connections: ");

    for (p = edges.begin(); p != edges.end(); p++) {
        d_debug_logger->debug("Flattening edge {}", (*p).identifier());

        endpoint_vector_t src_endps = resolve_endpoint(p->src(), false);
        endpoint_vector_t dst_endps = resolve_endpoint(p->dst(), true);

        endpoint_viter_t s, d;
        for (s = src_endps.begin(); s != src_endps.end(); s++) {
            for (d = dst_endps.begin(); d != dst_endps.end(); d++) {
                d_debug_logger->debug(" {} -> {}", (*s).identifier(), (*d).identifier());
                sfg->connect(*s, *d);
            }
        }
    }

    // loop through flattening hierarchical connections
    d_debug_logger->debug("Flattening msg connections: ");

    std::vector<std::pair<msg_endpoint, bool>> resolved_endpoints;
    for (q = msg_edges.begin(); q != msg_edges.end(); q++) {
        d_debug_logger->debug(" flattening edge ( {}, {}, {:d}) -> ( {}, {}, {:d})",
                              q->src().block()->identifier(),
                              pmt::write_string(q->src().port()),
                              q->src().is_hier(),
                              q->dst().block()->identifier(),
                              pmt::write_string(q->dst().port()),
                              q->dst().is_hier());


        if (q->src().is_hier() && q->src().block().get() == d_owner) {
            // connection into this block ..
            d_debug_logger->debug("hier incoming port: {}", q->src().identifier());
            sfg->replace_endpoint(q->src(), q->dst(), false);
            resolved_endpoints.push_back(std::pair<msg_endpoint, bool>(q->src(), false));
        } else if (q->dst().is_hier() && q->dst().block().get() == d_owner) {
            // connection out of this block
            d_debug_logger->debug("hier outgoing port: {}", q->dst().identifier());
            sfg->replace_endpoint(q->dst(), q->src(), true);
            resolved_endpoints.push_back(std::pair<msg_endpoint, bool>(q->dst(), true));
        } else {
            // internal connection only
            d_debug_logger->debug("internal msg connection: {} --> {}",
                                  q->src().identifier(),
                                  q->dst().identifier());
            sfg->connect(q->src(), q->dst());
        }
    }

    for (std::vector<std::pair<msg_endpoint, bool>>::iterator it =
             resolved_endpoints.begin();
         it != resolved_endpoints.end();
         it++) {
        d_debug_logger->debug(
            "sfg->clear_endpoint({}, {})", it->first.identifier(), it->second);
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
    std::vector<basic_block_sptr>::const_iterator b; // Because flatten_aux is const
    for (b = d_blocks.begin(); b != d_blocks.end(); b++) {
        tmp.push_back(*b);
    }

    // Now add the list of connected input blocks
    std::stringstream msg;
    for (unsigned int i = 0; i < d_inputs.size(); i++) {
        if (d_inputs[i].empty()) {
            msg << "In hierarchical block " << d_owner->name() << ", input " << i
                << " is not connected internally";
            throw std::runtime_error(msg.str());
        }

        for (unsigned int j = 0; j < d_inputs[i].size(); j++)
            tmp.push_back(d_inputs[i][j].block());
    }

    for (unsigned int i = 0; i < d_outputs.size(); i++) {
        basic_block_sptr blk = d_outputs[i].block();
        if (!blk) {
            msg << "In hierarchical block " << d_owner->name() << ", output " << i
                << " is not connected internally";
            throw std::runtime_error(msg.str());
        }
        // Set the buffers of only the blocks connected to the hier output
        if (!set_all_min_buff) {
            min_buff = d_owner->min_output_buffer(i);
            if (min_buff != 0) {
                block_sptr bb = std::dynamic_pointer_cast<block>(blk);
                if (bb != 0) {
                    int bb_src_port = d_outputs[i].port();
                    d_debug_logger->debug("Block ({:s}) Port ({:d}) min_buff ({:d})",
                                          bb->alias(),
                                          bb_src_port,
                                          min_buff);
                    bb->set_min_output_buffer(bb_src_port, min_buff);
                } else {
                    hier_block2_sptr hh = std::dynamic_pointer_cast<hier_block2>(blk);
                    if (hh != 0) {
                        int hh_src_port = d_outputs[i].port();
                        d_debug_logger->debug("HBlock ({:s}) Port ({:d}) min_buff ({:d})",
                                              hh->alias(),
                                              hh_src_port,
                                              min_buff);
                        hh->set_min_output_buffer(hh_src_port, min_buff);
                    }
                }
            }
        }
        if (!set_all_max_buff) {
            max_buff = d_owner->max_output_buffer(i);
            if (max_buff != 0) {
                block_sptr bb = std::dynamic_pointer_cast<block>(blk);
                if (bb != 0) {
                    int bb_src_port = d_outputs[i].port();
                    d_debug_logger->debug("Block ({:s}) Port ({:d}) max_buff ({:d})",
                                          bb->alias(),
                                          bb_src_port,
                                          max_buff);
                    bb->set_max_output_buffer(bb_src_port, max_buff);
                } else {
                    hier_block2_sptr hh = std::dynamic_pointer_cast<hier_block2>(blk);
                    if (hh != 0) {
                        int hh_src_port = d_outputs[i].port();
                        d_debug_logger->debug("HBlock ({:s}) Port ({:d}) max_buff ({:d})",
                                              hh->alias(),
                                              hh_src_port,
                                              max_buff);
                        hh->set_max_output_buffer(hh_src_port, max_buff);
                    }
                }
            }
        }
        tmp.push_back(blk);
    }
    sort(tmp.begin(), tmp.end());

    std::insert_iterator<basic_block_vector_t> inserter(blocks, blocks.begin());
    unique_copy(tmp.begin(), tmp.end(), inserter);

    // Recurse hierarchical children
    for (basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
        hier_block2_sptr hier_block2(cast_to_hier_block2_sptr(*p));
        if (hier_block2 && (hier_block2.get() != d_owner)) {
            d_debug_logger->debug("flatten_aux: recursing into hierarchical block {:s}",
                                  hier_block2->alias());
            hier_block2->d_detail->flatten_aux(sfg);
        }
    }

    // prune any remaining hier connections
    //   if they were not replaced with hier internal connections while in sub-calls
    //   they must remain unconnected and can be deleted...
    if (is_top_block) {
        sfg->clear_hier();
    }

    // print all primitive connections at exit
    if (is_top_block) {
        d_debug_logger->debug("flatten_aux finished in top_block");
        // sfg->dump();
    }

    // if ctrlport is enabled, call setup RPC for all blocks in the flowgraph
    if (ctrlport_on) {
        for (b = blocks.begin(); b != blocks.end(); b++) {
            if (!(*b)->is_rpc_set()) {
                (*b)->setup_rpc();
                (*b)->rpc_set();
            }
        }
    }
}

void hier_block2_detail::lock()
{
    d_debug_logger->debug("lock: entered in {:p}", (void*)this);

    auto parent = d_parent.lock();
    if (parent)
        parent->d_detail->lock();
    else {
        auto owner = dynamic_cast<top_block*>(d_owner);
        if (owner)
            owner->lock();
    }
}

void hier_block2_detail::unlock()
{
    d_debug_logger->debug("unlock: entered in {:p}", (void*)this);

    auto parent = d_parent.lock();
    if (parent)
        parent->d_detail->unlock();
    else {
        auto owner = dynamic_cast<top_block*>(d_owner);
        if (owner)
            owner->unlock();
    }
}

void hier_block2_detail::set_processor_affinity(const std::vector<int>& mask)
{
    basic_block_vector_t tmp = d_fg->calc_used_blocks();
    for (basic_block_viter_t p = tmp.begin(); p != tmp.end(); p++) {
        (*p)->set_processor_affinity(mask);
    }
}

void hier_block2_detail::unset_processor_affinity()
{
    basic_block_vector_t tmp = d_fg->calc_used_blocks();
    for (basic_block_viter_t p = tmp.begin(); p != tmp.end(); p++) {
        (*p)->unset_processor_affinity();
    }
}

std::vector<int> hier_block2_detail::processor_affinity()
{
    basic_block_vector_t tmp = d_fg->calc_used_blocks();
    return tmp[0]->processor_affinity();
}

void hier_block2_detail::set_log_level(const std::string& level)
{
    basic_block_vector_t tmp = d_fg->calc_used_blocks();
    for (basic_block_viter_t p = tmp.begin(); p != tmp.end(); p++) {
        (*p)->set_log_level(level);
    }
}

std::string hier_block2_detail::log_level()
{
    // Assume that log_level was set for all hier_block2 blocks
    basic_block_vector_t tmp = d_fg->calc_used_blocks();
    return tmp[0]->log_level();
}

void hier_block2_detail::set_parent(hier_block2* parent)
{
    auto old_parent = d_parent.lock();
    if (!old_parent) {
        d_parent = parent->to_hier_block2();
        d_parent_refcnt++;
        return;
    }
    if (old_parent.get() == parent) {
        d_parent_refcnt++;
        return;
    }
    std::stringstream msg;
    msg << "A hierarchical block cannot have multiple parents. Block \""
        << d_owner->name() << "\" must be completely removed from parent \""
        << old_parent->name() << "\" before being added to parent \"" << parent->name()
        << "\".";
    throw std::runtime_error(msg.str());
}

void hier_block2_detail::reset_parent(bool force)
{
    if (force) {
        d_parent_refcnt = 0;
        d_parent.reset();
    } else {
        if (d_parent_refcnt > 0)
            d_parent_refcnt--;
        if (d_parent_refcnt == 0)
            d_parent.reset();
    }
}

void hier_block2_detail::reset_hier_blocks_parent()
{
    basic_block_vector_t tmp = d_fg->calc_used_blocks();
    hier_block2_sptr hb;
    std::vector<basic_block_sptr>::const_iterator b;

    for (b = tmp.begin(); b != tmp.end(); b++) {
        hb = cast_to_hier_block2_sptr(*b);
        if (hb)
            hb->d_detail->reset_parent(true);
    }

    for (b = d_blocks.begin(); b != d_blocks.end(); b++) {
        hb = cast_to_hier_block2_sptr(*b);
        if (hb)
            hb->d_detail->reset_parent(true);
    }

    for (unsigned int i = 0; i < d_inputs.size(); i++) {
        if (d_inputs[i].empty())
            continue;

        for (unsigned int j = 0; j < d_inputs[i].size(); j++) {
            hb = cast_to_hier_block2_sptr(d_inputs[i][j].block());
            if (hb)
                hb->d_detail->reset_parent(true);
        }
    }

    for (unsigned int i = 0; i < d_outputs.size(); i++) {
        basic_block_sptr blk = d_outputs[i].block();
        if (blk) {
            hb = cast_to_hier_block2_sptr(blk);
            if (hb)
                hb->d_detail->reset_parent(true);
        }
    }
}


} /* namespace gr */

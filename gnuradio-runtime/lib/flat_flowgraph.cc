/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "flat_flowgraph.h"
#include <gnuradio/block_detail.h>
#include <gnuradio/buffer.h>
#include <gnuradio/buffer_double_mapped.h>
#include <gnuradio/buffer_reader.h>
#include <gnuradio/buffer_type.h>
#include <gnuradio/logger.h>
#include <gnuradio/prefs.h>
#include <volk/volk.h>
#include <iostream>
#include <map>
#include <numeric>

namespace gr {

flat_flowgraph_sptr make_flat_flowgraph()
{
    return flat_flowgraph_sptr(new flat_flowgraph());
}

flat_flowgraph::flat_flowgraph() {}

flat_flowgraph::~flat_flowgraph() {}

void flat_flowgraph::setup_connections()
{
    basic_block_vector_t blocks = calc_used_blocks();

    // Assign block details to blocks
    for (basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
        allocate_block_detail(*p);
    }

    // Connect inputs to outputs for each block
    for (basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
        connect_block_inputs(*p);

        block_sptr block = cast_to_block_sptr(*p);
        block->set_unaligned(0);
        block->set_is_unaligned(false);
    }

    // Connect message ports connections
    for (msg_edge_viter_t i = d_msg_edges.begin(); i != d_msg_edges.end(); i++) {
        d_debug_logger->debug("flat_fg connecting msg primitives: ({}, {})->({}, {})\n",
                              i->src().block()->identifier(),
                              pmt::write_string(i->src().port()),
                              i->dst().block()->identifier(),
                              pmt::write_string(i->dst().port()));
        i->src().block()->message_port_sub(
            i->src().port(), pmt::cons(i->dst().block()->alias_pmt(), i->dst().port()));
    }
}

void flat_flowgraph::allocate_block_detail(basic_block_sptr block)
{
    int ninputs = calc_used_ports(block, true).size();
    int noutputs = calc_used_ports(block, false).size();

    block_sptr grblock = cast_to_block_sptr(block);
    if (!grblock) {
        std::ostringstream msg;
        msg << "allocate_block_detail found non-gr::block (" << block->alias() << ")";
        throw std::runtime_error(msg.str());
    }

    // Determine the downstream max per output port
    std::vector<int> downstream_max_nitems(noutputs, 0);
    std::vector<uint64_t> downstream_lcm_nitems(noutputs, 1);
    std::vector<uint32_t> downstream_max_out_mult(noutputs, 1);

#ifdef BUFFER_DEBUG
    d_logger->debug("BLOCK: {:s}", block->identifier()); // could also be d_debug_logger
#endif
    for (int i = 0; i < noutputs; i++) {
        int nitems = 0;
        uint64_t lcm_nitems = 1;
        uint32_t max_out_multiple = 1;
        basic_block_vector_t downstream_blocks = calc_downstream_blocks(grblock, i);
        for (basic_block_viter_t blk = downstream_blocks.begin();
             blk != downstream_blocks.end();
             blk++) {
            block_sptr dgrblock = cast_to_block_sptr(*blk);
            if (!dgrblock)
                throw std::runtime_error("allocate_buffer found non-gr::block");

#ifdef BUFFER_DEBUG
            d_logger->debug("      DWNSTRM BLOCK: {:s}", dgrblock->identifier());
#endif

            // If any downstream blocks are decimators and/or have a large
            // output_multiple, ensure we have a buffer at least twice their
            // decimation factor*output_multiple
            double decimation = (1.0 / dgrblock->relative_rate());
            int multiple = dgrblock->output_multiple();
            int history = dgrblock->history();
            nitems =
                std::max(nitems, static_cast<int>(2 * (decimation * multiple + history)));

            // Calculate the LCM of downstream reader nitems
#ifdef BUFFER_DEBUG
            d_logger->debug("        OUT MULTIPLE: {:d}", multiple);
#endif

            if (dgrblock->fixed_rate()) {
                lcm_nitems =
                    std::lcm(lcm_nitems,
                             (uint64_t)(dgrblock->fixed_rate_noutput_to_ninput(1) -
                                        (dgrblock->history() - 1)));
            }

            if (dgrblock->relative_rate() != 1.0) {
                // Relative rate
                lcm_nitems = std::lcm(lcm_nitems, dgrblock->relative_rate_d());
            }

            // Sanity check, make sure lcm_nitems is at least 1
            if (lcm_nitems < 1) {
                lcm_nitems = 1;
            }

            if (static_cast<uint32_t>(multiple) > max_out_multiple) {
                max_out_multiple = multiple;
            }

#ifdef BUFFER_DEBUG
            d_logger->debug("        NINPUT_ITEMS: {:d}", nitems);
            d_logger->debug("        LCM NITEMS: {:d}", lcm_nitems);
            d_logger->debug("        HISTORY: {:d}", dgrblock->history());
            d_logger->debug("        DELAY: {:d}", dgrblock->sample_delay(0));
#endif
        }
        downstream_max_nitems[i] = nitems;
        downstream_lcm_nitems[i] = lcm_nitems;
        downstream_max_out_mult[i] = max_out_multiple;
    }

    // Allocate the block detail and necessary buffers
    grblock->allocate_detail(ninputs,
                             noutputs,
                             downstream_max_nitems,
                             downstream_lcm_nitems,
                             downstream_max_out_mult);
}

void flat_flowgraph::connect_block_inputs(basic_block_sptr block)
{
    block_sptr grblock = cast_to_block_sptr(block);
    if (!grblock)
        throw std::runtime_error("connect_block_inputs found non-gr::block");

    // Get its detail and edges that feed into it
    block_detail_sptr detail = grblock->detail();
    edge_vector_t in_edges = calc_upstream_edges(block);

    // For each edge that feeds into it
    for (edge_viter_t e = in_edges.begin(); e != in_edges.end(); e++) {
        // Set the buffer reader on the destination port to the output
        // buffer on the source port
        int dst_port = e->dst().port();
        int src_port = e->src().port();
        basic_block_sptr src_block = e->src().block();
        block_sptr src_grblock = cast_to_block_sptr(src_block);
        if (!src_grblock)
            throw std::runtime_error("connect_block_inputs found non-gr::block");

        // In order to determine the buffer's transfer type, we need to examine both
        // the upstream and the downstream buffer_types
        buffer_type src_buf_type =
            src_grblock->output_signature()->stream_buffer_type(src_port);
        buffer_type dest_buf_type =
            grblock->input_signature()->stream_buffer_type(dst_port);

        transfer_type buf_xfer_type;
        if (src_buf_type == buffer_double_mapped::type &&
            dest_buf_type == buffer_double_mapped::type) {
            buf_xfer_type = transfer_type::HOST_TO_HOST;
        } else if (src_buf_type != buffer_double_mapped::type &&
                   dest_buf_type == buffer_double_mapped::type) {
            buf_xfer_type = transfer_type::DEVICE_TO_HOST;
        } else if (src_buf_type == buffer_double_mapped::type &&
                   dest_buf_type != buffer_double_mapped::type) {
            buf_xfer_type = transfer_type::HOST_TO_DEVICE;
        } else if (src_buf_type != buffer_double_mapped::type &&
                   dest_buf_type != buffer_double_mapped::type) {
            buf_xfer_type = transfer_type::DEVICE_TO_DEVICE;
        }

        buffer_sptr src_buffer;
        if (dest_buf_type == buffer_double_mapped::type ||
            dest_buf_type == src_buf_type) {
            // The block is not using a custom buffer OR the block and the upstream
            // block both use the same kind of custom buffer
            src_buffer = src_grblock->detail()->output(src_port);
        } else {
            if (dest_buf_type != buffer_double_mapped::type &&
                src_buf_type == buffer_double_mapped::type) {
                // The block uses a custom buffer but the upstream block does not
                // therefore the upstream block's buffer can be replaced with the
                // type of buffer that the block needs
                std::ostringstream msg;
                msg << "Block: " << grblock->identifier()
                    << " replacing upstream block: " << src_grblock->identifier()
                    << " buffer with a custom buffer";
                GR_LOG_DEBUG(d_logger, msg.str());
                src_buffer = src_grblock->replace_buffer(src_port, dst_port, grblock);
            } else {
                // Both the block and upstream block use incompatible buffer types
                // which is not currently allowed
                std::ostringstream msg;
                msg << "Block: " << grblock->identifier()
                    << " and upstream block: " << src_grblock->identifier()
                    << " use incompatible custom buffer types (" << dest_buf_type.name()
                    << " -- " << src_buf_type.name() << ")  --> "
                    << (dest_buf_type == src_buf_type);
                GR_LOG_ERROR(d_logger, msg.str());
                throw std::runtime_error(msg.str());
            }
        }

        // Set buffer's transfer type
        src_buffer->set_transfer_type(buf_xfer_type);

        std::ostringstream msg;
        msg << "Setting input " << dst_port << " from edge " << (*e).identifier()
            << " transfer type: " << buf_xfer_type;
        GR_LOG_DEBUG(d_debug_logger, msg.str());

        detail->set_input(dst_port,
                          buffer_add_reader(src_buffer,
                                            grblock->history() - 1,
                                            grblock,
                                            grblock->sample_delay(src_port)));
    }
}

void flat_flowgraph::merge_connections(flat_flowgraph_sptr old_ffg)
{
    // Allocate block details if needed.  Only new blocks that aren't pruned out
    // by flattening will need one; existing blocks still in the new flowgraph will
    // already have one.
    for (basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
        block_sptr block = cast_to_block_sptr(*p);

        if (!block->detail()) {
            d_debug_logger->debug("merge: allocating new detail for block {:s}",
                                  block->identifier());
            allocate_block_detail(block);
        } else {
            d_debug_logger->debug("merge: reusing original detail for block {:s}",
                                  block->identifier());
        }
    }

    // Calculate the old edges that will be going away, and clear the
    // buffer readers on the RHS.
    for (edge_viter_t old_edge = old_ffg->d_edges.begin();
         old_edge != old_ffg->d_edges.end();
         old_edge++) {
        d_debug_logger->debug("merge: testing old edge {:s}...", old_edge->identifier());

        edge_viter_t new_edge;
        for (new_edge = d_edges.begin(); new_edge != d_edges.end(); new_edge++)
            if (new_edge->src() == old_edge->src() && new_edge->dst() == old_edge->dst())
                break;

        if (new_edge == d_edges.end()) { // not found in new edge list
            d_debug_logger->debug("not in new edge list");
            // zero the buffer reader on RHS of old edge
            block_sptr block(cast_to_block_sptr(old_edge->dst().block()));
            int port = old_edge->dst().port();
            block->detail()->set_input(port, buffer_reader_sptr());
        } else {
            d_debug_logger->debug("found in new edge list");
        }
    }

    // Now connect inputs to outputs, reusing old buffer readers if they exist
    for (basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
        block_sptr block = cast_to_block_sptr(*p);

        d_debug_logger->debug("merge: merging {:s}...", block->identifier());

        if (old_ffg->has_block_p(*p)) {
            // Block exists in old flow graph
            d_debug_logger->debug("used in old flow graph");
            block_detail_sptr detail = block->detail();

            // Iterate through the inputs and see what needs to be done
            int ninputs = calc_used_ports(block, true).size(); // Might be different now
            for (int i = 0; i < ninputs; i++) {
                d_debug_logger->debug(
                    "Checking input {:s}:{:d}...", block->identifier(), i);
                edge edge = calc_upstream_edge(*p, i);

                // Fish out old buffer reader and see if it matches correct buffer from
                // edge list
                block_sptr src_block = cast_to_block_sptr(edge.src().block());
                block_detail_sptr src_detail = src_block->detail();
                buffer_sptr src_buffer = src_detail->output(edge.src().port());
                buffer_reader_sptr old_reader;
                if (i < detail->ninputs()) // Don't exceed what the original detail has
                    old_reader = detail->input(i);

                // If there's a match, use it
                if (old_reader && (src_buffer == old_reader->buffer())) {
                    d_debug_logger->debug("matched, reusing");
                } else {
                    d_debug_logger->debug("needs a new reader");

                    // Create new buffer reader and assign
                    detail->set_input(
                        i, buffer_add_reader(src_buffer, block->history() - 1, block));
                }
            }
        } else {
            // Block is new, it just needs buffer readers at this point
            d_debug_logger->debug("new block");
            connect_block_inputs(block);

            // Make sure all buffers are aligned
            setup_buffer_alignment(block);
        }

        // Connect message ports connections
        for (msg_edge_viter_t i = d_msg_edges.begin(); i != d_msg_edges.end(); i++) {
            d_debug_logger->debug(
                "flat_fg connecting msg primitives: ({}, {})->({}, {})\n",
                i->src().block()->identifier(),
                pmt::write_string(i->src().port()),
                i->dst().block()->identifier(),
                pmt::write_string(i->dst().port()));
            i->src().block()->message_port_sub(
                i->src().port(),
                pmt::cons(i->dst().block()->alias_pmt(), i->dst().port()));
        }

        // Now deal with the fact that the block details might have
        // changed numbers of inputs and outputs vs. in the old
        // flowgraph.
        block->detail()->reset_nitem_counters();
        block->detail()->clear_tags();
    }
}

void flat_flowgraph::setup_buffer_alignment(block_sptr block)
{
    const int alignment = volk_get_alignment();
    for (int i = 0; i < block->detail()->ninputs(); i++) {
        const void* r = (const void*)block->detail()->input(i)->read_pointer();
        uintptr_t ri = (uintptr_t)r % alignment;
        // std::cerr << "reader: " << r << "  alignment: " << ri << std::endl;
        if (ri != 0) {
            size_t itemsize = block->detail()->input(i)->get_sizeof_item();
            block->detail()->input(i)->update_read_pointer((alignment - ri) / itemsize);
        }
        block->set_unaligned(0);
        block->set_is_unaligned(false);
    }

    for (int i = 0; i < block->detail()->noutputs(); i++) {
        void* w = (void*)block->detail()->output(i)->write_pointer();
        uintptr_t wi = (uintptr_t)w % alignment;
        // std::cerr << "writer: " << w << "  alignment: " << wi << std::endl;
        if (wi != 0) {
            size_t itemsize = block->detail()->output(i)->get_sizeof_item();
            block->detail()->output(i)->update_write_pointer((alignment - wi) / itemsize);
        }
        block->set_unaligned(0);
        block->set_is_unaligned(false);
    }
}

std::string flat_flowgraph::edge_list()
{
    std::stringstream s;
    for (edge_viter_t e = d_edges.begin(); e != d_edges.end(); e++)
        s << (*e) << std::endl;
    return s.str();
}

std::string flat_flowgraph::msg_edge_list()
{
    std::stringstream s;
    for (msg_edge_viter_t e = d_msg_edges.begin(); e != d_msg_edges.end(); e++)
        s << (*e) << std::endl;
    return s.str();
}

void flat_flowgraph::dump()
{
    for (edge_viter_t e = d_edges.begin(); e != d_edges.end(); e++)
        d_logger->info(" edge: {}", (*e).identifier());

    for (basic_block_viter_t p = d_blocks.begin(); p != d_blocks.end(); p++) {
        d_logger->info(" block: {}", (*p)->identifier());
        block_detail_sptr detail = cast_to_block_sptr(*p)->detail();
        d_logger->info(" detail @{}:", static_cast<void*>(detail.get()));

        int ni = detail->ninputs();
        int no = detail->noutputs();
        for (int i = 0; i < no; i++) {
            buffer_sptr buffer = detail->output(i);
            d_logger->info(
                "   output {:d}: {}",
                i,
                static_cast<std::stringstream&&>(std::stringstream() << buffer).str());
        }

        for (int i = 0; i < ni; i++) {
            buffer_reader_sptr reader = detail->input(i);
            d_logger->info(
                "   reader {:d}: {} reading from buffer={}",
                i,
                static_cast<std::stringstream&&>(std::stringstream() << reader).str(),
                static_cast<std::stringstream&&>(std::stringstream() << reader->buffer())
                    .str());
        }
    }
}

block_vector_t flat_flowgraph::make_block_vector(basic_block_vector_t& blocks)
{
    block_vector_t result;
    for (basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
        result.push_back(cast_to_block_sptr(*p));
    }

    return result;
}

void flat_flowgraph::clear_endpoint(const msg_endpoint& e, bool is_src)
{
    for (size_t i = 0; i < d_msg_edges.size(); i++) {
        if (is_src) {
            if (d_msg_edges[i].src() == e) {
                d_msg_edges.erase(d_msg_edges.begin() + i);
                i--;
            }
        } else {
            if (d_msg_edges[i].dst() == e) {
                d_msg_edges.erase(d_msg_edges.begin() + i);
                i--;
            }
        }
    }
}

void flat_flowgraph::clear_hier()
{
    d_debug_logger->debug("Clear_hier()");
    for (size_t i = 0; i < d_msg_edges.size(); i++) {
        d_debug_logger->debug("edge: {:s}-->{:s}",
                              d_msg_edges[i].src().identifier(),
                              d_msg_edges[i].dst().identifier());
        if (d_msg_edges[i].src().is_hier() || d_msg_edges[i].dst().is_hier()) {
            d_debug_logger->debug("is hier");
            d_msg_edges.erase(d_msg_edges.begin() + i);
            i--;
        }
    }
}

void flat_flowgraph::replace_endpoint(const msg_endpoint& e,
                                      const msg_endpoint& r,
                                      bool is_src)
{
    d_debug_logger->debug("flat_flowgraph::replace_endpoint( {}, {}, {:d} )\n",
                          e.block()->identifier(),
                          r.block()->identifier(),
                          is_src);
    for (size_t i = 0; i < d_msg_edges.size(); i++) {
        if (is_src) {
            if (d_msg_edges[i].src() == e) {
                d_debug_logger->debug(
                    "flat_flowgraph::replace_endpoint() flattening to ( {}, {} )\n",
                    r.identifier(),
                    d_msg_edges[i].dst().identifier());
                d_msg_edges.push_back(msg_edge(r, d_msg_edges[i].dst()));
            }
        } else {
            if (d_msg_edges[i].dst() == e) {
                d_debug_logger->debug(
                    "flat_flowgraph::replace_endpoint() flattening to ( {}, {} )\n",
                    r.identifier(),
                    d_msg_edges[i].src().identifier());
                d_msg_edges.push_back(msg_edge(d_msg_edges[i].src(), r));
            }
        }
    }
}

void flat_flowgraph::enable_pc_rpc()
{
#ifdef GR_PERFORMANCE_COUNTERS
    if (prefs::singleton()->get_bool("PerfCounters", "on", false)) {
        basic_block_viter_t p;
        for (p = d_blocks.begin(); p != d_blocks.end(); p++) {
            block_sptr block = cast_to_block_sptr(*p);
            if (!block->is_pc_rpc_set())
                block->setup_pc_rpc();
        }
    }
#endif /* GR_PERFORMANCE_COUNTERS */
}

} /* namespace gr */

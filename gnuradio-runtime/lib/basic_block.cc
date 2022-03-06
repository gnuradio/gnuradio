/* -*- c++ -*- */
/*
 * Copyright 2006,2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <memory>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/basic_block.h>
#include <gnuradio/block_registry.h>
#include <gnuradio/logger.h>
#include <sstream>
#include <stdexcept>

namespace gr {

static long s_next_id = 0;
static long s_ncurrently_allocated = 0;

long basic_block_ncurrently_allocated() { return s_ncurrently_allocated; }

basic_block::basic_block(const std::string& name,
                         io_signature::sptr input_signature,
                         io_signature::sptr output_signature)
    : d_name(name),
      d_input_signature(input_signature),
      d_output_signature(output_signature),
      d_unique_id(s_next_id++),
      d_symbolic_id(global_block_registry.block_register(this)),
      d_symbol_name(global_block_registry.register_symbolic_name(this)),
      d_color(WHITE),
      d_rpc_set(false),
      d_logger(std::make_shared<gr::logger>(name)),
      d_debug_logger(std::make_shared<logger_ptr::element_type>(name + " (debug)")),
      d_message_subscribers(pmt::make_dict())
{
    d_logger->set_level(logging::singleton().default_level());
    d_debug_logger->set_level(logging::singleton().debug_level());
    s_ncurrently_allocated++;
}

basic_block::~basic_block()
{
    s_ncurrently_allocated--;
    global_block_registry.block_unregister(this);
}

basic_block_sptr basic_block::to_basic_block() { return shared_from_this(); }

void basic_block::set_block_alias(std::string name)
{
    // Only keep one alias'd name around for each block. If we don't
    // have an alias, add it; if we do, update the entry in the
    // registry.
    if (alias_set())
        global_block_registry.update_symbolic_name(this, name);
    else
        global_block_registry.register_symbolic_name(this, name);

    // set the block's alias
    d_symbol_alias = name;
    d_logger->set_name(name);
    d_debug_logger->set_name(name + " (debug)");
}

// ** Message passing interface **

//  - register a new input message port
void basic_block::message_port_register_in(pmt::pmt_t port_id)
{
    if (!pmt::is_symbol(port_id)) {
        throw std::runtime_error("message_port_register_in: bad port id");
    }
    msg_queue[port_id] = msg_queue_t();
}

pmt::pmt_t basic_block::message_ports_in()
{
    pmt::pmt_t port_names = pmt::make_vector(msg_queue.size(), pmt::PMT_NIL);
    msg_queue_map_itr itr = msg_queue.begin();
    for (size_t i = 0; i < msg_queue.size(); i++) {
        pmt::vector_set(port_names, i, (*itr).first);
        itr++;
    }
    return port_names;
}

//  - register a new output message port
void basic_block::message_port_register_out(pmt::pmt_t port_id)
{
    if (!pmt::is_symbol(port_id)) {
        throw std::runtime_error("message_port_register_out: bad port id");
    }
    if (pmt::dict_has_key(d_message_subscribers, port_id)) {
        throw std::runtime_error("message_port_register_out: port already in use");
    }
    d_message_subscribers = pmt::dict_add(d_message_subscribers, port_id, pmt::PMT_NIL);
}

pmt::pmt_t basic_block::message_ports_out()
{
    size_t len = pmt::length(d_message_subscribers);
    pmt::pmt_t port_names = pmt::make_vector(len, pmt::PMT_NIL);
    pmt::pmt_t keys = pmt::dict_keys(d_message_subscribers);
    for (size_t i = 0; i < len; i++) {
        pmt::vector_set(port_names, i, pmt::nth(i, keys));
    }
    return port_names;
}

//  - publish a message on a message port
void basic_block::message_port_pub(pmt::pmt_t port_id, pmt::pmt_t msg)
{
    if (!pmt::dict_has_key(d_message_subscribers, port_id)) {
        throw std::runtime_error("port does not exist");
    }

    pmt::pmt_t currlist = pmt::dict_ref(d_message_subscribers, port_id, pmt::PMT_NIL);
    // iterate through subscribers on port
    while (pmt::is_pair(currlist)) {
        pmt::pmt_t target = pmt::car(currlist);

        pmt::pmt_t block = pmt::car(target);
        pmt::pmt_t port = pmt::cdr(target);

        currlist = pmt::cdr(currlist);
        basic_block_sptr blk = global_block_registry.block_lookup(block);
        // blk->post(msg);
        blk->post(port, msg);
    }
}

//  - subscribe to a message port
void basic_block::message_port_sub(pmt::pmt_t port_id, pmt::pmt_t target)
{
    if (!pmt::dict_has_key(d_message_subscribers, port_id)) {
        std::stringstream ss;
        ss << "Port does not exist: \"" << pmt::write_string(port_id)
           << "\" on block: " << pmt::write_string(target) << std::endl;
        throw std::runtime_error(ss.str());
    }
    pmt::pmt_t currlist = pmt::dict_ref(d_message_subscribers, port_id, pmt::PMT_NIL);

    // ignore re-adds of the same target
    if (!pmt::list_has(currlist, target))
        d_message_subscribers = pmt::dict_add(
            d_message_subscribers, port_id, pmt::list_add(currlist, target));
}

void basic_block::message_port_unsub(pmt::pmt_t port_id, pmt::pmt_t target)
{
    if (!pmt::dict_has_key(d_message_subscribers, port_id)) {
        std::stringstream ss;
        ss << "Port does not exist: \"" << pmt::write_string(port_id)
           << "\" on block: " << pmt::write_string(target) << std::endl;
        throw std::runtime_error(ss.str());
    }

    // ignore unsubs of unknown targets
    pmt::pmt_t currlist = pmt::dict_ref(d_message_subscribers, port_id, pmt::PMT_NIL);
    d_message_subscribers =
        pmt::dict_add(d_message_subscribers, port_id, pmt::list_rm(currlist, target));
}

void basic_block::_post(pmt::pmt_t which_port, pmt::pmt_t msg)
{
    insert_tail(which_port, msg);
}

void basic_block::insert_tail(pmt::pmt_t which_port, pmt::pmt_t msg)
{
    gr::thread::scoped_lock guard(mutex);
    const auto& queue = msg_queue.find(which_port);
    if (queue == msg_queue.end()) {
        d_logger->error("attempted insertion on invalid queue {:s}",
                        pmt::symbol_to_string(which_port));
        throw std::runtime_error("attempted to insert_tail on invalid queue!");
    }

    queue->second.push_back(msg);

    // wake up thread if BLKD_IN or BLKD_OUT
    global_block_registry.notify_blk(d_symbol_name);
}

pmt::pmt_t basic_block::delete_head_nowait(pmt::pmt_t which_port)
{
    gr::thread::scoped_lock guard(mutex);

    if (empty_p(which_port)) {
        return pmt::pmt_t();
    }

    pmt::pmt_t m(msg_queue[which_port].front());
    msg_queue[which_port].pop_front();

    return m;
}

pmt::pmt_t basic_block::message_subscribers(pmt::pmt_t port)
{
    return pmt::dict_ref(d_message_subscribers, port, pmt::PMT_NIL);
}


} /* namespace gr */

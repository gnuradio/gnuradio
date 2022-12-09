/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/basic_block.h>
#include <gnuradio/block.h>
#include <gnuradio/block_detail.h>
#include <gnuradio/block_registry.h>
#include <gnuradio/tpb_detail.h>

gr::block_registry global_block_registry;

namespace gr {

block_registry::block_registry() { d_ref_map = pmt::make_dict(); }

long block_registry::block_register(basic_block* block)
{
    gr::thread::scoped_lock guard(d_mutex);

    if (d_map.find(block->name()) == d_map.end()) {
        d_map[block->name()] = blocksubmap_t();
        d_map[block->name()][0] = block;
        return 0;
    } else {
        for (size_t i = 0; i <= d_map[block->name()].size(); i++) {
            if (d_map[block->name()].find(i) == d_map[block->name()].end()) {
                d_map[block->name()][i] = block;
                return i;
            }
        }
    }
    throw std::runtime_error("should not reach this");
}

void block_registry::block_unregister(basic_block* block)
{
    gr::thread::scoped_lock guard(d_mutex);

    d_map[block->name()].erase(d_map[block->name()].find(block->symbolic_id()));
    d_ref_map = pmt::dict_delete(d_ref_map, pmt::intern(block->symbol_name()));
    if (block->alias_set()) {
        d_ref_map = pmt::dict_delete(d_ref_map, pmt::intern(block->alias()));
    }
}

std::string block_registry::register_symbolic_name(basic_block* block)
{
    std::stringstream ss;
    ss << block->name() << block->symbolic_id();
    // std::cout << "register_symbolic_name: " << ss.str() << std::endl;
    register_symbolic_name(block, ss.str());
    return ss.str();
}

void block_registry::register_symbolic_name(basic_block* block, std::string name)
{
    gr::thread::scoped_lock guard(d_mutex);

    if (pmt::dict_has_key(d_ref_map, pmt::intern(name))) {
        throw std::runtime_error("symbol already exists, can not re-use!");
    }
    d_ref_map = pmt::dict_add(d_ref_map, pmt::intern(name), pmt::make_any(block));
}

void block_registry::update_symbolic_name(basic_block* block, std::string name)
{
    gr::thread::scoped_lock guard(d_mutex);

    if (pmt::dict_has_key(d_ref_map, pmt::intern(name))) {
        throw std::runtime_error("symbol already exists, can not re-use!");
    }

    // If we don't already have an alias, don't try and delete it.
    if (block->alias_set()) {
        // And make sure that the registry has the alias key.
        // We test both in case the block's and registry ever get out of sync.
        if (pmt::dict_has_key(d_ref_map, block->alias_pmt())) {
            d_ref_map = pmt::dict_delete(d_ref_map, block->alias_pmt());
        }
    }
    d_ref_map = pmt::dict_add(d_ref_map, pmt::intern(name), pmt::make_any(block));
}

basic_block_sptr block_registry::block_lookup(pmt::pmt_t symbol)
{
    gr::thread::scoped_lock guard(d_mutex);

    pmt::pmt_t ref = pmt::dict_ref(d_ref_map, symbol, pmt::PMT_NIL);
    if (pmt::eq(ref, pmt::PMT_NIL)) {
        throw std::runtime_error("block lookup failed! block not found!");
    }
    basic_block* blk = std::any_cast<basic_block*>(pmt::any_ref(ref));
    return blk->shared_from_this();
}

void block_registry::register_primitive(std::string blk, block* ref)
{
    gr::thread::scoped_lock guard(d_mutex);

    primitive_map[blk] = ref;
}

void block_registry::unregister_primitive(std::string blk)
{
    gr::thread::scoped_lock guard(d_mutex);

    primitive_map.erase(primitive_map.find(blk));
}

void block_registry::notify_blk(std::string blk)
{
    gr::thread::scoped_lock guard(d_mutex);

    if (primitive_map.find(blk) == primitive_map.end()) {
        throw std::runtime_error("block notify failed: block not found!");
    }
    if (primitive_map[blk]->detail().get()) {
        primitive_map[blk]->detail()->d_tpb.notify_msg();
    } else {
        // not having block detail is not necessarily a problem; this will happen when
        // publishing a message to a block that exists but has not yet been started
    }
}

} /* namespace gr */

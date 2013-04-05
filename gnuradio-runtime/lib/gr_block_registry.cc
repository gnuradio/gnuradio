/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
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

#include <gr_basic_block.h>
#include <gr_block_registry.h>
#include <gr_tpb_detail.h>
#include <gr_block_detail.h>
#include <gr_block.h>
#include <stdio.h>

gr_block_registry global_block_registry;

gr_block_registry::gr_block_registry(){
    d_ref_map = pmt::make_dict();
}

long gr_block_registry::block_register(gr_basic_block* block){
    if(d_map.find(block->name()) == d_map.end()){
        d_map[block->name()] = blocksubmap_t();
        d_map[block->name()][0] = block;
        return 0;
    } else {
        for(size_t i=0; i<=d_map[block->name()].size(); i++){
            if(d_map[block->name()].find(i) == d_map[block->name()].end()){
                d_map[block->name()][i] = block;
                return i;
            }
        }
    }
    throw std::runtime_error("should not reach this");
}

void gr_block_registry::block_unregister(gr_basic_block* block){
    d_map[block->name()].erase( d_map[block->name()].find(block->symbolic_id()));
    d_ref_map = pmt::dict_delete(d_ref_map, pmt::intern(block->symbol_name()));
    if(block->alias_set()){
        d_ref_map = pmt::dict_delete(d_ref_map, pmt::intern(block->alias()));
    }
}

std::string gr_block_registry::register_symbolic_name(gr_basic_block* block){
    std::stringstream ss;
    ss << block->name() << block->symbolic_id();
    //std::cout << "register_symbolic_name: " << ss.str() << std::endl;
    register_symbolic_name(block, ss.str());
    return ss.str();
}

void gr_block_registry::register_symbolic_name(gr_basic_block* block, std::string name){
    if(pmt::dict_has_key(d_ref_map, pmt::intern(name))){
        throw std::runtime_error("symbol already exists, can not re-use!");
        }
    d_ref_map = pmt::dict_add(d_ref_map, pmt::intern(name), pmt::make_any(block));
}

gr_basic_block_sptr gr_block_registry::block_lookup(pmt::pmt_t symbol){
    pmt::pmt_t ref = pmt::dict_ref(d_ref_map, symbol, pmt::PMT_NIL);
    if(pmt::eq(ref, pmt::PMT_NIL)){
        throw std::runtime_error("block lookup failed! block not found!");
    }
    gr_basic_block* blk = boost::any_cast<gr_basic_block*>( pmt::any_ref(ref) );
    return blk->shared_from_this();
}


void gr_block_registry::register_primitive(std::string blk, gr_block* ref){
    primitive_map[blk] = ref;
}

void gr_block_registry::unregister_primitive(std::string blk){
    primitive_map.erase(primitive_map.find(blk));
}

void gr_block_registry::notify_blk(std::string blk){
    if(primitive_map.find(blk) == primitive_map.end()){ return; }
    if(primitive_map[blk]->detail().get())
        primitive_map[blk]->detail()->d_tpb.notify_msg();
}


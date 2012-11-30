#include <gr_basic_block.h>
#include <gr_block_registry.h>
#include <gr_tpb_detail.h>
#include <gr_block_detail.h>
#include <gr_block.h>
#include <stdio.h>

gr_block_registry global_block_registry;

gr_block_registry::gr_block_registry(){
    d_ref_map = pmt::pmt_make_dict();
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
    d_ref_map = pmt::pmt_dict_delete(d_ref_map, pmt::pmt_intern(block->symbol_name()));
    if(block->alias_set()){
        d_ref_map = pmt::pmt_dict_delete(d_ref_map, pmt::pmt_intern(block->alias()));
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
    if(pmt_dict_has_key(d_ref_map, pmt::pmt_intern(name))){
        throw std::runtime_error("symbol already exists, can not re-use!");
        }
    d_ref_map = pmt_dict_add(d_ref_map, pmt::pmt_intern(name), pmt::pmt_make_any(block));
}

gr_basic_block_sptr gr_block_registry::block_lookup(pmt::pmt_t symbol){
    pmt::pmt_t ref = pmt_dict_ref(d_ref_map, symbol, pmt::PMT_NIL);
    if(pmt::pmt_eq(ref, pmt::PMT_NIL)){
        throw std::runtime_error("block lookup failed! block not found!");
    }
    gr_basic_block* blk = boost::any_cast<gr_basic_block*>( pmt::pmt_any_ref(ref) );
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


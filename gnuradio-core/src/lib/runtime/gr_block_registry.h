#ifndef GR_BLOCK_REGISTRY_H
#define GR_BLOCK_REGISTRY_H

#include <map>
#include <gr_basic_block.h>

#ifndef GR_BASIC_BLOCK_H
class gr_basic_block;
class gr_block;
#endif

class gr_block_registry {
    public:
        gr_block_registry();

        long block_register(gr_basic_block* block);
        void block_unregister(gr_basic_block* block);

        std::string register_symbolic_name(gr_basic_block* block);
        void register_symbolic_name(gr_basic_block* block, std::string name);

        gr_basic_block_sptr block_lookup(pmt::pmt_t symbol);
 
        void register_primitive(std::string blk, gr_block* ref);
        void unregister_primitive(std::string blk);
        void notify_blk(std::string blk);

    private:
     
        //typedef std::map< long, gr_basic_block_sptr >   blocksubmap_t;
        typedef std::map< long, gr_basic_block* >   blocksubmap_t;
        typedef std::map< std::string, blocksubmap_t >  blockmap_t;

        blockmap_t d_map;
        pmt::pmt_t d_ref_map;
        std::map< std::string, gr_block*> primitive_map;
 
};

extern gr_block_registry global_block_registry;

#endif


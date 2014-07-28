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

#ifndef GR_RUNTIME_BLOCK_REGISTRY_H
#define GR_RUNTIME_BLOCK_REGISTRY_H

#include <gnuradio/api.h>
#include <gnuradio/basic_block.h>
#include <map>

namespace gr {

#ifndef GR_BASIC_BLOCK_H
  class basic_block;
  class block;
#endif

  class GR_RUNTIME_API block_registry
  {
  public:
    block_registry();

    long block_register(basic_block* block);
    void block_unregister(basic_block* block);

    std::string register_symbolic_name(basic_block* block);
    void register_symbolic_name(basic_block* block, std::string name);
    void update_symbolic_name(basic_block* block, std::string name);

    basic_block_sptr block_lookup(pmt::pmt_t symbol);

    void register_primitive(std::string blk, gr::block* ref);
    void unregister_primitive(std::string blk);
    void notify_blk(std::string blk);

  private:
    //typedef std::map< long, basic_block_sptr >   blocksubmap_t;
    typedef std::map< long, basic_block* >   blocksubmap_t;
    typedef std::map< std::string, blocksubmap_t >  blockmap_t;

    blockmap_t d_map;
    pmt::pmt_t d_ref_map;
    std::map< std::string, block*> primitive_map;
    gr::thread::mutex d_mutex;
  };

} /* namespace gr */

GR_RUNTIME_API extern gr::block_registry global_block_registry;

#endif /* GR_RUNTIME_BLOCK_REGISTRY_H */

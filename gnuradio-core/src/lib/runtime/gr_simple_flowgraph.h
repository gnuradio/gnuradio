/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

#ifndef INCLUDED_GR_SIMPLE_FLOWGRAPH_H
#define INCLUDED_GR_SIMPLE_FLOWGRAPH_H

#include <gr_block.h>
#include <boost/shared_ptr.hpp>
#include <string>

gr_simple_flowgraph_sptr gr_make_simple_flowgraph();

class gr_simple_flowgraph_detail;

class gr_simple_flowgraph
{
private:
    friend class gr_runtime_impl;
    friend gr_simple_flowgraph_sptr gr_make_simple_flowgraph();
    gr_simple_flowgraph();

    gr_simple_flowgraph_detail *d_detail;
            
public:
    ~gr_simple_flowgraph();

    void define_component(const std::string &name, gr_block_sptr block);    
    void connect(const std::string &src, int src_port, 
                 const std::string &dst, int dst_port);
    void validate();
};

#endif /* INCLUDED_GR_SIMPLE_FLOWGRAPH_H */

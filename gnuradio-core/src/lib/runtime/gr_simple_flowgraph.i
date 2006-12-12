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

class gr_simple_flowgraph;
typedef boost::shared_ptr<gr_simple_flowgraph> gr_simple_flowgraph_sptr;
%template(gr_simple_flowgraph_sptr) boost::shared_ptr<gr_simple_flowgraph>;
%rename(simple_flowgraph) gr_make_simple_flowgraph;
%ignore gr_simple_flowgraph;

gr_simple_flowgraph_sptr gr_make_simple_flowgraph();

class gr_simple_flowgraph 
{
private:
    gr_simple_flowgraph();

public:
    ~gr_simple_flowgraph();
    void define_component(const std::string name, gr_block_sptr block)
        throw (std::invalid_argument);
    void connect(const std::string &src, int src_port,
                 const std::string &dst, int dst_port)
        throw (std::invalid_argument);
    void validate()
        throw (std::runtime_error);
};

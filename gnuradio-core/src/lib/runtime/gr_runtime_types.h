/* -*- c++ -*- */
/*
 * Copyright 2004,2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_RUNTIME_TYPES_H
#define INCLUDED_GR_RUNTIME_TYPES_H

#include <gr_core_api.h>
#include <gr_types.h>

/*
 * typedefs for smart pointers we use throughout the runtime system
 */

class gr_basic_block;
class gr_block;
class gr_block_detail;
class gr_hier_block2;
class gr_io_signature;
class gr_buffer;
class gr_buffer_reader;
class gr_flowgraph;
class gr_flat_flowgraph;
class gr_top_block;
class gr_top_block_detail;

typedef boost::shared_ptr<gr_basic_block>       gr_basic_block_sptr;
typedef boost::shared_ptr<gr_block>             gr_block_sptr;
typedef boost::shared_ptr<gr_block_detail>	gr_block_detail_sptr;
typedef boost::shared_ptr<gr_hier_block2>	gr_hier_block2_sptr;
typedef boost::shared_ptr<gr_io_signature>      gr_io_signature_sptr;
typedef boost::shared_ptr<gr_buffer>		gr_buffer_sptr;
typedef boost::shared_ptr<gr_buffer_reader>	gr_buffer_reader_sptr;
typedef boost::shared_ptr<gr_flowgraph>         gr_flowgraph_sptr;
typedef boost::shared_ptr<gr_flat_flowgraph>    gr_flat_flowgraph_sptr;
typedef boost::shared_ptr<gr_top_block>         gr_top_block_sptr;

#endif /* INCLUDED_GR_RUNTIME_TYPES_H */

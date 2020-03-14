/* -*- c++ -*- */
/*
 * Copyright 2004,2007 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RUNTIME_TYPES_H
#define INCLUDED_GR_RUNTIME_TYPES_H

#include <gnuradio/api.h>
#include <gnuradio/types.h>

namespace gr {

/*
 * typedefs for smart pointers we use throughout the runtime system
 */
class basic_block;
class block;
class block_detail;
class buffer;
class buffer_reader;
class hier_block2;
class flat_flowgraph;
class flowgraph;
class top_block;

typedef std::shared_ptr<basic_block> basic_block_sptr;
typedef std::shared_ptr<block> block_sptr;
typedef std::shared_ptr<block_detail> block_detail_sptr;
typedef std::shared_ptr<buffer> buffer_sptr;
typedef std::shared_ptr<buffer_reader> buffer_reader_sptr;
typedef std::shared_ptr<hier_block2> hier_block2_sptr;
typedef std::shared_ptr<flat_flowgraph> flat_flowgraph_sptr;
typedef std::shared_ptr<flowgraph> flowgraph_sptr;
typedef std::shared_ptr<top_block> top_block_sptr;

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_TYPES_H */

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

using basic_block_sptr = std::shared_ptr<basic_block>;
using block_sptr = std::shared_ptr<block>;
using block_detail_sptr = std::shared_ptr<block_detail>;
using buffer_sptr = std::shared_ptr<buffer>;
using buffer_reader_sptr = std::shared_ptr<buffer_reader>;
using hier_block2_sptr = std::shared_ptr<hier_block2>;
using flat_flowgraph_sptr = std::shared_ptr<flat_flowgraph>;
using flowgraph_sptr = std::shared_ptr<flowgraph>;
using top_block_sptr = std::shared_ptr<top_block>;

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_TYPES_H */

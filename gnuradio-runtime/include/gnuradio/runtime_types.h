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

  typedef boost::shared_ptr<basic_block>    basic_block_sptr;
  typedef boost::shared_ptr<block>          block_sptr;
  typedef boost::shared_ptr<block_detail>   block_detail_sptr;
  typedef boost::shared_ptr<buffer>         buffer_sptr;
  typedef boost::shared_ptr<buffer_reader>  buffer_reader_sptr;
  typedef boost::shared_ptr<hier_block2>    hier_block2_sptr;
  typedef boost::shared_ptr<flat_flowgraph> flat_flowgraph_sptr;
  typedef boost::shared_ptr<flowgraph>      flowgraph_sptr;
  typedef boost::shared_ptr<top_block>      top_block_sptr;

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_TYPES_H */

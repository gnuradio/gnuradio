/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2009,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_RUNTIME_HIER_BLOCK2_DETAIL_H
#define INCLUDED_GR_RUNTIME_HIER_BLOCK2_DETAIL_H

#include <gnuradio/api.h>
#include <gnuradio/hier_block2.h>
#include <flat_flowgraph.h>
#include <boost/utility.hpp>

namespace gr {

  /*!
   * \ingroup internal
   */
  class GR_RUNTIME_API hier_block2_detail : boost::noncopyable
  {
  public:
    hier_block2_detail(hier_block2 *owner);
    ~hier_block2_detail();

    void connect(basic_block_sptr block);
    void connect(basic_block_sptr src, int src_port,
                 basic_block_sptr dst, int dst_port);
    void msg_connect(basic_block_sptr src, pmt::pmt_t srcport,
                     basic_block_sptr dst, pmt::pmt_t dstport);
    void msg_disconnect(basic_block_sptr src, pmt::pmt_t srcport,
                        basic_block_sptr dst, pmt::pmt_t dstport);
    void disconnect(basic_block_sptr block);
    void disconnect(basic_block_sptr, int src_port,
                    basic_block_sptr, int dst_port);
    void disconnect_all();
    void lock();
    void unlock();
    void flatten_aux(flat_flowgraph_sptr sfg) const;

    void set_processor_affinity(const std::vector<int> &mask);
    void unset_processor_affinity();
    std::vector<int> processor_affinity();
    
    // Track output buffer min/max settings
    std::vector<size_t> d_max_output_buffer;
    std::vector<size_t> d_min_output_buffer;

  private:
    // Private implementation data
    hier_block2 *d_owner;
    hier_block2_detail *d_parent_detail;
    flowgraph_sptr d_fg;
    std::vector<endpoint_vector_t> d_inputs; // Multiple internal endpoints per external input
    endpoint_vector_t d_outputs;             // Single internal endpoint per external output
    basic_block_vector_t d_blocks;

    void refresh_io_signature();
    void connect_input(int my_port, int port, basic_block_sptr block);
    void connect_output(int my_port, int port, basic_block_sptr block);
    void disconnect_input(int my_port, int port, basic_block_sptr block);
    void disconnect_output(int my_port, int port, basic_block_sptr block);

    endpoint_vector_t resolve_port(int port, bool is_input);
    endpoint_vector_t resolve_endpoint(const endpoint &endp, bool is_input) const;
  };

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_HIER_BLOCK2_DETAIL_H */

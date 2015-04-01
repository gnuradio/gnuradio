/* -*- c++ -*- */
/*
 * Copyright 2006-2008,2013 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/hier_block2.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/flowgraph.h>
#include "hier_block2_detail.h"
#include <iostream>

namespace gr {

#define GR_HIER_BLOCK2_DEBUG 0

  hier_block2_sptr
  make_hier_block2(const std::string &name,
                   gr::io_signature::sptr input_signature,
                   gr::io_signature::sptr output_signature)
  {
    return gnuradio::get_initial_sptr
      (new hier_block2(name, input_signature, output_signature));
  }

  hier_block2::hier_block2(const std::string &name,
                           gr::io_signature::sptr input_signature,
                           gr::io_signature::sptr output_signature)
    : basic_block(name, input_signature, output_signature),
      d_detail(new hier_block2_detail(this)),
      hier_message_ports_in(pmt::PMT_NIL),
      hier_message_ports_out(pmt::PMT_NIL)
  {
    // This bit of magic ensures that self() works in the constructors of derived classes.
    gnuradio::detail::sptr_magic::create_and_stash_initial_sptr(this);
  }

  hier_block2::~hier_block2()
  {
    delete d_detail;
  }

  hier_block2::opaque_self
  hier_block2::self()
  {
    return shared_from_this();
  }

  hier_block2_sptr
  hier_block2::to_hier_block2()
  {
    return cast_to_hier_block2_sptr(shared_from_this());
  }

  void
  hier_block2::connect(basic_block_sptr block)
  {
    d_detail->connect(block);
  }

  void
  hier_block2::connect(basic_block_sptr src, int src_port,
                       basic_block_sptr dst, int dst_port)
  {
    d_detail->connect(src, src_port, dst, dst_port);
  }

  void
  hier_block2::msg_connect(basic_block_sptr src, pmt::pmt_t srcport,
                           basic_block_sptr dst, pmt::pmt_t dstport)
  {
    if(!pmt::is_symbol(srcport)) {
      throw std::runtime_error("bad port id");
    }
    d_detail->msg_connect(src, srcport, dst, dstport);
  }

  void
  hier_block2::msg_connect(basic_block_sptr src, std::string srcport,
                           basic_block_sptr dst, std::string dstport)
  {
    d_detail->msg_connect(src, pmt::mp(srcport), dst, pmt::mp(dstport));
  }

  void
  hier_block2::msg_disconnect(basic_block_sptr src, pmt::pmt_t srcport,
                              basic_block_sptr dst, pmt::pmt_t dstport)
  {
    if(!pmt::is_symbol(srcport)) {
      throw std::runtime_error("bad port id");
    }
    d_detail->msg_disconnect(src, srcport, dst, dstport);
  }

  void
  hier_block2::msg_disconnect(basic_block_sptr src, std::string srcport,
                              basic_block_sptr dst, std::string dstport)
  {
    d_detail->msg_disconnect(src, pmt::mp(srcport), dst, pmt::mp(dstport));
  }

  void
  hier_block2::disconnect(basic_block_sptr block)
  {
    d_detail->disconnect(block);
  }

  void
  hier_block2::disconnect(basic_block_sptr src, int src_port,
                          basic_block_sptr dst, int dst_port)
  {
    d_detail->disconnect(src, src_port, dst, dst_port);
  }

  void
  hier_block2::disconnect_all()
  {
    d_detail->disconnect_all();
  }

  void
  hier_block2::lock()
  {
    d_detail->lock();
  }

  void
  hier_block2::unlock()
  {
    d_detail->unlock();
  }

  flat_flowgraph_sptr
  hier_block2::flatten() const
  {
    flat_flowgraph_sptr new_ffg = make_flat_flowgraph();
    d_detail->flatten_aux(new_ffg);
    return new_ffg;
  }

  void
  hier_block2::set_processor_affinity(const std::vector<int> &mask)
  {
    d_detail->set_processor_affinity(mask);
  }

  void
  hier_block2::unset_processor_affinity()
  {
    d_detail->unset_processor_affinity();
  }

  std::vector<int>
  hier_block2::processor_affinity()
  {
    return d_detail->processor_affinity();
  }

  std::string
  dot_graph(hier_block2_sptr hierblock2)
  {
    return dot_graph_fg(hierblock2->flatten());
  }

  size_t
  hier_block2::max_output_buffer(size_t i)
  {
    if(i >= d_detail->d_max_output_buffer.size())
      throw std::invalid_argument("hier_block2::max_output_buffer: port out of range.");
    return d_detail->d_max_output_buffer[i];
  }
  
  void
  hier_block2::set_max_output_buffer(size_t max_output_buffer)
  {
    if(output_signature()->max_streams()>0)
    {
      if(d_detail->d_max_output_buffer.size() == 0)
        throw std::length_error("hier_block2::max_output_buffer: out_sig greater than zero, buff_vect isn't");
      for(size_t idx = 0; idx < output_signature()->max_streams(); idx++){
        d_detail->d_max_output_buffer[idx] = max_output_buffer;
      }
    }
  }
  
  void
  hier_block2::set_max_output_buffer(int port, size_t max_output_buffer)
  {
    if((size_t)port >= d_detail->d_max_output_buffer.size())
      throw std::invalid_argument("hier_block2::max_output_buffer: port out of range.");
    else{
      d_detail->d_max_output_buffer[port] = max_output_buffer;
    }
  }

  size_t
  hier_block2::min_output_buffer(size_t i)
  {
    if(i >= d_detail->d_min_output_buffer.size())
      throw std::invalid_argument("hier_block2::min_output_buffer: port out of range.");
    return d_detail->d_min_output_buffer[i];
  }

  void
  hier_block2::set_min_output_buffer(size_t min_output_buffer)
  {
    if(output_signature()->max_streams()>0)
    {
      if(d_detail->d_min_output_buffer.size() == 0)
        throw std::length_error("hier_block2::min_output_buffer: out_sig greater than zero, buff_vect isn't");
      for(size_t idx = 0; idx < output_signature()->max_streams(); idx++){
        d_detail->d_min_output_buffer[idx] = min_output_buffer;
      }
    }
  }

  void
  hier_block2::set_min_output_buffer(int port, size_t min_output_buffer)
  {
    if((size_t)port >= d_detail->d_min_output_buffer.size())
      throw std::invalid_argument("hier_block2::min_output_buffer: port out of range.");
    else{
      d_detail->d_min_output_buffer[port] = min_output_buffer;
    }
  }
  
  bool
  hier_block2::all_min_output_buffer_p(void)
  {
    if(d_detail->d_min_output_buffer.size() > 0){
      bool all_equal = true;
      for(int idx = 1; (idx < d_detail->d_min_output_buffer.size()) && all_equal; idx++){
        if(d_detail->d_min_output_buffer[0] != d_detail->d_min_output_buffer[idx])
          all_equal = false;
      }
      return all_equal;
    }
    else{
      return false;
    }
  }
  bool
  hier_block2::all_max_output_buffer_p(void)
  {
    if(d_detail->d_max_output_buffer.size() > 0){
      bool all_equal = true;
      for(int idx = 1; (idx < d_detail->d_max_output_buffer.size()) && all_equal; idx++){
        if(d_detail->d_max_output_buffer[0] != d_detail->d_max_output_buffer[idx])
          all_equal = false;
      }
      return all_equal;
    }
    else{
      return false;
    }
  }

} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2005-2007,2013 Free Software Foundation, Inc.
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

%include <basic_block.i>

class gr::hier_block2;
typedef boost::shared_ptr<gr::hier_block2> gr::hier_block2_sptr;
%template(hier_block2_sptr) boost::shared_ptr<gr::hier_block2>;

namespace gr {
  // Hack to have a Python shim implementation of gr.hier_block2
  // that instantiates one of these and passes through calls
  %rename(hier_block2_swig) make_hier_block2;
  gr::hier_block2_sptr
  make_hier_block2(const std::string name,
                   gr::io_signature::sptr input_signature,
                   gr::io_signature::sptr output_signature)
  throw (std::runtime_error);
}

// Rename connect and disconnect so that we can more easily build a
// better interface in scripting land.
%rename(primitive_connect) gr::hier_block2::connect;
%rename(primitive_disconnect) gr::hier_block2::disconnect;
%rename(primitive_msg_connect) gr::hier_block2::msg_connect;
%rename(primitive_msg_disconnect) gr::hier_block2::msg_disconnect;
%rename(primitive_message_port_register_hier_in) gr::hier_block2::message_port_register_hier_in;
%rename(primitive_message_port_register_hier_out) gr::hier_block2::message_port_register_hier_out;

namespace gr {
  class hier_block2 : public gr::basic_block
  {
  private:
    hier_block2(const std::string name,
                gr::io_signature::sptr input_signature,
                gr::io_signature::sptr output_signature);

  public:
    ~hier_block2 ();

    void connect(gr::basic_block_sptr block)
      throw (std::invalid_argument);
    void connect(gr::basic_block_sptr src, int src_port,
                 gr::basic_block_sptr dst, int dst_port)
      throw (std::invalid_argument);
    void msg_connect(gr::basic_block_sptr src, pmt::pmt_t srcport,
                     gr::basic_block_sptr dst, pmt::pmt_t dstport)
      throw (std::runtime_error);
    void msg_connect(gr::basic_block_sptr src, std::string srcport,
                     gr::basic_block_sptr dst,  std::string dstport)
      throw (std::runtime_error);
    void msg_disconnect(gr::basic_block_sptr src, pmt::pmt_t srcport,
                        gr::basic_block_sptr dst, pmt::pmt_t dstport)
      throw (std::runtime_error);
    void msg_disconnect(gr::basic_block_sptr src, std::string srcport,
                        gr::basic_block_sptr dst, std::string dstport)
      throw (std::runtime_error);

    void disconnect(gr::basic_block_sptr block)
      throw (std::invalid_argument);
    void disconnect(gr::basic_block_sptr src, int src_port,
                    gr::basic_block_sptr dst, int dst_port)
      throw (std::invalid_argument);
    void disconnect_all();
    void lock();
    void unlock();

    void message_port_register_hier_in(pmt::pmt_t port_id);
    void message_port_register_hier_out(pmt::pmt_t port_id);

    void set_processor_affinity(const std::vector<int> &mask);
    void unset_processor_affinity();
    std::vector<int> processor_affinity();

    // Methods to manage block's min/max buffer sizes.
    size_t max_output_buffer(int i);
    void set_max_output_buffer(size_t max_output_buffer);
    void set_max_output_buffer(int port, size_t max_output_buffer);
    size_t min_output_buffer(int i);
    void set_min_output_buffer(size_t min_output_buffer);
    void set_min_output_buffer(int port, size_t min_output_buffer);

    gr::hier_block2_sptr to_hier_block2(); // Needed for Python type coercion
  };

  std::string dot_graph(hier_block2_sptr hierblock2);
}

/* -*- c++ -*- */
/*
 * Copyright 2005-2007,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

%include <basic_block.i>

class gr::hier_block2;
typedef std::shared_ptr<gr::hier_block2> gr::hier_block2_sptr;
%template(hier_block2_sptr) std::shared_ptr<gr::hier_block2>;

namespace gr {
  // Hack to have a Python shim implementation of gr.hier_block2
  // that instantiates one of these and passes through calls
  %rename(hier_block2_swig) make_hier_block2;
  gr::hier_block2_sptr
  make_hier_block2(const std::string name,
                   gr::io_signature::sptr input_signature,
                   gr::io_signature::sptr output_signature)
    noexcept(false);
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
      noexcept(false);
    void connect(gr::basic_block_sptr src, int src_port,
                 gr::basic_block_sptr dst, int dst_port)
      noexcept(false);
    void msg_connect(gr::basic_block_sptr src, pmt::pmt_t srcport,
                     gr::basic_block_sptr dst, pmt::pmt_t dstport)
      noexcept(false);
    void msg_connect(gr::basic_block_sptr src, std::string srcport,
                     gr::basic_block_sptr dst,  std::string dstport)
      noexcept(false);
    void msg_disconnect(gr::basic_block_sptr src, pmt::pmt_t srcport,
                        gr::basic_block_sptr dst, pmt::pmt_t dstport)
      noexcept(false);
    void msg_disconnect(gr::basic_block_sptr src, std::string srcport,
                        gr::basic_block_sptr dst, std::string dstport)
      noexcept(false);

    void disconnect(gr::basic_block_sptr block)
      noexcept(false);
    void disconnect(gr::basic_block_sptr src, int src_port,
                    gr::basic_block_sptr dst, int dst_port)
      noexcept(false);
    void disconnect_all();
    void lock();
    void unlock();

    void message_port_register_hier_in(pmt::pmt_t port_id);
    void message_port_register_hier_out(pmt::pmt_t port_id);

    void set_processor_affinity(const std::vector<int> &mask);
    void unset_processor_affinity();
    std::vector<int> processor_affinity();

    void set_log_level(std::string level);
    std::string log_level();

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

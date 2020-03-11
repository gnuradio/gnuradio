/* -*- c++ -*- */
/*
 * Copyright 2006,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

class gr::basic_block;
typedef boost::shared_ptr<gr::basic_block> gr::basic_block_sptr;
%template(basic_block_sptr) boost::shared_ptr<gr::basic_block>;

%import "pmt_swig.i"

// support vectors of these...
namespace std {
  %template(x_vector_basic_block_sptr) vector<gr::basic_block_sptr>;
};

namespace gr {

  class gr::basic_block
  {
  protected:
    basic_block();

  public:
    virtual ~basic_block();
    std::string name() const;
    std::string symbol_name() const;
    gr::io_signature::sptr input_signature() const;
    gr::io_signature::sptr output_signature() const;
    long unique_id() const;
    gr::basic_block_sptr to_basic_block();
    bool check_topology(int ninputs, int noutputs);
    std::string alias();
    void set_block_alias(std::string name);
    void _post(pmt::pmt_t which_port, pmt::pmt_t msg);
    pmt::pmt_t message_ports_in();
    pmt::pmt_t message_ports_out();
    pmt::pmt_t message_subscribers(pmt::pmt_t which_port);
  };

  %rename(block_ncurrently_allocated) basic_block_ncurrently_allocated;
  long basic_block_ncurrently_allocated();
}

#ifdef SWIGPYTHON
%pythoncode %{
basic_block_sptr.__repr__ = lambda self: "<basic_block %s (%d)>" % (self.name(), self.unique_id ())
%}
#endif

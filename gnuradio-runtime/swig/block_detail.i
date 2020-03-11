/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

class gr::block_detail;
typedef boost::shared_ptr<gr::block_detail> gr::block_detail_sptr;
%template(block_detail_sptr) boost::shared_ptr<gr::block_detail>;
%rename(block_detail) gr::make_block_detail;
%ignore gr::block_detail;

gr::block_detail_sptr gr::
gr::make_block_detail(unsigned int ninputs, unsigned int noutputs);

namespace gr {

  class gr::block_detail {
  public:

    ~block_detail ();

    int ninputs() const;
    int noutputs() const;
    bool sink_p() const;
    bool source_p() const;

    void set_input(unsigned int which, gr::buffer_reader_sptr reader);
    gr::buffer_reader_sptr input(unsigned int which);

    void set_output(unsigned int which, gr::buffer_sptr buffer);
    gr::buffer_sptr output(unsigned int which);

  private:
    block_detail(unsigned int ninputs, unsigned int noutputs);
  };

  %rename(block_detail_ncurrently_allocated) block_detail_ncurrently_allocated;
  long block_detail_ncurrently_allocated();
}

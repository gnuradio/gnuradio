/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

class gr::buffer;
typedef boost::shared_ptr<gr::buffer> gr::buffer_sptr;
%template(buffer_sptr) boost::shared_ptr<gr::buffer>;
%rename(buffer) gr::make_buffer;
%ignore gr::buffer;

gr::buffer_sptr
gr::make_buffer (int nitems, size_t sizeof_item, gr::block_sptr link);

class gr::buffer_reader;
typedef boost::shared_ptr<gr::buffer_reader> gr::buffer_reader_sptr;
%template(buffer_reader_sptr) boost::shared_ptr<gr::buffer_reader>;
%ignore gr::buffer_reader;

%rename(buffer_add_reader) gr::buffer_add_reader;
gr::buffer_reader_sptr
gr::buffer_add_reader (gr::buffer_sptr buf, int nzero_preload, gr::block_sptr link);

namespace gr {

  class gr::buffer
  {
  public:
    ~buffer();

  private:
    buffer(int nitems, size_t sizeof_item, gr::block_sptr link);
  };

  class gr::buffer_reader
  {
  public:
    ~buffer_reader();

  private:
    friend class buffer;
    buffer_reader(gr::buffer_sptr buffer, unsigned int read_index, gr::block_sptr link);
  };

  %rename(buffer_ncurrently_allocated) buffer_ncurrently_allocated;
  long buffer_ncurrently_allocated();

  %rename(buffer_reader_ncurrently_allocated) buffer_reader_ncurrently_allocated;
  long buffer_reader_ncurrently_allocated();
}

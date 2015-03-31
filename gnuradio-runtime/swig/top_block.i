/* -*- c++ -*- */
/*
 * Copyright 2007,2008,2010,2013 Free Software Foundation, Inc.
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

%template(top_block_sptr) boost::shared_ptr<gr::top_block>;

namespace gr {
  // Hack to have a Python shim implementation of gr.top_block
  // that instantiates one of these and passes through calls
  %rename(top_block_swig) make_top_block;
  gr::top_block_sptr make_top_block(const std::string name)
    throw (std::logic_error);

  class top_block : public gr::hier_block2
  {
  private:
    top_block(const std::string &name);

  public:
    ~top_block();

    void start(int max_noutput_items=100000000) throw (std::runtime_error);
    void stop();
    //void wait();
    //void run() throw (std::runtime_error);
    void lock();
    void unlock() throw (std::runtime_error);
    std::string edge_list();
    std::string msg_edge_list();
    void dump();

    int max_noutput_items();
    void set_max_noutput_items(int nmax);

    gr::top_block_sptr to_top_block(); // Needed for Python type coercion
  };
}

#ifdef SWIGPYTHON

%inline %{
void top_block_run_unlocked(gr::top_block_sptr r) throw (std::runtime_error)
{
    GR_PYTHON_BLOCKING_CODE
    (
        r->run();
    )
}

void top_block_start_unlocked(gr::top_block_sptr r, int max_noutput_items) throw (std::runtime_error)
{
    GR_PYTHON_BLOCKING_CODE
    (
        r->start(max_noutput_items);
    )
}

void top_block_wait_unlocked(gr::top_block_sptr r) throw (std::runtime_error)
{
    GR_PYTHON_BLOCKING_CODE
    (
        r->wait();
    )
}

void top_block_stop_unlocked(gr::top_block_sptr r) throw (std::runtime_error)
{
    GR_PYTHON_BLOCKING_CODE
    (
        r->stop();
    )
}

void top_block_unlock_unlocked(gr::top_block_sptr r) throw (std::runtime_error)
{
    GR_PYTHON_BLOCKING_CODE
    (
        r->unlock();
    )
}

std::string
dot_graph_tb(gr::top_block_sptr r)
{
  return dot_graph(r);
}

%}

#endif

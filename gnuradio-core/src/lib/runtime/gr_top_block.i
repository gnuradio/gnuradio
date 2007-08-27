/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

%include <gr_top_block.i>

class gr_top_block;
typedef boost::shared_ptr<gr_top_block> gr_top_block_sptr;
%template(gr_top_block_sptr) boost::shared_ptr<gr_top_block>;

// Hack to have a Python shim implementation of gr.top_block
// that instantiates one of these and passes through calls
%rename(top_block_swig) gr_make_top_block;
gr_top_block_sptr gr_make_top_block(const std::string name);

class gr_top_block : public gr_hier_block2
{
private:
  gr_top_block(const std::string &name);
    
public:
  ~gr_top_block();

  void start();
  void stop();
  void wait();
  void run();
  void lock();
  void unlock();
};

%{
class ensure_py_gil_state2 {
    PyGILState_STATE	d_gstate;
public:
  ensure_py_gil_state2()  { d_gstate = PyGILState_Ensure(); }
  ~ensure_py_gil_state2() { PyGILState_Release(d_gstate); }
};
%}

%inline %{
void top_block_run_unlocked(gr_top_block_sptr r) throw (std::runtime_error) 
{
    ensure_py_gil_state2 _lock;
    r->run();
}

void top_block_wait_unlocked(gr_top_block_sptr r) throw (std::runtime_error) 
{
    ensure_py_gil_state2 _lock;
    r->wait();
}
%}

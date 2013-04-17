/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
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

#include <pmt/pmt.h>

namespace gr {

  class py_feval_dd : public feval_dd
  {
  public:
    double calleval(double x)
    {
      ensure_py_gil_state _lock;
      return eval(x);
    }
  };

  class py_feval_cc : public feval_cc
  {
  public:
    gr_complex calleval(gr_complex x)
    {
      ensure_py_gil_state _lock;
      return eval(x);
    }
  };

  class py_feval_ll : public feval_ll
  {
  public:
    long calleval(long x)
    {
      ensure_py_gil_state _lock;
      return eval(x);
    }
  };

  class py_feval : public feval
  {
  public:
    void calleval()
    {
      ensure_py_gil_state _lock;
      eval();
    }
  };

  class py_feval_p : public feval_p
  {
  public:
    void calleval(pmt::pmt_t x)
    {
      ensure_py_gil_state _lock;
      eval(x);
    }
  };

} /* namespace gr */

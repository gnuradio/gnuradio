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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/feval.h>

namespace gr {

  feval_dd::~feval_dd(){}

  double
  feval_dd::eval(double x)
  {
    return 0;
  }

  double
  feval_dd::calleval(double x)
  {
    return eval(x);
  }

  // ----------------------------------------------------------------

  feval_cc::~feval_cc(){}

  gr_complex
  feval_cc::eval(gr_complex x)
  {
    return 0;
  }

  gr_complex
  feval_cc::calleval(gr_complex x)
  {
    return eval(x);
  }

  // ----------------------------------------------------------------

  feval_ll::~feval_ll(){}

  long
  feval_ll::eval(long x)
  {
    return 0;
  }

  long
  feval_ll::calleval(long x)
  {
    return eval(x);
  }

  // ----------------------------------------------------------------

  feval::~feval(){}

  void
  feval::eval(void)
  {
    // nop
  }

  void
  feval::calleval(void)
  {
    eval();
  }

  // ----------------------------------------------------------------

  feval_p::~feval_p(){}

  void
  feval_p::eval(pmt::pmt_t x)
  {
    // nop
  }

  void
  feval_p::calleval(pmt::pmt_t x)
  {
    eval(x);
  }

  /*
   * Trivial examples showing C++ (transparently) calling Python
   */
  double
  feval_dd_example(feval_dd *f, double x)
  {
    return f->calleval(x);
  }

  gr_complex
  feval_cc_example(feval_cc *f, gr_complex x)
  {
    return f->calleval(x);
  }

  long
  feval_ll_example(feval_ll *f, long x)
  {
    return f->calleval(x);
  }

  void
  feval_example(feval *f)
  {
    f->calleval();
  }

} /* namespace gr */

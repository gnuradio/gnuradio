/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

#include <gr_feval.h>

gr_feval_dd::~gr_feval_dd(){}

double 
gr_feval_dd::eval(double x)
{
  return 0;
}

gr_feval_cc::~gr_feval_cc(){}

gr_complex
gr_feval_cc::eval(gr_complex x)
{
  return 0;
}

gr_feval_ll::~gr_feval_ll(){}

long
gr_feval_ll::eval(long x)
{
  return 0;
}

gr_feval::~gr_feval(){}

void
gr_feval::eval(void)
{
  // nop
}

/*
 * Trivial examples showing C++ (transparently) calling Python
 */
double
gr_feval_dd_example(gr_feval_dd *f, double x)
{
  return f->eval(x);
}

gr_complex
gr_feval_cc_example(gr_feval_cc *f, gr_complex x)
{
  return f->eval(x);
}

long
gr_feval_ll_example(gr_feval_ll *f, long x)
{
  return f->eval(x);
}

void
gr_feval_example(gr_feval *f)
{
  f->eval();
}

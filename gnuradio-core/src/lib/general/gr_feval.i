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

// Enable SWIG directors for these classes
%feature("director") gr_feval_dd;
%feature("director") gr_feval_cc;
%feature("director") gr_feval_ll;


%rename(feval_dd) gr_feval_dd;
class gr_feval_dd
{
public:
  gr_feval_dd() {}
  virtual ~gr_feval_dd();

  virtual double eval(double x);
};

%rename(feval_cc) gr_feval_cc;
class gr_feval_cc
{
public:
  gr_feval_cc() {}
  virtual ~gr_feval_cc();

  virtual gr_complex eval(gr_complex x);
};

%rename(feval_ll) gr_feval_ll;
class gr_feval_ll
{
public:
  gr_feval_ll() {}
  virtual ~gr_feval_ll();

  virtual long eval(long x);
};


// examples / test cases

%rename(feval_dd_example) gr_feval_dd_example;
double gr_feval_dd_example(gr_feval_dd *f, double x);

%rename(feval_cc_example) gr_feval_cc_example;
gr_complex gr_feval_cc_example(gr_feval_cc *f, gr_complex x);

%rename(feval_ll_example) gr_feval_ll_example;
long gr_feval_ll_example(gr_feval_ll *f, long x);

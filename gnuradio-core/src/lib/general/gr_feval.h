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
#ifndef INCLUDED_GR_FEVAL_H
#define INCLUDED_GR_FEVAL_H

#include <gr_complex.h>

/*!
 * \brief base class for evaluating a function: double -> double
 *
 * This class is designed to be subclassed in Python or C++
 * and is callable from both places.  It uses SWIG's
 * "director" feature to implement the magic.
 * It's slow. Don't use it in a performance critical path.
 */
class gr_feval_dd
{
public:
  gr_feval_dd() {}
  virtual ~gr_feval_dd();

  /*!
   * \brief override this to define the function
   */
  virtual double eval(double x);
};

/*!
 * \brief base class for evaluating a function: complex -> complex
 *
 * This class is designed to be subclassed in Python or C++
 * and is callable from both places.  It uses SWIG's
 * "director" feature to implement the magic.
 * It's slow. Don't use it in a performance critical path.
 */
class gr_feval_cc
{
public:
  gr_feval_cc() {}
  virtual ~gr_feval_cc();

  /*!
   * \brief override this to define the function
   */
  virtual gr_complex eval(gr_complex x);
};

/*!
 * \brief base class for evaluating a function: long -> long
 *
 * This class is designed to be subclassed in Python or C++
 * and is callable from both places.  It uses SWIG's
 * "director" feature to implement the magic.
 * It's slow. Don't use it in a performance critical path.
 */
class gr_feval_ll
{
public:
  gr_feval_ll() {}
  virtual ~gr_feval_ll();

  /*!
   * \brief override this to define the function
   */
  virtual long eval(long x);
};

/*!
 * \brief trivial examples / test cases showing C++ calling Python code
 */
double     gr_feval_dd_example(gr_feval_dd *f, double x);
gr_complex gr_feval_cc_example(gr_feval_cc *f, gr_complex x);
long       gr_feval_ll_example(gr_feval_ll *f, long x);


#endif /* INCLUDED_GR_FEVAL_H */

/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Example Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Example Public License for more details.
 * 
 * You should have received a copy of the GNU Example Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_QA_HOWTO_H
#define INCLUDED_QA_HOWTO_H

#include <cppunit/TestSuite.h>

//! collect all the tests for the example directory

class qa_howto {
 public:
  //! return suite of tests for all of example directory
  static CppUnit::TestSuite *suite ();
};

#endif /* INCLUDED_QA_HOWTO_H */

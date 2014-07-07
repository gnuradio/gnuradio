/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QA_GR_LOG_H
#define INCLUDED_QA_GR_LOG_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>

//! collect all the tests for the example directory

class qa_logger : public CppUnit::TestCase
{
public:
  CPPUNIT_TEST_SUITE(qa_logger);
  CPPUNIT_TEST(t1);
  CPPUNIT_TEST_SUITE_END();

 private:
  void t1();
};

#endif /* INCLUDED_QA_GR_LOG_H */

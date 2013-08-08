/* -*- c++ -*- */
/*
 * Copyright 2006,2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QA_GR_HIER_BLOCK2_DERIVED_H
#define INCLUDED_QA_GR_HIER_BLOCK2_DERIVED_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>
#include <stdexcept>

// Declare a QA test case
class qa_hier_block2_derived : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(qa_hier_block2_derived);
  CPPUNIT_TEST(test_1);
  CPPUNIT_TEST_SUITE_END();

private:
  void test_1();
};

#endif /* INCLUDED_QA_GR_HIER_BLOCK2_DERIVED_H */
